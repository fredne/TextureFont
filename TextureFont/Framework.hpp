#pragma once

// [Framework.hpp] �ý��� ��� �� ���� ����ü
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <wincodec.h> // Windows �⺻ �̹��� ���ڵ� ���̺귯��

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

struct Text
{
    wchar_t text;
    float left;
    float top;
    float width;
    float height;
};

struct FontSetting
{
    float startX;
    float startY;
    float width;
    float height;
    float strideX;
};


// 1. ���� ������ ����ü
struct Vertex 
{
    XMFLOAT3 pos;
    XMFLOAT2 uv;
};

struct ConstantBuffer 
{
    XMMATRIX matWorld;
};


// 2. ���̴� ���ҽ� ����
struct ShaderSet 
{
    ID3D11VertexShader* vs = nullptr;
    ID3D11PixelShader* ps = nullptr;
    ID3D11InputLayout* layout = nullptr;

    ShaderSet() = default;
    ShaderSet(ID3D11VertexShader* v, ID3D11PixelShader* p, ID3D11InputLayout* l)
        : vs(v), ps(p), layout(l) 
    {    }

    void Release() 
    {
        if (vs) { vs->Release(); vs = nullptr; }
        if (ps) { ps->Release(); ps = nullptr; }
        if (layout) { layout->Release(); layout = nullptr; }
    }
};

/*
 * [���� ��Ʈ 1: �ؽ�ó(Texture)��?]
 * 1. �������� �����: �ؽ�ó�� �ܼ��� �̹��� ������ �ƴ϶�, GPU �޸𸮿� �ö� �ȼ� �迭�̾�.
 * 2. ���(SRV): GPU�� �ؽ�ó�� ���� ������ �ʾ�. 'Shader Resource View(SRV)'��� ��θ�
 *    ���ؼ��� �����͸� ���� �� ����. �� Ŭ������ �� ��θ� �����ϴ� �����ھ�.
 *
 * [���� ��Ʈ 2: ��������(Register) ��ȣ]
 * 1. t0, t1... : GPU�� ������ ���� �� �ؽ�ó�� ���� ��ġ�� �ǹ���.
 *    C++���� PSSetShaderResources(0, ...) ��� �ϸ� t0 ���Կ� ����ȴٴ� ������.
 *
 * [���� ��Ʈ 3: ���÷�(Sampler)��?]
 * 1. �ؼ��� ��Ģ: ������ ũ�� Ȯ���ϰų� �۰� ����� ��, �ȼ��� ��� ������ ���ϴ� ���;�.
 * 2. �� �ʿ��Ѱ�?: �Ȱ��� ������ ��Ȳ�� ���� �ε巴��(Linear) ����, ��Ʈó��(Point) ����
 *    �����ؾ� �ϱ� ��������.
 * 3. �Ӽ���:
 *    - Filter: �ȼ��� ���� ��� (MIN_MAG_MIP_LINEAR�� ���� �ε巯�� ���)
 *    - AddressU/V: �̹����� �߸� �� �ݺ�����(Wrap) Ȥ�� �����ڸ� �ȼ��� ä����(Clamp) ����.
 * 
 * 
 * 
 */

 /*
  * [���� ��Ʈ: ���÷� ������Ʈ(Sampler State) ���� ����]
  *
  * 1. ���÷��� ����:
  *    - �ؽ�ó(�̹���)�� �������� �����Ͱ� �ƴ� '�ȼ��� ����'��.
  *    - ���÷��� ���̴��� "�ؽ�ó�� �� ��ǥ(UV)�� ���� ���̾�?"��� ��� ��,
  *      �ֺ� �ȼ����� ��� �ؼ��ؼ� �˷����� �����ϴ� '�ؼ� ��Ģ'��.
  *
  * 2. ����(Filter) �ɼ�:
  *    - D3D11_FILTER_MIN_MAG_MIP_LINEAR:
  *        - ���� ���� ����. Ȯ��/��� �� ���� �ȼ����� ��հ��� ���(Linear)�Ͽ� �ε巴�� ǥ����.
  *    - D3D11_FILTER_MIN_MAG_MIP_POINT:
  *        - '��Ʈ �׷���' ������ �� �� �ʼ�. ���� ���� ���� ����� �ȼ� ���� �״�� ������.
  *    - D3D11_FILTER_ANISOTROPIC:
  *        - ���漺 ���͸�. �ٴ�ó�� �񽺵��ϰ� �����ִ� �ؽ�ó�� �ָ��� �� ��
  *          �������� ������ �����ϰ� �����ϰ� ������(���� �׷��ȿ�).
  *
  * 3. ��巹�� ���(AddressU/V/W):
  *    - UV ��ǥ�� [0, 1] ������ ��� ��(��: 1.5, -0.2) ��� ó������ ������.
  *    - D3D11_TEXTURE_ADDRESS_WRAP:
  *        - Ÿ��ó�� �ݺ��� (���� �⺻).
  *    - D3D11_TEXTURE_ADDRESS_CLAMP:
  *        - ���κ� �ȼ� ���� �� �÷��� ä�� (�̹����� �������� ���� ȿ�� ����).
  *    - D3D11_TEXTURE_ADDRESS_BORDER:
  *        - ������ ���� ����(BorderColor)���� ä��.
  *
  * 4. ��Ÿ ����:
  *    - ComparisonFunc:
  *        - �ַ� ������ ��(Shadow Map) ���� ���� �� ���� ���� ���ϴ� �뵵�� ���.
  *        - �Ϲ� �ؽ�ó�� D3D11_COMPARISON_NEVER.
  *    - MinLOD / MaxLOD:
  *        - MipMap ����(Level of Detail)�� ������ ������.
  *        - 0 ~ FLT_MAX�� �θ� ��� MipMap �ܰ踦 �� ���ڴٴ� ����.
  */
class Texture 
{
public:
    // [GPU ���ҽ� ������]
    // ID3D11ShaderResourceView: ���̴��� �ؽ�ó�� "�Ĵٺ��� ���" ������.
    // ComPtr�� ����ϸ� Release()�� �ڵ����� ȣ�����־� �޸� ������ ������.
    ID3D11ShaderResourceView* pSRV = nullptr;
    ID3D11SamplerState* pSampler = nullptr; // ���÷�

    // �ؽ�ó�� ����/���� ũ�� (UI ��ġ�� ���� ��� �� �л��鿡�� ������)
    uint32_t width = 0;
    uint32_t height = 0;

    Texture() = default;

    // ���� ���� (GPU ���ҽ��� �Ժη� �����ϸ� �� ��. �ּҰ��� �����ؾ� ��)
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    //�Ҹ���
    ~Texture() {
        Release();
    }

    void Release() {
        if (pSRV) { pSRV->Release();     pSRV = nullptr; }
        if (pSampler) { pSampler->Release(); pSampler = nullptr; }
    }

    /**
     * * �̹��� ������ �ε��Ͽ� GPU ���ҽ��� ������.
     * @param device: DirectX11 ��ġ (����Ʈ ���� ��ü)
     * @param path: �̹��� ���� ��� (�����ڵ� ���ڿ� L"..." ����)
     * @return ���� ���� (bool)
     */
    bool Load(ID3D11Device* device, const std::wstring& path) 
    {
        // 0. �ʱ�ȭ �� ���� ���ҽ� ����
        Release();
                
        // 1. WIC ���丮 ���� (�̹��� ó���� ���� ���� ���� ����)
        IWICImagingFactory* wicFactory = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
        if (FAILED(hr))
             return false;

        // 2. �̹��� ���ڴ� ���� (���� ����)
        IWICBitmapDecoder* decoder = nullptr;
        hr = wicFactory->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
        if (FAILED(hr))
        {
            if (wicFactory) 
                wicFactory->Release();
            return false;
        }

        // 3. ù ��° ������ �������� (��κ��� �̹����� 1����)
        IWICBitmapFrameDecode* frame = nullptr;
        hr = decoder->GetFrame(0, &frame);
        if (FAILED(hr))
        {
            if (decoder)    decoder->Release();
            if (wicFactory) wicFactory->Release();
            return false;
        }

        // 4. �̹��� ũ�� ���� ȹ��
        hr = frame->GetSize(&width, &height);
        if (FAILED(hr))
        {
            if (decoder)    decoder->Release();
            if (wicFactory) wicFactory->Release();
            if (frame) frame->Release();
            return false;
        }

        // 5. DX11�� �����ϴ� RGBA 8��Ʈ �������� ���� ��ȯ
        // (�̹��� ������ JPG(RGB)�� PNG(RGBA)�� ������ RGBA�� ������)
        IWICFormatConverter* converter = nullptr;
        hr = wicFactory->CreateFormatConverter(&converter);
        if (FAILED(hr))
        {
            if (converter) converter->Release();
            if (frame) frame->Release();
            if (decoder) decoder->Release();
            if (wicFactory) wicFactory->Release();
            return false;
        }

        hr = converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
        if (FAILED(hr))
        {
            if (converter) converter->Release();
            if (frame) frame->Release();
            if (decoder) decoder->Release();
            if (wicFactory) wicFactory->Release();
            return false;
        }

        // 6. CPU �޸𸮿� �ȼ� ������ ���� (���� �Ҵ�)
        std::vector<uint8_t> pixelData(width * height * 4); // RGBA�� �ȼ��� 4����Ʈ
        hr = converter->CopyPixels(nullptr, width * 4, (UINT)pixelData.size(), pixelData.data());
        if (FAILED(hr))
        {
            if (converter) converter->Release();
            if (frame) frame->Release();
            if (decoder) decoder->Release();
            if (wicFactory) wicFactory->Release();
            return false;
        }

        // 7. GPU�� �ؽ�ó(ID3D11Texture2D) ����
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ǥ�� RGBA ����
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = pixelData.data(); // ��� ������ �ȼ� ������ �ּ�
        initData.SysMemPitch = width * 4;    // �� ���� ����Ʈ ũ��

        ID3D11Texture2D* tex2D = nullptr;
        hr = device->CreateTexture2D(&texDesc, &initData, &tex2D);
        if (FAILED(hr))
        {
            if (tex2D) tex2D->Release();
            if (converter) converter->Release();
            if (frame) frame->Release();
            if (decoder) decoder->Release();
            if (wicFactory) wicFactory->Release();
            return false;
        }

        // 8. ���� ������� Shader Resource View(SRV) ����
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        hr = device->CreateShaderResourceView(tex2D, &srvDesc, &pSRV);

        if (tex2D) tex2D->Release();
        if (converter) converter->Release();
        if (frame) frame->Release();
        if (decoder) decoder->Release();
        if (wicFactory) wicFactory->Release();

        return SUCCEEDED(hr);
    }

    //�⺻ ���÷� ���� ���� (�⺻���� �ε巯�� Linear ���)
    void CreateSampler(ID3D11Device* device) {
        D3D11_SAMPLER_DESC desc = {};
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // [�Ӽ�] �ε巯�� Ȯ��/���
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    // [�Ӽ�] U�� �ݺ�
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    // [�Ӽ�] V�� �ݺ�
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;    // [�Ӽ�] W�� �ݺ�
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.MinLOD = 0;
        desc.MaxLOD = D3D11_FLOAT32_MAX;

        device->CreateSamplerState(&desc, &pSampler);
    }

};