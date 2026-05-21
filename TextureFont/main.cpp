//텍스쳐 매핑 관련 예시

//framework.hpp -> texture
//그외 수정내용 : material, meshrenderer



#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include "GameLoop.hpp"
#include "MeshRenderer.hpp"
#include "PlayerControl.hpp"

#include "Font.hpp"


// -----------------------------------------------------------------------------
// [윈도우 메시지 처리기]
// -----------------------------------------------------------------------------
LRESULT CALLBACK GlobalWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(h, m, w, l);
}

// -----------------------------------------------------------------------------
// [메인 엔트리 포인트]
// -----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nS) 
{
    // 1. 엔진 매니저 초기화
    GameLoop gEngine;
    gEngine.Initialize(hI, GlobalWndProc);
   
    D3D11_INPUT_ELEMENT_DESC ied[] = 
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // 셰이더 컴파일 및 생성
    ShaderSet texShaders;
    // 버텍스 셰이더와 레이아웃 로드
    gEngine.gfx.LoadVertexShader(&texShaders, L"vs", ied, ARRAYSIZE(ied));
    // 픽셀 셰이더 로드
    gEngine.gfx.LoadPixelShader(&texShaders, L"ps");

    ShaderSet fontShader;
    gEngine.gfx.LoadVertexShader(&fontShader, L"font_vs", ied, ARRAYSIZE(ied));
    gEngine.gfx.LoadPixelShader(&fontShader, L"font_ps");

    //float width = 626;
    //float height = 313;
    //float startX = 134.f;
    //float startY = 30.f;
    //float offsetX = 32.f;
    //float offsetY = 64.f;
    //float strideX = 27.f;
    //int idx = 1;
    //Rect p = { };
    //p.left = (startX + (strideX + offsetX)*idx ) / width;
    //p.top = (startY) / height;
    //p.width = offsetX / width;
    //p.height = offsetY / height;

    std::vector<Vertex> vQuad;
    vQuad.push_back({ {-0.5f,  0.5f, 0.0f}, {0, 0} }); // 좌상
    vQuad.push_back({ { 0.5f,  0.5f, 0.0f}, {1, 0} }); // 우상
    vQuad.push_back({ {-0.5f, -0.5f, 0.0f}, {0, 1} }); // 좌하
    
    vQuad.push_back({ { 0.5f, -0.5f, 0.0f}, {1, 1} }); // 우하
    vQuad.push_back({ {-0.5f, -0.5f, 0.0f}, {0, 1} }); // 좌하
    vQuad.push_back({ { 0.5f,  0.5f, 0.0f}, {1, 0} }); // 우상

    //vQuad.push_back({ { 1.f,  1.5f, 0.0f}, {0, 0} }); // 좌상
    //vQuad.push_back({ { 1.5f,  1.5f, 0.0f}, {1, 0} }); // 우상
    //vQuad.push_back({ { 1.f,  1.f, 0.0f}, {0, 1} }); // 좌하

    //vQuad.push_back({ { 1.5f,  1.f, 0.0f}, {1, 1} }); // 우하
    //vQuad.push_back({ { 1.f,  1.f, 0.0f}, {0, 1} }); // 좌하
    //vQuad.push_back({ { 1.5f,  1.5f, 0.0f}, {1, 0} }); // 우상

    //float right = p.left + p.width;
    //float bottom = p.top + p.height;
    //std::vector<Vertex> vQuad;
    //vQuad.push_back({ {-0.5f,  0.5f, 0.0f}, {p.left, p.top} }); // 좌상
    //vQuad.push_back({ { 0.5f,  0.5f, 0.0f}, {right, p.top} }); // 우상
    //vQuad.push_back({ {-0.5f, -0.5f, 0.0f}, {p.left, bottom} }); // 좌하

    //vQuad.push_back({ { 0.5f, -0.5f, 0.0f}, {right, bottom} }); // 우하
    //vQuad.push_back({ {-0.5f, -0.5f, 0.0f}, {p.left, bottom} }); // 좌하
    //vQuad.push_back({ { 0.5f,  0.5f, 0.0f}, {right, p.top} }); // 우상

        // 텍스처 로드 및 생성
    Texture* tex = new Texture();
    tex->Load(gEngine.gfx.Device, L"digital-numbers.png");
    tex->CreateSampler(gEngine.gfx.Device);

    //텍스쳐 머티리얼 생성
    Material* texMat = new Material();
    texMat->SetShaderSet(&texShaders);
    texMat->AddTexture(tex);

    Material* fontMat = new Material();
    fontMat->SetShaderSet(&fontShader);
    fontMat->AddTexture(tex);

    // 메쉬생성
    Mesh* quadMesh = new Mesh();
    quadMesh->Create(&gEngine.gfx, vQuad);    

    Font::font = new Font();
    Mesh* font = Font::font;
    font->Create(&gEngine.gfx, vQuad);
        
    //게임오브젝트 생성
    GameObject* obj = new GameObject(-0.5f, 0.5f, 0);
   
    obj->scale.x = 0.1f;
    obj->scale.y = 0.1f;
    obj->AddComponent(new MeshRenderer(font, fontMat));
    //obj->AddComponent(new PlayerController());
    gEngine.world.push_back(obj);
    
    // 엔진 실행 (메인 루프)
    gEngine.Run();

    // ----자원 해제--------------
    // 머티리얼 해제
    delete texMat;

    // 셰이더 세트 해제
    texShaders.Release();

    // 메쉬 해제
    delete quadMesh;

    // gEngine은 소멸자에서 world 내의 모든 GameObject를 delete함
    return 0;
}


