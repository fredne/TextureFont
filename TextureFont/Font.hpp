#pragma once
#include "Framework.hpp"
#include "Mesh.hpp"
#include <iostream>
#include <cwctype>

struct Rect
{
    float left;
    float top;
    float width;
    float height;
};

class Font : public Mesh
{
public:
    static Font* font;

    int idx = 0;
    float width = 626;
    float height = 313;
    float startX = 28.f;
    float startY = 31.f;
    float offsetX = 32.f;
    float offsetY = 64.f;
    float strideX = 28.f;

    std::vector<Rect> textList;

    void Create(GraphicsContext* gfx, const std::vector<Vertex>& vertices) override
    {

        vertexCount = (UINT)vertices.size();

        D3D11_BUFFER_DESC bd = { 0 };
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(Vertex) * vertexCount * 100;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA sd = { 0 };
        sd.pSysMem = vertices.data();

        gfx->Device->CreateBuffer(&bd, &sd, &vBuffer);
    }

    void Input()
    {
        if (GetAsyncKeyState('S') & 0x0001)
        {
            std::wstring str;
            std::wcout << "이름을 입력하세요: ";
            std::wcin >> str;
            std::wcout << str << std::endl;

            for (const wchar_t& c : str)
            {
                int idx = -1;
                if (std::iswdigit(c))
                {
                    idx = c - L'0';

                    Rect r = { };
                    r.left = (startX + (strideX + offsetX) * idx) / width;
                    r.top = (startY) / height;
                    r.width = offsetX / width;
                    r.height = offsetY / height;

                    textList.push_back(r);

                }
            }
        }

        if (GetAsyncKeyState(VK_BACK) & 0x0001)
        {
            textList.pop_back();
        }

        if (GetAsyncKeyState('Q') & 0x0001)
            idx = (idx + 1) % 10;
    }

    void UpdateMesh(GraphicsContext* gfx)
    {
        if (!vBuffer) return;


        std::vector<Vertex> vQuad;
        for (int i = 0; i < textList.size(); ++i)
        {
            float vLeft = i;
            float vtop = 1;
            float vRight = vLeft + 1;
            float vBottom = vtop - 1;

            const Rect& text = textList[i];
            float rRight = text.left + text.width;
            float rBottom = text.top + text.height;

            vQuad.push_back({ { vLeft + 1,  vtop, 0.0f}, {text.left, text.top} }); // 좌상
            vQuad.push_back({ { vRight + 1,  vtop, 0.0f}, {rRight, text.top} }); // 우상
            vQuad.push_back({ { vLeft + 1,  vBottom, 0.0f}, {text.left, rBottom} }); // 좌하

            vQuad.push_back({ { vRight + 1, vBottom, 0.0f}, {rRight, rBottom} }); // 우하
            vQuad.push_back({ { vLeft + 1, vBottom, 0.0f}, {text.left, rBottom} }); // 좌하
            vQuad.push_back({ { vRight + 1, vtop, 0.0f}, {rRight, text.top} }); // 우상

        }

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
