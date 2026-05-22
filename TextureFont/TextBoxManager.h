#pragma once

class TextComponent;

class TextBoxManager
{
public:
	static TextBoxManager& Get()
	{
		static TextBoxManager instance;
		return instance;
	}

	TextComponent* focus;

};

