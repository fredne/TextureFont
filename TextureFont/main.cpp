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

POINT mousePos = { };
// -----------------------------------------------------------------------------
// [������ �޽��� ó����]
// -----------------------------------------------------------------------------
LRESULT CALLBACK GlobalWndProc(HWND h, UINT m, WPARAM w, LPARAM l) 
{
    if (m == WM_DESTROY) PostQuitMessage(0);
    if (m == WM_CHAR)
    {
        const wchar_t key = static_cast<wchar_t>(w);
        TextRenderer* tc = TextBoxManager::Get().focus;
        if(tc)
            tc->InputText(key);

    }
    if (m == WM_MOUSEMOVE)
    {
        mousePos.x = LOWORD(l);
        mousePos.y = HIWORD(l);
    }

    return DefWindowProc(h, m, w, l);
}

// -----------------------------------------------------------------------------
// [���� ��Ʈ�� ����Ʈ]
// -----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS) 
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

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
    FontMesh* fontMesh1 = FontMesh::font;
    fontMesh1->Create(&gEngine.gfx, vQuad);

    FontMesh* fontMesh2 = new FontMesh();
    fontMesh2->Create(&gEngine.gfx, vQuad);

   // Object
    GameObject* textBox1 = new GameObject(0, 0.5f, 0);
    textBox1->scale.x = 0.1f;
    textBox1->scale.y = 0.1f;
    TextRenderer* textRenderer1 = new TextRenderer(fontMesh1, fontMat, font);
    textBox1->AddComponent(textRenderer1);
    gEngine.world.push_back(textBox1);

    GameObject* textBox2 = new GameObject(0, 0.0f, 0);
    textBox2->scale.x = 0.1f;
    textBox2->scale.y = 0.1f;
    TextRenderer* textRenderer2 = new TextRenderer(fontMesh2, fontMat, font);
    TextBoxManager::Get().focus = textRenderer2;
    textBox2->AddComponent(textRenderer2);
    gEngine.world.push_back(textBox2);

    TextBoxManager& tbManager = TextBoxManager::Get();
    tbManager.textBoxList.push_back(textRenderer1);
    tbManager.textBoxList.push_back(textRenderer2);
    tbManager.focus = textRenderer1;
    tbManager.focus->focused = true;

    gEngine.Run();

    // Delete
    delete texMat;
    delete fontMat;

    texShaders.Release();
    fontShader.Release();

    delete tex;

    delete quadMesh;
    delete fontMesh1;
    delete fontMesh2;

    delete font;

    return 0;
}


