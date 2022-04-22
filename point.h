#pragma once
#include<time.h>
#include<iostream>

class Point
{
private:
	float x;
	float y;
public:
	Point() { x = 0; y = 0; }
	Point(float x, float y) :x(x), y(y) {}
	bool operator==(const Point& other)
	{
		return this->x == other.x && this->y == other.y;
	}

	bool operator != (const Point& other)
	{
		return !(*this == other);
	}

	float& GetX()
	{
		return this->x;
	}

	float& GetY()
	{
		return this->y;
	}

	friend std::ostream& operator << (std::ostream& out, const Point& other)
	{
		out << " X " << other.x << " y "<< other.y << std::endl;
		return out;


	}

	Point RandomPointAroundTheScreen(int asteroid_width, int asteroid_height)
	{
		//srand(time(NULL));

		float x_pos = 0, y_pos=0;
		int situation = rand() % 3;
		//one chance from three that it either goind to be 0 end of screen width or in between
		switch (situation)
		{
		case 0://ситуація нуль, при якій в нас вибираєтся рандомне числов по всій лінії х , та вибирається чи це буде знизу чи зверху по лінії у
		{//(верх або низ екрану)
			x_pos = rand() % (g_width - asteroid_width) + 5;
			switch (rand() % 2)
			{
			case 0:
			{
				y_pos = 5;
			}break;

			case 1:
			{
				y_pos = g_height - asteroid_height-5;
			}break;
			}

		}break;
		case 1://ситуація коли в нас вибирається нуль по х та рандомне число по у(ліва сторона екрану)
		{
			x_pos = 5;

			y_pos = rand() % (g_height - asteroid_height) -5;

		}break;
		case 2://останнє значення ширини екрану і по у вибирається рандом
		{//(права сторона екрану)
			x_pos = g_width-asteroid_width-5;
			y_pos = rand() % (g_height - asteroid_height) - 5;
		}break;
		}


		return Point(x_pos, y_pos);
	}

	Point RandomPointInTheScreen(int asteroid_width, int asteroid_height, int size_of_avatar = 301)
	{
		//srand(time(NULL));
		
		int y_pos = -10;
		int x_pos = -10;

		while (x_pos < 0 || x_pos > g_width - asteroid_width && y_pos < 0 || y_pos > g_height - asteroid_height)
		{
			
			x_pos = rand() % (g_width)- asteroid_width;
			if (x_pos > g_width / 2 - size_of_avatar && x_pos < g_width / 2 + size_of_avatar)
			{
				switch (rand() % 2)
				{
				case 0:
				{
					y_pos = rand() % (g_height / 2 - size_of_avatar);
				}break;
				case 1:
				{
					y_pos = rand() % (g_height / 2 - asteroid_height) + (g_height / 2 + size_of_avatar);
				}break;
				}
			}
			else
			{
				y_pos = rand() % (g_height - asteroid_height - 10) + 10;
			}
		}

		return Point(x_pos, y_pos);
	}

	Point operator+(Point other) const
	{
		return Point(this->x + other.x, this->y + other.y);
	}

	virtual ~Point() {}
};