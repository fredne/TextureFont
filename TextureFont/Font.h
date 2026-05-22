#pragma once
#include "Framework.hpp"

class Font
{
public:
    float texWidth;
    float texHeight;
        
	FontSetting alphabetSetting;
	FontSetting numberSetting;

    Font(uint32_t texWidth, uint32_t texHeight);

    Text GetText(const wchar_t& c);

};

