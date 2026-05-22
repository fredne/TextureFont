#pragma once
#include "Framework.hpp"
#include "Mesh.hpp"

class FontMesh : public Mesh
{
public:
    static FontMesh* font;

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

    void UpdateMesh(GraphicsContext* gfx, const std::vector<Text>& textList)
    {
        if (!vBuffer) return;

        // 문자 리스트에 따라 사각형 생성
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

