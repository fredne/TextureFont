#include "Framework.hpp"
#include "TextComponent.h"
#include "FontMesh.hpp"
#include <iostream>
#include "Font.h"
#include "MeshRenderer.hpp"

TextComponent::TextComponent(FontMesh* fontMesh, Font* font) :
    fontMesh(fontMesh), shouldUpdate(false), font(font)
{
}

void TextComponent::Start(GraphicsContext* gfx)
{
    std::wstring s = L"hi owo";
    for (auto c : s)
    {
        InputText(c);
    }

    fontMesh->UpdateMesh(gfx, textList);
}

void TextComponent::Input()
{
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
    if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
    {
        textList.clear();
        std::wstring s = L"ghijklmnopqr";
        for (auto c : s)
        {
            InputText(c);
        }

        shouldUpdate = true;

    }
    if (GetAsyncKeyState(VK_UP) & 0x0001)
    {
        textList.clear();
        std::wstring s = L"stuvwcyz";
        for (auto c : s)
        {
            InputText(c);
        }

        shouldUpdate = true;

    }
    if (GetAsyncKeyState(VK_DOWN) & 0x0001)
    {
        textList.clear();
        std::wstring s = L"0123456789";
        for (auto c : s)
        {
            InputText(c);
        }

        shouldUpdate = true;

    }

    // Tab -> 콘솔 입력
    if (GetAsyncKeyState(VK_TAB) & 0x0001)
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

    //Backspace -> 문자 제거
    if (GetAsyncKeyState(VK_BACK) & 0x0001)
    {
        if (!textList.empty())
        {
            textList.pop_back();
            shouldUpdate = true;
        }
    }

}

void TextComponent::InputText(const wchar_t& c)
{
    Text text = font->GetText(c);
    if (text.text != std::wstring::npos && text.text != VK_BACK)
    {
        textList.push_back(text);
        shouldUpdate = true;
    }
    
}


void TextComponent::Update(float dt)
{

}

void TextComponent::Render(GraphicsContext* gfx)
{
    if (shouldUpdate)
    {
        fontMesh->UpdateMesh(gfx, textList);
        shouldUpdate = false;
    }

}
