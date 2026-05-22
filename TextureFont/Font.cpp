#include "Font.h"
#include <iostream>
#include <cwctype>

Font::Font(uint32_t texWidth, uint32_t texHeight) :
    texWidth((float)texWidth), texHeight((float)texHeight)
{
    alphabetSetting.startX = 5.f;
    alphabetSetting.startY = 8.f;
    alphabetSetting.width = 45.f;
    alphabetSetting.height = 88.f;
    alphabetSetting.strideX = 16.f;

    numberSetting.startX = 21.f;
    numberSetting.startY = 112.f;
    numberSetting.width = 38.f;
    numberSetting.height = 64.f;
    numberSetting.strideX = 22.f;

}

Text Font::GetText(const wchar_t& c)
{
    Text r = { };
    r.text = std::wstring::npos;
    int idx = -1;

    if (std::iswdigit(c))
    {
        idx = c - L'0';

        FontSetting& fs = numberSetting;
        r.left = (fs.startX + (fs.strideX + fs.width) * idx) / texWidth;
        r.top = (fs.startY) / texHeight;
        r.width = fs.width / texWidth;
        r.height = fs.height / texHeight;
        r.text = c;

    }
    else if (std::iswalpha(c))
    {
        idx = std::towlower(c) - L'a';

        FontSetting& fs = alphabetSetting;
        r.left = (fs.startX + (fs.strideX + fs.width) * idx) / texWidth;
        r.top = (fs.startY) / texHeight;
        r.width = fs.width / texWidth;
        r.height = fs.height / texHeight;
        r.text = c;

    }
    else if (c == L' ')
    {
        r.text = c;
    }
    else if (c == VK_BACK)
    {
        r.text = c;
    }

    return r;
}