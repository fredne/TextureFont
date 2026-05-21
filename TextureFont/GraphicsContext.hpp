#pragma once
#include "Framework.hpp"

class GraphicsContext 
{
public:
    ID3D11Device* Device = nullptr;
    ID3D11DeviceContext* ImmediateContext = nullptr;
    IDXGISwapChain* SwapChain = nullptr;
    ID3D11RenderTargetView* RTV = nullptr;
    bool IsFullscreen = false;
    int VSync = 1;

    ~GraphicsContext() 
    {
        if (RTV) RTV->Release();
        if (SwapChain) SwapChain->Release();
        if (ImmediateContext) ImmediateContext->Release();
        if (Device) Device->Release();
    }

    bool InitDX(HWND hWnd, int w, int h) 
    {
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = w; sd.BufferDesc.Height = h;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd; sd.SampleDesc.Count = 1; sd.Windowed = TRUE;

        HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0,
            D3D11_SDK_VERSION, &sd, &SwapChain, &Device, NULL, &ImmediateContext);

        return SUCCEEDED(hr) && CreateRTV(w, h);
    }

    bool CreateRTV(int w, int h) 
    {
        if (RTV) RTV->Release();
        ID3D11Texture2D* pBB;
        SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBB);
        Device->CreateRenderTargetView(pBB, NULL, &RTV);
        pBB->Release();
        return true;
    }

    void Resize(int w, int h) 
    {
        ImmediateContext->OMSetRenderTargets(0, 0, 0);
        if (RTV) { RTV->Release(); RTV = nullptr; }
        SwapChain->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0);
        CreateRTV(w, h);
    }

    void SetFullscreen(bool goFull) 
    {
        IsFullscreen = goFull;
        SwapChain->SetFullscreenState(goFull, NULL);
    }





    ShaderSet CompileAndCreate(const void* source, size_t length, bool isFile, D3D11_INPUT_ELEMENT_DESC* ied, UINT iedCount)
    {
        ShaderSet res;
        ID3DBlob* vsBlob = nullptr;
        ID3DBlob* psBlob = nullptr;
        ID3DBlob* errBlob = nullptr;
        HRESULT hr;

        if (isFile)
        {
            // VS ������
            hr = D3DCompileFromFile((LPCWSTR)source, nullptr, nullptr, "VS", "vs_5_0", 0, 0, &vsBlob, &errBlob);
            if (FAILED(hr))
            {
                HandleCompileError(hr, errBlob, L"VS");
                if (vsBlob) vsBlob->Release();
                return res;
            }

            // PS ������
            hr = D3DCompileFromFile((LPCWSTR)source, nullptr, nullptr, "PS", "ps_5_0", 0, 0, &psBlob, &errBlob);
            if (FAILED(hr))
            {
                HandleCompileError(hr, errBlob, L"PS");
                if (psBlob) psBlob->Release();
                return res;
            }
        }
        else
        {
            // �޸� �ҽ� VS ������
            hr = D3DCompile(source, length, nullptr, nullptr, nullptr, "VS", "vs_5_0", 0, 0, &vsBlob, &errBlob);
            if (FAILED(hr))
            {
                HandleCompileError(hr, errBlob, L"VS");
                if (vsBlob) vsBlob->Release();
                return res;
            }

            // �޸� �ҽ� PS ������
            hr = D3DCompile(source, length, nullptr, nullptr, nullptr, "PS", "ps_5_0", 0, 0, &psBlob, &errBlob);
            if (FAILED(hr))
            {
                HandleCompileError(hr, errBlob, L"PS");
                if (psBlob) psBlob->Release();
                return res;
            }
        }

        // ���ҽ� ����
        Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &res.vs);
        Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &res.ps);

        if (vsBlob && ied)
        {
            Device->CreateInputLayout(ied, iedCount, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &res.layout);
        }

        if (vsBlob) vsBlob->Release();
        if (psBlob) psBlob->Release();

        return res;
    }

    /**
 * [���̴� �ε� ����]
 * 1. .cso ���� ���� ���� Ȯ�� -> ������ D3DReadFileToBlob���� ��� �ε� (���� ����ȭ)
 * 2. .cso ���� -> .hlsl ���� Ȯ�� -> D3DCompileFromFile�� �ǽð� ��� (������)
 * 3. �� �� ���� -> ���� �޽��� ��� �� �� ����ü ��ȯ
 */
 /**
* [Vertex Shader ���� �δ�]
* - CSO �켱 �õ� �� HLSL ������
* - Input Layout ���� ����
*/
    void LoadVertexShader(ShaderSet* res, const std::wstring& shaderName, D3D11_INPUT_ELEMENT_DESC* ied, UINT iedCount)
    {
        if (!res) return;

        ID3DBlob* vsBlob = nullptr;
        ID3DBlob* errBlob = nullptr;
        std::wstring csoPath = shaderName + L".cso";
        std::wstring hlslPath = shaderName + L".hlsl";

        // 1. CSO �õ�
        HRESULT hr = D3DReadFileToBlob(csoPath.c_str(), &vsBlob);

        // 2. ���� �� HLSL ������ �õ�
        if (FAILED(hr))
        {
            hr = D3DCompileFromFile(hlslPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                "main", "vs_5_0", 0, 0, &vsBlob, &errBlob);
            if (FAILED(hr))
            {
                HandleCompileError(hr, errBlob, hlslPath.c_str());
                return;
            }
        }

        // 3. ���ҽ� �� ���̾ƿ� ����
        if (vsBlob)
        {
            Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &res->vs);

            if (ied)
            {
                Device->CreateInputLayout(ied, iedCount, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &res->layout);
            }
            vsBlob->Release();
        }
    }

    /**
 * [Pixel Shader ���� �δ�]
 * - CSO �켱 �õ� �� HLSL ������
 */
    void LoadPixelShader(ShaderSet* res, const std::wstring& shaderName)
    {
        if (!res) return;

        ID3DBlob* psBlob = nullptr;
        ID3DBlob* errBlob = nullptr;
        std::wstring csoPath = shaderName + L".cso";
        std::wstring hlslPath = shaderName + L".hlsl";

        // 1. CSO �õ�
        HRESULT hr = D3DReadFileToBlob(csoPath.c_str(), &psBlob);

        // 2. ���� �� HLSL ������ �õ�
        if (FAILED(hr))
        {
            hr = D3DCompileFromFile(hlslPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                "main", "ps_5_0", 0, 0, &psBlob, &errBlob);
            if (FAILED(hr))
            {
                HandleCompileError(hr, errBlob, hlslPath.c_str());
                return;
            }
        }

        // 3. ���ҽ� ����
        if (psBlob)
        {
            Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &res->ps);
            psBlob->Release();
        }
    }

    // ���� ó���� ���� ���� �Լ�
    void HandleCompileError(HRESULT hr, ID3DBlob* errBlob, const wchar_t* path) 
    {
        if (errBlob) 
        {
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
            MessageBoxA(NULL, (char*)errBlob->GetBufferPointer(), "Shader Compile Error", MB_ICONERROR);
            errBlob->Release();
        }
        else if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) 
        {
            std::wstring msg = L"������ ã�� �� �����ϴ�: " + std::wstring(path);
            MessageBoxW(NULL, msg.c_str(), L"File Error", MB_ICONERROR);
        }
    }
};