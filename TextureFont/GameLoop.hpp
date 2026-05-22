#pragma once
#include "WindowContext.hpp"
#include "GraphicsContext.hpp"
#include "Timer.hpp"
#include "ObjectBase.hpp"
#include "FontMesh.hpp"

class GameLoop {
private:
    GameLoop() {
        printf("[Engine] GameLoop Created.\n");
    }
    GameLoop(const GameLoop&) = delete;
    GameLoop(GameLoop&&) = delete;

public:
    static GameLoop& Get()
    {
        static GameLoop instance;
        return instance;
    }

    WindowContext win;
    GraphicsContext gfx;
    DeltaTime timer;
    std::vector<GameObject*> world;
    bool isRunning = true;

    ~GameLoop() {
        for (auto obj : world) delete obj;
        world.clear();
        printf("[Engine] GameLoop Destroyed.\n");
    }

    void Initialize(HINSTANCE hInst, LRESULT(CALLBACK* wndProc)(HWND, UINT, WPARAM, LPARAM)) {
        win.Initialize(hInst, 800, 800, wndProc);
        gfx.InitDX(win.hWnd, 800, 800);
    }

    void Input() {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) isRunning = false;

        // â ũ�� ���� ��� ���� (CŰ)
        //if (GetAsyncKeyState('C') & 0x0001) {
        //    win.Width = 600; win.Height = 600;
        //    RECT rc = { 0, 0, win.Width, win.Height };
        //    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        //    SetWindowPos(win.hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
        //    gfx.Resize(win.Width, win.Height);
        //}

        for (auto obj : world) obj->Input();
    }

    void Update() {
        float dt = timer.GetDelta();
        for (auto obj : world) obj->Update(dt, &gfx);
    }

    void Render() {
        //float col[] = { 0.1f, 0.2f, 0.3f, 1.0f };
        float col[] = { 0.6f, 0.6f, 0.6f, 1.0f };
        gfx.ImmediateContext->ClearRenderTargetView(gfx.RTV, col);

        D3D11_VIEWPORT vp = { 0, 0, (float)win.Width, (float)win.Height, 0, 1 };
        gfx.ImmediateContext->RSSetViewports(1, &vp);
        gfx.ImmediateContext->OMSetRenderTargets(1, &gfx.RTV, NULL);
        gfx.ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        for (auto obj : world) obj->Render(&gfx);

        gfx.SwapChain->Present(gfx.VSync, 0);
    }

    void Run() {
        MSG msg = {};
        while (msg.message != WM_QUIT && isRunning) {
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg); DispatchMessage(&msg);
            }
            else {
                Input();
                //Font::font->UpdateMesh(&gfx);
                Update();
                Render();
            }
        }
    }
};