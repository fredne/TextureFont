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
    int number;
};

class Font : public Mesh
{
public:
    static Font* font;

    int idx = 0;
    float totalWidth = 626;
    float totalHeight = 313;
    float startX = 22.f;
    float startY = 31.f;
    float width = 38.f;
    float height = 64.f;
    float strideX = 22.f;

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
        // S 입력시 텍스트 입력 받기
        if (GetAsyncKeyState('S') & 0x0001)
        {
            std::wstring str;
            std::wcout << "이름을 입력하세요: ";
            std::wcin >> str;
            std::wcout << str << std::endl;

            for (const wchar_t& c : str)
            {
                // 일단 숫자일 경우에만 텍스트 추가
                if (std::iswdigit(c))
                {
                    int idx = c - L'0';

                    Rect r = { };
                    r.left = (startX + (strideX + width) * idx) / totalWidth;
                    r.top = (startY) / totalHeight;
                    r.width = width / totalWidth;
                    r.height = height / totalHeight;

                    textList.push_back(r);

                }
                else if (std::iswalpha(c))
                {
                    int idx = c - L'a';

                    Rect r = { };
                    r.left = (startX + (strideX + width) * idx) / totalWidth;
                    r.top = (startY) / totalHeight;
                    r.width = width / totalWidth;
                    r.height = height / totalHeight;
                    r.number = idx;

                    textList.push_back(r);
                }
            }
        }


        //Backspace 입력시 지우기
        if (GetAsyncKeyState(VK_BACK) & 0x0001)
        {
            if(!textList.empty())
                textList.pop_back();
        }

        if (GetAsyncKeyState('Q') & 0x0001)
            idx = (idx + 1) % 10;
    }

    void UpdateMesh(GraphicsContext* gfx)
    {
        if (!vBuffer) return;


        // 새 정점 데이터 생성
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

        // 정점 데이터 업데이트
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
