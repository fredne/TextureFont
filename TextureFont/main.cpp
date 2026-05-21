//�ؽ��� ���� ���� ����

//framework.hpp -> texture
//�׿� �������� : material, meshrenderer

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include "GameLoop.hpp"
#include "MeshRenderer.hpp"
#include "PlayerControl.hpp"

#include "Font.hpp"


// -----------------------------------------------------------------------------
// [������ �޽��� ó����]
// -----------------------------------------------------------------------------
LRESULT CALLBACK GlobalWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_DESTROY) PostQuitMessage(0);
    if (m == WM_CHAR)
    {
        const wchar_t key = static_cast<wchar_t>(w);
        Font::font->InputTextAndUpdateMesh(key, &GameLoop::Get().gfx);
    }
    return DefWindowProc(h, m, w, l);
}

// -----------------------------------------------------------------------------
// [���� ��Ʈ�� ����Ʈ]
// -----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS) 
{
    // 1. ���� �Ŵ��� �ʱ�ȭ
    GameLoop& gEngine = GameLoop::Get();
    gEngine.Initialize(hI, GlobalWndProc);
   
    D3D11_INPUT_ELEMENT_DESC ied[] = 
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // ���̴� ������ �� ����
    ShaderSet texShaders;
    // ���ؽ� ���̴��� ���̾ƿ� �ε�
    gEngine.gfx.LoadVertexShader(&texShaders, L"vs", ied, ARRAYSIZE(ied));
    // �ȼ� ���̴� �ε�
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

    // �޽�����
    Mesh* quadMesh = new Mesh();
    quadMesh->Create(&gEngine.gfx, vQuad);

    // �ؽ�ó �ε� �� ����
    Texture* tex = new Texture();
    //tex->Load(gEngine.gfx.Device, L"digital-numbers.png");
    tex->Load(gEngine.gfx.Device, L"textfont.png");
    tex->CreateSampler(gEngine.gfx.Device);

    //�ؽ��� ��Ƽ���� ����
    Material* texMat = new Material();
    texMat->SetShaderSet(&texShaders);
    texMat->AddTexture(tex);

    // Font
    // �ؽ�Ʈ�� ����� ��Ƽ����
    Material* fontMat = new Material();
    fontMat->SetShaderSet(&fontShader);
    fontMat->AddTexture(tex);

    // ��Ʈ �޽�
    Font::font = new Font();
    Mesh* font = Font::font;
    font->Create(&gEngine.gfx, vQuad);
        
    //���ӿ�����Ʈ ����
    GameObject* obj = new GameObject(-0.8f, 0.5f, 0);
    obj->scale.x = 0.1f;
    obj->scale.y = 0.1f;
    // ��Ʈ ������ �߰�
    obj->AddComponent(new MeshRenderer(font, fontMat));
    //obj->AddComponent(new PlayerController());
    gEngine.world.push_back(obj);

    // ���� ���� (���� ����)
    gEngine.Run();

    // ----�ڿ� ����--------------
    // ��Ƽ���� ����
    delete texMat;
    delete fontMat;

    // ���̴� ��Ʈ ����
    texShaders.Release();
    fontShader.Release();

    delete tex;

    // �޽� ����
    delete quadMesh;
    delete font;

    // gEngine�� �Ҹ��ڿ��� world ���� ��� GameObject�� delete��
    return 0;
}


