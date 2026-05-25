#pragma once
#include "ObjectBase.hpp"
#include "MeshRenderer.hpp"	

enum class Align
{
	Start, Center, End
};

class Font;
class FontMesh;

	class TextRenderer : public MeshRenderer
	{
	public:
		Font* font;
		FontMesh* fontMesh;
		std::vector<Text> textList;
		bool shouldUpdate;
		bool focused;
		Align alignX;
		Align alignY;

		TextRenderer(FontMesh* fontMesh, Material* fontMat, Font* font);

		// Component을(를) 통해 상속됨
		void Start(GraphicsContext* gfx) override;
		void Input() override;
		void Update(float dt) override;
		void Render(GraphicsContext* gfx) override;

		void InputText(const wchar_t& key);

		// return ltrb
		std::array<float, 4> GetTextBoxToScreen();


	};

