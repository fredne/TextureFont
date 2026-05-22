#pragma once
#include "ObjectBase.hpp"

class Font;
class FontMesh;

class TextComponent : public Component
{
private:
	Font* font;
	FontMesh* fontMesh;
	std::vector<Text> textList;
	bool shouldUpdate;

public:

	TextComponent(FontMesh* fontMesh, Font* font);

	// Component을(를) 통해 상속됨
	void Start(GraphicsContext* gfx) override;
	void Input() override;
	void Update(float dt) override;
	void Render(GraphicsContext* gfx) override;

	void InputText(const wchar_t& key);

	void InputTextAndUpdateMesh(const wchar_t& key, GraphicsContext* gfx);


};

