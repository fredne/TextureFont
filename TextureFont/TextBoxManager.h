#pragma once

class TextRenderer;

class TextBoxManager
{
public:
	static TextBoxManager& Get()
	{
		static TextBoxManager instance;
		return instance;
	}

	TextRenderer* focus;

};

