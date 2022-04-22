#pragma once

class BulletType
{
private:
	Sprite* sprite;
	friend class BulletFactory;
	int width;
	int height;
	std::string directory = "data\\";
	std::string type_name;
	BulletType(std::string type_name, std::string filename)
	{
		this->type_name = type_name;
		std::string final = (this->directory + filename).c_str();
		const char* file = &final[0];
		sprite = createSprite(file);
		getSpriteSize(sprite, this->width, this->height);
	}
public:

	std::string& GetTypeName()
	{
		return type_name;
	}

	Sprite* GetSprite()
	{
		return sprite;
	}

	int GetWidth()
	{
		return width;
	}

	int GetHeight()
	{
		return height;
	}

	virtual ~BulletType()
	{
		destroySprite(sprite);
	}// delete sprite; }


};

class BulletFactory
{
private:

	friend class BulletType;
	static std::map<std::string, BulletType*> bulletype_map;
public:

	static BulletType* CreateBulletType(std::string bulletname, std::string filename)
	{
		std::map<std::string, BulletType*>::iterator it = bulletype_map.find(bulletname);
		if (it != bulletype_map.end())
		{
			return it->second;
		}
		else
		{
			BulletType* type = new BulletType(bulletname, filename);
			bulletype_map.insert(std::make_pair(bulletname, type));
			return type;
		}

	}

};

class Bullet
{
private:
	BulletType* type;
	friend class BulletMagazine;
	Point start;
	Point current;
	Point finish;
	Point step;
	float speed;
public:

	Bullet(BulletType* type, Point& start, Point& finish, float speed)
	{
		this->type = type;
		this->current = this->start = start;
		this->finish = finish;
		this->speed = speed;
		if (start == detached || finish == detached) {
			;
		}
		else
		{
			step = FindStepToFinish();//пулю створили но вона знаходиться поза межами екрану
		}
	}

	void DrawBullet()
	{
		drawSprite(type->GetSprite(), current.GetX(), current.GetY());//пристосував функцию задану
	}


	Point& FindStepToFinish()//логіка руху кулі а саме знахлдження її кроку до цілі
	{//Функція та сама шо і функція руху астероїда до точки
		float finish_x = finish.GetX() - 10;
		float finish_y = finish.GetY() - 10;

		float start_x = start.GetX();
		float start_y = start.GetY();

		step = Point((finish.GetX() - start.GetX()) / g_bullet_speed, (finish.GetY() - start.GetY()) / g_bullet_speed);

		return step;
	}

	void MoveBullet()
	{
		if (current == detached)
		{
			return;
		}
		current = current + step;
	}

	bool isOutOfBounds()//перевіряєм чи пуля покинула екран цілком
	{
		if (current != detached) {
			float cur_x_pos = current.GetX();
			float cur_y_pos = current.GetY();

			if (cur_x_pos > g_width + 100 || cur_x_pos < -50 || cur_y_pos > g_height + 100 || cur_y_pos < -50)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	virtual ~Bullet() { delete type; }//delete type; 


};

class BulletMagazine
{
private:
	std::list<Bullet*> magazine;

public:

	void InsertBullet(Bullet* bullet)
	{
		magazine.push_back(bullet);
	}

	void InsertBullet(BulletType* type, Point& start, Point& finish, float speed)
	{
		magazine.push_back(new Bullet(type, start, finish, speed));
	}

	void DrawMagazine()
	{

		for (auto& bullet : magazine)
		{
			if (bullet->current != detached)
				bullet->DrawBullet();
		}


	}

	void MoveMagazine()
	{
		for (auto& bullet : magazine)
		{
			if (bullet->current != detached)
				bullet->MoveBullet();
		}

		for (std::list<Bullet*>::iterator iter = magazine.begin(); iter != magazine.end(); )
		{
			bool flag = false;
			if ((*iter)->isOutOfBounds())
			{
				flag = true;
			}

			if (flag)
			{
				std::list<Bullet*>::iterator prev = iter;
				iter++;
				magazine.remove(*prev);
				std::cout << "ammo left " << magazine.size() << std::endl;
			}
			else
			{
				iter++;
			}

		}
	}

	void CheckForCollision(AsteroidField& asteroid_field)//тепер треба чи пуля попадає в якись астероїх і кшо так то великий ділить на два, а маленькй вбиває
	{//функція перебирає і віддає всі полощини астероїдів
		for (std::list<Bullet*>::iterator current_bullet = magazine.begin(); current_bullet != magazine.end(); current_bullet++)
		{
			if ((*current_bullet)->current == detached)
			{
				continue;
			}

			int first_bullet_beginning_point_X = (*current_bullet)->current.GetX() - 5;
			int first_bullet_ending_point_x = first_bullet_beginning_point_X + (*current_bullet)->type->GetWidth() + 10;

			int first_bullet_beginning_point_y = (*current_bullet)->current.GetY() - 5;
			int first_bullet_ending_point_y = first_bullet_beginning_point_y + (*current_bullet)->type->GetHeight() + 10;

			for (std::list<Asteroid*>::iterator current_asteroid = asteroid_field.GetAsteroids()->begin(); current_asteroid != asteroid_field.GetAsteroids()->end(); current_asteroid++)
			{
				int second_asteroid_beginning_point_x = (*current_asteroid)->current.GetX() - 5;
				int second_asteroid_ending_point_x = (*current_asteroid)->current.GetX() + (*current_asteroid)->type->GetWidth() + 10;

				int second_asteroid_beginning_point_y = (*current_asteroid)->current.GetY() - 5;
				int second_asteroid_ending_point_y = (*current_asteroid)->current.GetY() + (*current_asteroid)->type->GetHeight() + 10;

				//це початкова точка відліку кулі                тут його ширина
				for (int i = first_bullet_beginning_point_X - 2; i < first_bullet_ending_point_x + 2; i++)//проходимось по всій ширині 1го астероїда
				{
					if (i >= first_bullet_beginning_point_X - 2 && i <= first_bullet_beginning_point_X + 1)//ліва сторона порівнюваного астероїда
					{//тут ми походитимо по всіх точках порівнюваного астороїда по лівій стороні , таких точок буде вся висота
						for (int vertical = first_bullet_beginning_point_y; vertical < first_bullet_ending_point_y; vertical++)//проходимось по всіх точках 1-шого астероїда по вертикалі
						{//діло в тому шо до лівої сторони 1-го астероїд, 2-ий можу доторкатись тільки правою стороною 
							for (int j = second_asteroid_ending_point_x - 1; j < second_asteroid_ending_point_x + 1; j++)//всі точки другого астероїда по горизонталі
							{
								for (int second_vertical = second_asteroid_beginning_point_y; second_vertical < second_asteroid_ending_point_y; second_vertical++)
								{
									if (i == j && vertical == second_vertical)
									{
										std::cout << "Bullet left side of first to right side of second" << std::endl;

										magazine.remove(*current_bullet);//видаляєм пулю в будь якому випадку

										if ((*current_asteroid)->type->GetTypeName() == g_big_asteroid_name)//якшо великикй астероїд видаляєм його
										{//а натомість поміщаємо в список астероїдів два нових маленьких які стартують з одної точки

											Point start_for_small_1 = (*current_asteroid)->current;
											Point start_for_small_2 = start_for_small_1 + Point(50, 50);

											asteroid_field.GetAsteroids()->remove(*current_asteroid);

											AsteroidBuilder ab(g_asteroid_speed);
											asteroid_field.GetAsteroids()->push_back(ab.BuildSmallAsteroidWithStart(start_for_small_1));
											asteroid_field.GetAsteroids()->push_back(ab.BuildSmallAsteroidWithStart(start_for_small_2));

										}
										else if ((*current_asteroid)->type->GetTypeName() == g_small_asteroid_name)
										{
											asteroid_field.GetAsteroids()->remove(*current_asteroid);
										}

										return;
									}
								}
							}
						}
					}
					else if (i > first_bullet_beginning_point_X && i < first_bullet_ending_point_x)//по х більше за перший елемнт але менше за перед останній того -2
					{
						for (int first_vertical = first_bullet_beginning_point_y - 1; first_vertical < first_bullet_beginning_point_y + 1; first_vertical++)//верх першого доторкається до низу другого
						{
							for (int second_horisontal = second_asteroid_beginning_point_x; second_horisontal < second_asteroid_ending_point_x; second_horisontal++)
							{
								for (int second_last_vertical = second_asteroid_ending_point_y - 1; second_last_vertical < second_asteroid_ending_point_y + 1; second_last_vertical++)
								{
									if (i == second_horisontal && first_vertical == second_last_vertical)
									{
										std::cout << "Bullet top to bottom sides two asteroids" << std::endl;

										magazine.remove(*current_bullet);//видаляєм пулю в будь якому випадку

										if ((*current_asteroid)->type->GetTypeName() == g_big_asteroid_name)//якшо великикй астероїд видаляєм його
										{//а натомість поміщаємо в список астероїдів два нових маленьких які стартують з одної точки

											
											Point start_for_small_2 = Point((*current_asteroid)->current.GetX(), (*current_asteroid)->current.GetY() + (*current_asteroid)->type->GetHeight()+20);
											AsteroidBuilder ab(g_asteroid_speed);
											asteroid_field.GetAsteroids()->push_back(ab.BuildSmallAsteroidWithStart((*current_asteroid)->current));
											asteroid_field.GetAsteroids()->push_back(ab.BuildSmallAsteroidWithStart(start_for_small_2));
											asteroid_field.GetAsteroids()->remove(*current_asteroid);
										}
										else if ((*current_asteroid)->type->GetTypeName() == g_small_asteroid_name)
										{
											asteroid_field.GetAsteroids()->remove(*current_asteroid);
										}

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
										std::cout << "Bullet bottom to top sides two asteroids" << std::endl;

										magazine.remove(*current_bullet);//видаляєм пулю в будь якому випадку

										if ((*current_asteroid)->type->GetTypeName() == g_big_asteroid_name)//якшо великикй астероїд видаляєм його
										{//а натомість поміщаємо в список астероїдів два нових маленьких які стартують з одної точки
											Point start_for_small_1 = (*current_asteroid)->current;
											Point start_for_small_2 = Point(start_for_small_1.GetX(), start_for_small_1.GetY() - (*current_asteroid)->type->GetHeight()-20 );


											
											AsteroidBuilder ab(g_asteroid_speed);

											asteroid_field.GetAsteroids()->push_back(ab.BuildSmallAsteroidWithStart(start_for_small_1));
											asteroid_field.GetAsteroids()->push_back(ab.BuildSmallAsteroidWithStart(start_for_small_2));

											asteroid_field.GetAsteroids()->remove(*current_asteroid);
										}
										else if ((*current_asteroid)->type->GetTypeName() == g_small_asteroid_name)
										{
											asteroid_field.GetAsteroids()->remove(*current_asteroid);
										}

										return;//обновити траэкторыю ъхнього руху
									}
								}
							}
						}

					}
					else if (i >= first_bullet_ending_point_x)
					{
						for (int first_vertical = first_bullet_beginning_point_y; first_vertical < first_bullet_ending_point_y; first_vertical++)
						{
							for (int second_horisontal = second_asteroid_beginning_point_x - 1; second_horisontal < second_asteroid_beginning_point_x + 1; second_horisontal++)
							{
								for (int second_vertical = second_asteroid_beginning_point_y; second_vertical < second_asteroid_ending_point_y; second_vertical++)
								{
									if (i == second_horisontal && first_vertical == second_vertical)
									{
										std::cout << "Bullet right side of first collided with left side of second" << std::endl;

										magazine.remove(*current_bullet);//видаляєм пулю в будь якому випадку

										if ((*current_asteroid)->type->GetTypeName() == g_big_asteroid_name)//якшо великикй астероїд видаляєм його
										{//а натомість поміщаємо в список астероїдів два нових маленьких які стартують з одної точки

											Point start_for_small_2 = (*current_asteroid)->current;
											Point start_for_small_1 = Point(start_for_small_1.GetX() + (*current_asteroid)->type->GetWidth()+10, start_for_small_1.GetY());

											
											AsteroidBuilder ab(g_asteroid_speed);
											asteroid_field.GetAsteroids()->push_back(ab.BuildSmallAsteroidWithStart(start_for_small_1));
											asteroid_field.GetAsteroids()->push_back(ab.BuildSmallAsteroidWithStart(start_for_small_2));

											asteroid_field.GetAsteroids()->remove(*current_asteroid);
										}
										else if ((*current_asteroid)->type->GetTypeName() == g_small_asteroid_name)
										{
											asteroid_field.GetAsteroids()->remove(*current_asteroid);
										}

										return;
									}
								}
							}
						}

					}
				}
			}
		}
	}

	std::list<Bullet*>* GetMagazine()
	{
		return &magazine;
	}

	bool isEmpty()
	{
		return magazine.size() == 0;
	}

	void FireFirstBullet(Bullet* bullet, Point& start, Point& end)
	{
		bullet->start = bullet->current = start;//задаєм точку початку яка є точкою гравця
		bullet->finish = end;//задаєм точку кінця яка є точкою курсора
		bullet->FindStepToFinish();//знаходимо крок пулі

	}
};

class BulletBuilder
{
private:

	//Point point;
	float speed;

public:

	BulletBuilder(float speed) :speed(speed)
	{

	}

	Bullet* BuildBullet()
	{
		BulletType* type = BulletFactory::CreateBulletType(g_regular_bullet_name, "bullet.png");
		int width = type->GetWidth(), height = type->GetHeight();
		return new Bullet(type, detached, detached, speed);
	}

	Bullet* BuildBullet(Point& start, Point& end)
	{
		BulletType* type = BulletFactory::CreateBulletType(g_regular_bullet_name, "bullet.png");
		int width = type->GetWidth(), height = type->GetHeight();
		return new Bullet(type, start, end, speed);
	}

};
