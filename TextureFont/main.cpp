//�ؽ��� ���� ���� ����

//framework.hpp -> texture
//�׿� �������� : material, meshrenderer

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include "GameLoop.hpp"
#include "MeshRenderer.hpp"
#include "PlayerControl.hpp"

#include "FontMesh.hpp"
#include "TextRenderer.h"
#include "Font.h"
#include "TextBoxManager.h"

// -----------------------------------------------------------------------------
// [������ �޽��� ó����]
// -----------------------------------------------------------------------------
LRESULT CALLBACK GlobalWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_DESTROY) PostQuitMessage(0);
    if (m == WM_CHAR)
    {
        const wchar_t key = static_cast<wchar_t>(w);
        TextRenderer* tc = TextBoxManager::Get().focus;
        if(tc)
            tc->InputText(key);

    }
    return DefWindowProc(h, m, w, l);
}

// -----------------------------------------------------------------------------
// [���� ��Ʈ�� ����Ʈ]
// -----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS) 
{
    GameLoop& gEngine = GameLoop::Get();
    gEngine.Initialize(hI, GlobalWndProc);
   
    D3D11_INPUT_ELEMENT_DESC ied[] = 
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    ShaderSet texShaders;
    gEngine.gfx.LoadVertexShader(&texShaders, L"vs", ied, ARRAYSIZE(ied));
    gEngine.gfx.LoadPixelShader(&texShaders, L"ps");

    ShaderSet fontShader;
    gEngine.gfx.LoadVertexShader(&fontShader, L"font_vs", ied, ARRAYSIZE(ied));
    gEngine.gfx.LoadPixelShader(&fontShader, L"font_ps");

    std::vector<Vertex> vQuad;
    vQuad.push_back({ {-0.5f,  0.5f, 0.0f}, {0, 0} }); // �»�
    vQuad.push_back({ { 0.5f,  0.5f, 0.0f}, {1, 0} }); // ���
    vQuad.push_back({ {-0.5f, -0.5f, 0.0f}, {0, 1} }); // ����
    
    vQuad.push_back({ { 0.5f, -0.5f, 0.0f}, {1, 1} }); // ����
    vQuad.push_back({ {-0.5f, -0.5f, 0.0f}, {0, 1} }); // ����
    vQuad.push_back({ { 0.5f,  0.5f, 0.0f}, {1, 0} }); // ���


    // quadMesh
    Mesh* quadMesh = new Mesh();
    quadMesh->Create(&gEngine.gfx, vQuad);

    // Texture
    Texture* tex = new Texture();
    tex->Load(gEngine.gfx.Device, L"textfont.png");
    tex->CreateSampler(gEngine.gfx.Device);

    // Material
    Material* texMat = new Material();
    texMat->SetShaderSet(&texShaders);
    texMat->AddTexture(tex);

    // Font
    Font* font = new Font(tex->width, tex->height);

    Material* fontMat = new Material();
    fontMat->SetShaderSet(&fontShader);
    fontMat->AddTexture(tex);

    // FontMesh
    FontMesh::font = new FontMesh();
    FontMesh* fontMesh = FontMesh::font;
    fontMesh->Create(&gEngine.gfx, vQuad);

   // Object
    GameObject* obj = new GameObject(-0.8f, 0.5f, 0);
    obj->scale.x = 0.1f;
    obj->scale.y = 0.1f;
    TextRenderer* textRenderer = new TextRenderer(fontMesh, fontMat, font);
    TextBoxManager::Get().focus = textRenderer;
    obj->AddComponent(textRenderer);
    gEngine.world.push_back(obj);

    //GameObject* obj2 = new GameObject(-0.8f, 0.0f, 0);
    //obj2->scale.x = 0.1f;
    //obj2->scale.y = 0.1f;
    //TextRenderer* textRenderer2 = new TextRenderer(fontMesh, fontMat, font);
    //TextBoxManager::Get().focus = textRenderer2;
    //obj2->AddComponent(textRenderer2);
    //gEngine.world.push_back(obj2);

    gEngine.Run();


    // Delete
    delete texMat;
    delete fontMat;

    texShaders.Release();
    fontShader.Release();

    delete tex;

    delete quadMesh;
    delete fontMesh;

    delete font;

    return 0;
}


