#pragma once
#include "Framework.hpp"
#include "Mesh.hpp"
#include <iostream>
#include <cwctype>

struct Text
{
    float left;
    float top;
    float width;
    float height;
    int number;
};

struct FontSetting
{
    static float totalWidth;
    static float totalHeight;
    float startX;
    float startY;
    float width;
    float height; 
    float strideX; 
};

float FontSetting::totalWidth = 0;
float FontSetting::totalHeight = 0;


class Font : public Mesh
{
public:
    static Font* font;

    FontSetting numberSetting;
    FontSetting alphabetSetting;

    std::vector<Text> textList;

    void Create(GraphicsContext* gfx, const std::vector<Vertex>& vertices) override
    {
        FontSetting::totalWidth = 1584.f;
        FontSetting::totalHeight = 730.f;

        numberSetting.startX = 22.f;
        numberSetting.startY = 405.f;
        numberSetting.width = 38.f;
        numberSetting.height = 64.f;
        numberSetting.strideX = 22.f;

        alphabetSetting.startX = 5.f;
        alphabetSetting.startY = 8.f;
        alphabetSetting.width = 45.f;
        alphabetSetting.height = 88.f;
        alphabetSetting.strideX = 16.f;

        vertexCount = (UINT)vertices.size();

        D3D11_BUFFER_DESC bd = { 0 };
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(Vertex) * vertexCount * 100;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA sd = { 0 };
        sd.pSysMem = vertices.data();

        gfx->Device->CreateBuffer(&bd, &sd, &vBuffer);

        UpdateMesh(gfx);
    }

    void Input(GraphicsContext* gfx)
    {
        // S �Է½� �ؽ�Ʈ �Է� �ޱ�
        if (GetAsyncKeyState(VK_TAB) & 0x0001)
        {
            std::wstring str;
            std::wcout << "Enter your name: ";
            std::wcin >> str;
            std::wcout << str << std::endl;

            for (const wchar_t& c : str)
            {
                InputText(c);
            }

            UpdateMesh(gfx);
        }


        //Backspace �Է½� �����
        if (GetAsyncKeyState(VK_BACK) & 0x0001)
        {
            if (!textList.empty())
            {
                textList.pop_back();
                UpdateMesh(gfx);
            }

        }

    }

    void InputText(const wchar_t& key)
    {
        Text r = { };
        // �ϴ� ������ ��쿡�� �ؽ�Ʈ �߰�
        if (std::iswdigit(key))
        {
            int idx = key - L'0';

            FontSetting& fs = numberSetting;
            r.left = (fs.startX + (fs.strideX + fs.width) * idx) / fs.totalWidth;
            r.top = (fs.startY) / fs.totalHeight;
            r.width = fs.width / fs.totalWidth;
            r.height = fs.height / fs.totalHeight;
            r.number = idx;

            textList.push_back(r);

        }
        else if (std::iswalpha(key))
        {
            int idx = std::towlower(key) - L'a';

            FontSetting& fs = alphabetSetting;
            r.left = (fs.startX + (fs.strideX + fs.width) * idx) / fs.totalWidth;
            r.top = (fs.startY) / fs.totalHeight;
            r.width = fs.width / fs.totalWidth;
            r.height = fs.height / fs.totalHeight;
            r.number = idx;

            textList.push_back(r);
        }
        else if (key == L' ')
        {
            textList.push_back(r);
        }

    }

    void InputTextAndUpdateMesh(const wchar_t& key, GraphicsContext* gfx)
    {
        InputText(key);
        UpdateMesh(gfx);

    }

    void UpdateMesh(GraphicsContext* gfx)
    {
        if (!vBuffer) return;

        // �� ���� ������ ����
        std::vector<Vertex> vQuad;
        for (int i = 0; i < textList.size(); ++i)
        {
            float vLeft = i;
            float vtop = 1;
            float vRight = vLeft + 1;
            float vBottom = vtop - 1.5f;

            const Text& text = textList[i];
            float rRight = text.left + text.width;
            float rBottom = text.top + text.height;

            vQuad.push_back({ { vLeft + 1,  vtop, 0.0f}, {text.left, text.top} }); // �»�
            vQuad.push_back({ { vRight + 1,  vtop, 0.0f}, {rRight, text.top} }); // ���
            vQuad.push_back({ { vLeft + 1,  vBottom, 0.0f}, {text.left, rBottom} }); // ����

            vQuad.push_back({ { vRight + 1, vBottom, 0.0f}, {rRight, rBottom} }); // ����
            vQuad.push_back({ { vLeft + 1, vBottom, 0.0f}, {text.left, rBottom} }); // ����
            vQuad.push_back({ { vRight + 1, vtop, 0.0f}, {rRight, text.top} }); // ���

        }

        // ���� ������ ������Ʈ
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

        HRESULT hr = gfx->ImmediateContext->Map(vBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (SUCCEEDED(hr))
        {
            vertexCount = vQuad.size();
            memcpy(mappedResource.pData, vQuad.data(), sizeof(Vertex) * vertexCount);
            gfx->ImmediateContext->Unmap(vBuffer, 0);
        }

    }


};

Font* Font::font = nullptr;
