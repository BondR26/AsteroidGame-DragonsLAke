#pragma once
class BackGround
{
private:
	Sprite* sprite;
	int width;
	int height;
	//BackGround(){}
public:
	Sprite* CreateBackGround(std::string filename)
	{
		if (sprite == nullptr)
		{
			sprite = createSprite(("data\\" + filename).c_str());
			getSpriteSize(sprite, width, height);
		}
		else
		{
			return sprite;
		}
	}

	Sprite* GetSprite()
	{
		return sprite;
	}

	void DrawBackground()
	{
		for (int i = 0; i < g_width; i += this->width)
		{
			for (int j = 0; j < g_height; j += height)
				drawSprite(sprite, i, j);
		}
	}

	virtual ~BackGround() { destroySprite(sprite); }
};