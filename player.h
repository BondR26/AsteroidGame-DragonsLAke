#pragma once
class Player
{
private:
	Sprite* sprite;
	Point start;
	Point current;
	Point finish;
	int width, height;
public:

	//Player() { sprite = nullptr; }

	Sprite* GetSprite()
	{
		return sprite;
	}

	Point& GetCurrent()
	{
		return current;
	}

	Sprite* SetPlayer(Sprite* sprite)
	{
		this->sprite = sprite;
		start = current = Point(g_width / 2, g_height / 2);
		getSpriteSize(this->sprite, width, height);
		return sprite;
	}

	void MovePlayer(Point step)
	{
		current = current + step;
		isOutOfBound();
	}

	bool isOutOfBound()
	{
		if (current.GetX() > g_width + this->width)
		{
			current.GetX() = 0;
			return true;
		}
		else if (current.GetX() < 0)
		{
			current.GetX() = g_width;
			return true;
		}
		else if (current.GetY() > g_height)
		{
			current.GetY() = 0;
			return true;
		}
		else if (current.GetY() < 0)
		{
			current.GetY() = g_height;
			return true;
		}
		return false;
	}

	void DrawPlayer()
	{
		drawSprite(sprite, current.GetX(), current.GetY());
	}

	void CheckForCollision(AsteroidField& asteroid_field)//����� ����� �� ���� ������ � ����� ������� � ��� ��� �� ������� ����� �� ���, � �������� �����
	{//������� �������� � ���� �� �������� ��������

		int first_bullet_beginning_point_X = current.GetX();
		int first_bullet_ending_point_x = first_bullet_beginning_point_X + width;

		int first_bullet_beginning_point_y = current.GetY();
		int first_bullet_ending_point_y = first_bullet_beginning_point_y + height;

		for (std::list<Asteroid*>::iterator current_asteroid = asteroid_field.GetAsteroids()->begin(); current_asteroid != asteroid_field.GetAsteroids()->end(); current_asteroid++)
		{
			int second_asteroid_beginning_point_x = (*current_asteroid)->current.GetX();
			int second_asteroid_ending_point_x = (*current_asteroid)->current.GetX() + (*current_asteroid)->type->GetWidth();

			int second_asteroid_beginning_point_y = (*current_asteroid)->current.GetY();
			int second_asteroid_ending_point_y = (*current_asteroid)->current.GetY() + (*current_asteroid)->type->GetHeight();

			//�� ��������� ����� ����� ���                ��� ���� ������
			for (int i = first_bullet_beginning_point_X - 2; i < first_bullet_ending_point_x + 2; i++)//����������� �� ��� ����� 1�� ��������
			{
				if (i == first_bullet_beginning_point_X - 2 || i == first_bullet_beginning_point_X - 1 || i == first_bullet_beginning_point_X - 0)//��� ������� ������������ ��������
				{//��� �� ���������� �� ��� ������ ������������ �������� �� ��� ������ , ����� ����� ���� ��� ������
					for (int vertical = first_bullet_beginning_point_y; vertical < first_bullet_ending_point_y; vertical++)//����������� �� ��� ������ 1-���� �������� �� ��������
					{//��� � ���� �� �� ��� ������� 1-�� �������, 2-�� ���� ����������� ����� ������ �������� 
						for (int j = second_asteroid_ending_point_x - 1; j < second_asteroid_ending_point_x + 1; j++)//�� ����� ������� �������� �� ����������
						{
							for (int second_vertical = second_asteroid_beginning_point_y; second_vertical < second_asteroid_ending_point_y; second_vertical++)
							{
								if (i == j && vertical == second_vertical)
								{
									std::cout << "SHip was destoryued" << std::endl;

									exit(0);

									return;
								}
							}
						}
					}
				}
				else if (i > first_bullet_beginning_point_X && i < first_bullet_ending_point_x)//�� � ����� �� ������ ������ ��� ����� �� ����� ������� ���� -2
				{
					for (int first_vertical = first_bullet_beginning_point_y - 1; first_vertical < first_bullet_beginning_point_y + 1; first_vertical++)//���� ������� ����������� �� ���� �������
					{
						for (int second_horisontal = second_asteroid_beginning_point_x; second_horisontal < second_asteroid_ending_point_x; second_horisontal++)
						{
							for (int second_last_vertical = second_asteroid_ending_point_y - 1; second_last_vertical < second_asteroid_ending_point_y + 1; second_last_vertical++)
							{
								if (i == second_horisontal && first_vertical == second_last_vertical)
								{
									//std::cout << "Bullet top to bottom sides two asteroids" << std::endl;

									std::cout << "SHip was destoryued" << std::endl;

									exit(0);

									return;
								}
							}
						}
					}

					for (int first_asteroid_last_vertical = first_bullet_ending_point_y - 1; first_asteroid_last_vertical < first_bullet_ending_point_y + 1; first_asteroid_last_vertical++)
					{
						for (int second_asteroid_horisontal = second_asteroid_beginning_point_x; second_asteroid_horisontal < second_asteroid_ending_point_x; second_asteroid_horisontal++)
						{
							for (int second_asteroid_top_vertical = second_asteroid_beginning_point_y - 1; second_asteroid_top_vertical < second_asteroid_beginning_point_y + 1; second_asteroid_top_vertical++)
							{
								if (i == second_asteroid_horisontal && first_asteroid_last_vertical == second_asteroid_top_vertical)
								{
									//std::cout << "Bullet bottom to top sides two asteroids" << std::endl;
									std::cout << "SHip was destoryued" << std::endl;

									exit(0);

									return;//�������� ���������� ������� ����
								}
							}
						}
					}

				}
				else if (i >= first_bullet_ending_point_x - 1)
				{
					for (int first_vertical = first_bullet_beginning_point_y; first_vertical < first_bullet_ending_point_y; first_vertical++)
					{
						for (int second_horisontal = second_asteroid_beginning_point_x - 1; second_horisontal < second_asteroid_beginning_point_x + 1; second_horisontal++)
						{
							for (int second_vertical = second_asteroid_beginning_point_y; second_vertical < second_asteroid_ending_point_y; second_vertical++)
							{
								if (i == second_horisontal && first_vertical == second_vertical)
								{
									//std::cout << "Bullet right side of first collided with left side of second" << std::endl;

									std::cout << "SHip was destoryued" << std::endl;

									exit(0);

									return;
								}
							}
						}
					}

				}
			}
		}
	}

	virtual ~Player() { destroySprite(sprite); }

};



