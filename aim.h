#pragma once

class Aim
{
private:
	Sprite* sprite;
	Point current;
public:
	Sprite* GetSprite()
	{
		return sprite;
	}
	Sprite* SetAim(const char* filename)
	{
		this->sprite = createSprite(filename);
		current = Point(-1, -1);
		return this->sprite;
	}

	Point& GetCurrent()
	{
		return current;
	}

	void MoveAim(Point current)
	{
		this->current = current;
	}

	void DrawSprite()
	{
		drawSprite(sprite, current.GetX(), current.GetY());
	}

	virtual ~Aim() { destroySprite(sprite); }
};
