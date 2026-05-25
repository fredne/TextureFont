#pragma once
#include <array>	
#include "WindowContext.hpp"
#include "TextRenderer.h";

extern POINT mousePos;
class TextRenderer;

class TextBoxManager
{
public:
	bool prevL = false;
	bool curL = false;
	std::vector<TextRenderer*> textBoxList;
	TextRenderer* focus;

	static TextBoxManager& Get()
	{
		static TextBoxManager instance;
		return instance;
	}


	void Update(WindowContext* win)
	{
		prevL = curL;
		curL = GetAsyncKeyState(VK_LBUTTON) & 0x8000;

		if (curL and !prevL)
		{
			float mouseX = (float)mousePos.x / (float)win->Width;
			float mouseY = (float)mousePos.y / (float)win->Height;
			mouseX = mouseX * 2 - 1;
			mouseY = mouseY * 2 - 1;

			for (auto textBox : textBoxList)
			{
				if (textBox == focus) continue;

				std::array<float, 4> ltrb = textBox->GetTextBoxToScreen();

				if (ltrb[0] < mouseX && mouseX < ltrb[2] &&
					ltrb[1] < mouseY && mouseY < ltrb[3])
				{
					focus->focused = false;
					focus = textBox;
					focus->focused = true;
					printf("The focus TextBox Changed\n");
				}
			}

		}


	}

};

