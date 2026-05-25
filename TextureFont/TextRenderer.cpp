#include "Framework.hpp"
#include "TextRenderer.h"
#include "FontMesh.hpp"
#include <iostream>
#include "Font.h"
#include <array>

TextRenderer::TextRenderer(FontMesh* fontMesh, Material* mat, Font* font) : 
    MeshRenderer(fontMesh, mat),
    fontMesh(fontMesh),  font(font),
    focused(false), shouldUpdate(false),
    alignX(Align::Start), alignY(Align::Start)
{

}


void TextRenderer::Start(GraphicsContext* gfx)
{
    MeshRenderer::Start(gfx);
    std::wstring s = L"hi owo";
    for (auto c : s)
    {
        InputText(c);
    }

    fontMesh->UpdateMesh(gfx, textList, alignX, alignY);
}

void TextRenderer::Input()
{
    if (not focused) return;

    if (GetAsyncKeyState(VK_LEFT) & 0x0001)
    {
        textList.clear();
        std::wstring s = L"abcdef";
        for (auto c : s)
        {
            InputText(c);
        }

        shouldUpdate = true;

    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
    {
        textList.clear();
        std::wstring s = L"ghijklmnopqr";
        for (auto c : s)
        {
            InputText(c);
        }

        shouldUpdate = true;

    }
    else if (GetAsyncKeyState(VK_UP) & 0x0001)
    {
        textList.clear();
        std::wstring s = L"stuvwcyz";
        for (auto c : s)
        {
            InputText(c);
        }

        shouldUpdate = true;

    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x0001)
    {
        textList.clear();
        std::wstring s = L"0123456789";
        for (auto c : s)
        {
            InputText(c);
        }

        shouldUpdate = true;

    }

    // Align
    if (GetAsyncKeyState(VK_F1) & 0x0001)
    {
        alignX = Align::Start;
        alignY = Align::Start;
        shouldUpdate = true;
    }
    else if (GetAsyncKeyState(VK_F2) & 0x0001)
    {
        alignX = Align::Center;
        alignY = Align::Center;
        shouldUpdate = true;
    }
    else if (GetAsyncKeyState(VK_F3) & 0x0001)
    {
        alignX = Align::End;
        alignY = Align::End;
        shouldUpdate = true;
    }

    // F! -> 콘솔 입력
    if (GetAsyncKeyState(VK_F4) & 0x0001)
    {
        std::wstring str;
        std::wcout << "Enter your name: ";
        std::wcin >> str;
        std::wcout << str << std::endl;

        textList.clear();

        for (const wchar_t& c : str)
        {
            InputText(c);
        }

        shouldUpdate = true;

    }

}

void TextRenderer::InputText(const wchar_t& c)
{
    if (c == VK_BACK)
    {
        if (!textList.empty())
        {
            textList.pop_back();
            shouldUpdate = true;
        }
    }
    else
    {
        Text text = font->GetText(c);
        if (text.text != static_cast<wchar_t>(std::string::npos))
        {
            textList.push_back(text);
            shouldUpdate = true;
        }
    }
}

std::array<float, 4> TextRenderer::GetTextBoxToScreen()
{
    float textScaleX = pOwner->scale.x * textList.size();
    float textScaleY = pOwner->scale.y * 1;
    float left = pOwner->pos.x;
    float top = -(pOwner->pos.y + textScaleY);
    float right = left + textScaleX;
    float bottom = -pOwner->pos.y;
    float offsetX = (float)alignX * textScaleX * 0.5f;
    float offsetY = (float)alignY * textScaleY * 0.5f;

    std::array<float, 4> rect =
    {
        left - offsetX, top - offsetY, 
        right - offsetX, bottom - offsetY
    };

    return rect;
}


void TextRenderer::Update(float dt)
{

}

void TextRenderer::Render(GraphicsContext* gfx)
{
    if (shouldUpdate)
    {
        fontMesh->UpdateMesh(gfx, textList, alignX, alignY);
        shouldUpdate = false;
    }

    MeshRenderer::Render(gfx);

}
