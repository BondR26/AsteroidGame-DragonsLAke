#pragma once
#include<string>
#include<iostream>
#include<map>
#include<list>

class AsteroidType
{
private:
	Sprite* sprite;
	int width;
	int height;
	static std::string picture_folder;
	std::string type_name;
	friend class AsteroidTypeFactory;//для того щоб функцію створення було видно було видно в фабриці
	AsteroidType(std::string type_name,std::string filename)
	{
		this->type_name = type_name;
		std::string final = (this->picture_folder + filename).c_str();
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

	virtual ~AsteroidType()
	{
		destroySprite(sprite);
	}// delete sprite; }



};

class AsteroidTypeFactory
{
private:
	static std::map<std::string, AsteroidType*> asteroidtype_map;
public:

	static AsteroidType* CreateAsteroidType(std::string asteroid_shape, std::string asteroid_filename)
	{
		std::map<std::string, AsteroidType*>::iterator it = asteroidtype_map.find(asteroid_shape);

		if (it != asteroidtype_map.end())
		{
			return it->second;
		}
		else
		{
			asteroidtype_map.insert(std::make_pair(asteroid_shape, new AsteroidType(asteroid_shape,asteroid_filename)));
			return asteroidtype_map[asteroid_shape];
		}
	}
};

class Asteroid
{
private:
	friend class BulletMagazine;  
	friend class AsteroidField;
	friend class Player;
	Point start;
	Point current;
	Point finish;
	Point step;
	float speed;
	AsteroidType* type;
public:

	Asteroid(AsteroidType* type, Point start, Point finish, float speed)
	{
		this->type = type;
		this->current = this->start = start;
		this->finish = finish;
		this->speed = speed;
		step = FindStepToFinish();
	}

	void DrawAsteroid()
	{
		drawSprite(type->GetSprite(), current.GetX(), current.GetY());//пристосував функцию задану
	}

	Point& GetCurrent()
	{
		return current;
	}

	Point FindStepToFinish()//логіка руху астероїда а саме знахлдження його кроку до цілі
	{
		float finish_x = finish.GetX() - 10;
		float finish_y = finish.GetY() - 10;

		float start_x = start.GetX();
		float start_y = start.GetY();

		step = Point((finish.GetX() - start.GetX()) / g_asteroid_speed, (finish.GetY() - start.GetY()) / g_asteroid_speed);
		
		return step;
	}

	void MoveAsteroid()
	{
		if (isOutOfBounds())//якшо астероїд досяг границь треба задати нову фінальну точку його руху і обновити крок
		{
			Point point;
			start = current;//тепер current це новий старт
			finish = point.RandomPointInTheScreen(this->type->GetWidth(), this->type->GetHeight());
			
			step = FindStepToFinish();

		/*	std::cout << "start" << start; //відслідковую положення астероїда
			std::cout << "current" << current;//поточне
			std::cout << "finish "<< finish; //куди рухається
			std::cout << "step "<<step;*/ //крок 
		}
		current = current + step;
	}

	bool isOutOfBounds()
	{
		int width, height;
		getSpriteSize(this->type->GetSprite(), width, height);
		int cur_x_pos = current.GetX();
		int cur_y_pos = current.GetY();

		//4 spots

		if (cur_x_pos < 2)//
		{
			return true;
		}
		else if (cur_x_pos + width > g_width-10)
		{
			return true;
		}
		else if (cur_y_pos < 2)
		{
			return true;
		}
		else if (cur_y_pos + height > g_height - 10)
		{
			return true;
		}

		return false;
	}

	virtual ~Asteroid() { delete type; }//delete type; 
};

class AsteroidField
{
private:
	std::list<Asteroid*> asteroids;
public:

	void InsertAsteroid(AsteroidType* type, Point& start, Point& finish, int speed)
	{
		asteroids.push_back(new Asteroid(type, start, finish, speed));
	}

	void InsertAsteroid(Asteroid* asteroid)
	{
		asteroids.push_back(asteroid);
	}

	void DrawAsteroidField()
	{
		for (auto& asteroid : asteroids)
		{
			asteroid->DrawAsteroid();
		}
	}

	std::list<Asteroid*>* GetAsteroids()
	{
		return &asteroids;
	}
	//ДОПРАЦЮВАТИ МОЖНА ТАК ШОБ СКРУШЛИТИ ЯКОСЬ КІНЦЕВІ ТОЧКИ АСЕТРОЇДІВ
	void CheckForCollision()//час перевірки на стики якраз може замінить оцю паузу для заторможення руху астероїдів
	{//функція перебирає і віддає всі полощини астероїдів
		for (std::list<Asteroid*>::iterator it = asteroids.begin(); it != asteroids.end(); it++)
		{
			int first_asteroid_beginning_point_X = (*it)->current.GetX();
			int first_asteroid_ending_point_x = first_asteroid_beginning_point_X + (*it)->type->GetWidth();

			int first_asteroid_beginning_point_y = (*it)->current.GetY();
			int first_asteroid_ending_point_y = (*it)->current.GetY() + (*it)->type->GetHeight();

			for (std::list<Asteroid*>::iterator iter = it; iter != asteroids.end(); iter++)
			{
				
				int second_asteroid_beginning_point_x = (*iter)->current.GetX();
				int second_asteroid_ending_point_x = (*iter)->current.GetX() + (*iter)->type->GetWidth();

				int second_asteroid_beginning_point_y = (*iter)->current.GetY();
				int second_asteroid_ending_point_y = (*iter)->current.GetY() + (*iter)->type->GetHeight();

				//це початкова точка відліку астероїда                тут його ширина
				for (int i = first_asteroid_beginning_point_X-2; i < first_asteroid_ending_point_x+2; i++)//проходимось по всій ширині 1го астероїда
				{
					if (i == first_asteroid_beginning_point_X-2|| i == first_asteroid_beginning_point_X - 1 || i == first_asteroid_beginning_point_X - 0)//ліва сторона порівнюваного астероїда
					{//тут ми походитимо по всіх точках порівнюваного астороїда по лівій стороні , таких точок буде вся висота
						for (int vertical = first_asteroid_beginning_point_y; vertical < first_asteroid_ending_point_y; vertical++)//проходимось по всіх точках 1-шого астероїда по вертикалі
						{//діло в тому шо до лівої сторони 1-го астероїд, 2-ий можу доторкатись тільки правою стороною 
							for (int j = second_asteroid_ending_point_x-1; j < second_asteroid_ending_point_x + 1; j++)//всі точки другого астероїда по горизонталі
							{
								for (int second_vertical = second_asteroid_beginning_point_y; second_vertical < second_asteroid_ending_point_y; second_vertical++)
								{
									if (i == j && vertical == second_vertical)
									{
										//std::cout << "collided left side of first to right side of second" << std::endl;

										(*it)->finish.GetX() = g_width - (*it)->finish.GetX() - (*it)->type->GetWidth();
										(*it)->start = (*it)->current;
										(*it)->step = (*it)->FindStepToFinish();


										(*iter)->finish.GetX() = g_width - (*iter)->finish.GetX();
										(*iter)->start = (*iter)->current;
										(*iter)->step = (*iter)->FindStepToFinish();

										return;
									}
								}
							}
						}
					}
					else if (i > first_asteroid_beginning_point_X && i < first_asteroid_ending_point_x)//по х більше за перший елемнт але менше за перед останній того -2
					{
						for (int first_vertical = first_asteroid_beginning_point_y-1; first_vertical < first_asteroid_beginning_point_y+1; first_vertical++)//верх першого доторкається до низу другого
						{
							for (int second_horisontal = second_asteroid_beginning_point_x; second_horisontal < second_asteroid_ending_point_x; second_horisontal++)
							{
								for (int second_last_vertical = second_asteroid_ending_point_y - 1; second_last_vertical < second_asteroid_ending_point_y +1; second_last_vertical++)
								{
									if (i == second_horisontal && first_vertical == second_last_vertical)
									{
										//std::cout << "collided top to bottom sides two asteroids" << std::endl;
										(*it)->finish.GetY() = g_height - (*it)->finish.GetY()- (*it)->type->GetHeight();
										(*it)->start = (*it)->current;
										(*it)->step = (*it)->FindStepToFinish();


										(*iter)->finish.GetY() = g_height - (*iter)->finish.GetY();
										(*iter)->start = (*iter)->current;
										(*iter)->step = (*iter)->FindStepToFinish();
										return;
									}
								}
							}
						}

						for (int first_asteroid_last_vertical = first_asteroid_ending_point_y-1; first_asteroid_last_vertical < first_asteroid_ending_point_y+1; first_asteroid_last_vertical++)
						{
							for (int second_asteroid_horisontal = second_asteroid_beginning_point_x; second_asteroid_horisontal < second_asteroid_ending_point_x; second_asteroid_horisontal++)
							{
								for (int second_asteroid_top_vertical = second_asteroid_beginning_point_y - 1; second_asteroid_top_vertical < second_asteroid_beginning_point_y + 1; second_asteroid_top_vertical++)
								{
									if (i == second_asteroid_horisontal && first_asteroid_last_vertical == second_asteroid_top_vertical)
									{
										//std::cout << "collided bottom to top sides two asteroids" << std::endl;
										
										(*it)->finish.GetY() = g_height - (*it)->finish.GetY();
										(*it)->start = (*it)->current;
										(*it)->step = (*it)->FindStepToFinish();


										(*iter)->finish.GetY() = g_height - (*iter)->finish.GetY()- (*it)->type->GetHeight();
										(*iter)->start = (*iter)->current;
										(*iter)->step = (*iter)->FindStepToFinish();
										return;//обновити траэкторыю ъхнього руху
									}
								}
							}
						}

					}
					else if (i >= first_asteroid_ending_point_x - 1)
					{
						for (int first_vertical = first_asteroid_beginning_point_y; first_vertical < first_asteroid_ending_point_y ; first_vertical++)
						{
							for (int second_horisontal = second_asteroid_beginning_point_x - 1; second_horisontal < second_asteroid_beginning_point_x + 1; second_horisontal++)
							{
								for (int second_vertical = second_asteroid_beginning_point_y; second_vertical < second_asteroid_ending_point_y; second_vertical++)
								{
									if (i == second_horisontal && first_vertical == second_vertical)
									{
										//std::cout << "right side of first collided with left side of second"<<std::endl;
										(*it)->finish.GetX() = g_width - (*it)->finish.GetX();
										(*it)->start = (*it)->current;
										(*it)->step = (*it)->FindStepToFinish();


										(*iter)->finish.GetX() = g_width - (*iter)->finish.GetX() - (*it)->type->GetWidth();
										(*iter)->start = (*iter)->current;
										(*iter)->step = (*iter)->FindStepToFinish();
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

	void MoveAsteroidField()
	{
		for (auto& asteroid : asteroids)
		{
			asteroid->MoveAsteroid();
		}
	}
};


class AsteroidBuilder
{
private:
	Point point;
	float speed;
public:

	AsteroidBuilder(float speed) :speed(speed)
	{

	}

	Asteroid* BuildBigAsteroid()
	{
		AsteroidType* type = AsteroidTypeFactory::CreateAsteroidType(g_big_asteroid_name, "big_asteroid.png");
		int width = type->GetWidth(), height = type->GetHeight();
		return new Asteroid(type, point.RandomPointInTheScreen(width, height), point.RandomPointAroundTheScreen(width, height), speed);
	}

	Asteroid* BuildSmallAsteroid()
	{
		AsteroidType* type = AsteroidTypeFactory::CreateAsteroidType(g_small_asteroid_name, "small_asteroid.png");
		int width = type->GetWidth(), height = type->GetHeight();
		return new Asteroid(type, point.RandomPointInTheScreen(width, height), point.RandomPointAroundTheScreen(width, height), speed);
	}

	Asteroid* BuildSmallAsteroidWithStart(Point start)
	{
		AsteroidType* type = AsteroidTypeFactory::CreateAsteroidType(g_small_asteroid_name, "small_asteroid.png");
		int width = type->GetWidth(), height = type->GetHeight();
		return new Asteroid(type, start, point.RandomPointAroundTheScreen(width, height), speed);
	}

};