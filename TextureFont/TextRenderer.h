#pragma once
#include "ObjectBase.hpp"
#include "MeshRenderer.hpp"	

class Font;
class FontMesh;

class TextRenderer : public MeshRenderer
{
private:
	Font* font;
	FontMesh* fontMesh;
	std::vector<Text> textList;
	bool shouldUpdate;

public:

	TextRenderer(FontMesh* fontMesh, Material* fontMat, Font* font);

	// Component을(를) 통해 상속됨
	void Start(GraphicsContext* gfx) override;
	void Input() override;
	void Update(float dt) override;
	void Render(GraphicsContext* gfx) override;

	void InputText(const wchar_t& key);


};

