#include<iostream>
//#include<filesystem>
#include<list>
#include<map>
#include<thread>
#include<chrono>
#include<mutex>
//#include<ctime>
#include<vector>
//#include<conio.h>
//my libs
static int g_width;//глобальні змінні видно з усіх класів
static int g_height;
static int g_asteroids_num;
static int g_bullets_num;
static int g_probability_num;//not used
static int g_map_width;//not used
static int g_map_height;//not used

#define g_big_asteroid_name "big"
#define g_small_asteroid_name "small"
#define g_regular_bullet_name "bullet"
#define g_asteroid_speed 150.00
#define g_bullet_speed 30.00
#define g_space_ship_speed_increase 0.2
#define g_space_ship_speed_decrease 0.2
#include"Framework.h"
#include "point.h"
#include"asteroid_flyweight.h"
//#include "player.h"
static Point detached = Point(-10, -10);//умовне значення яке означає шо спрайт вне игри 
//його тільки треба підключати між aim  i flyweight asteroid
#include "aim.h"
#include "bullet.h"
#include "background.h"
#include"player.h"

//паттерном будівельник будувати або маленький астероїд або великий і навчити один великий заміняти на 2 малих


//напсисати команду, яка б розставляла астероїди в рандомному порядку і рандомну кількість

/* Test Framework realization */
class MyFramework: public Framework {

public:

 	AsteroidField asteroid_field;
	Player player;
	BulletMagazine bullet_magazine;
	std::list<Bullet*>::iterator bullet;
	BackGround background;
	Aim aim;
	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = g_width;
		height = g_height;
		fullscreen = false;
		
	}
	
	virtual bool Init()//тільки в цій функції можна робитит підтягування спрайтів
	{
		
		//drawTestBackground();
		showCursor(false);
		
		aim.SetAim("data\\enemy.png");

		player.SetPlayer(createSprite("data\\spaceship.png"));
		
		background.CreateBackGround("background.png");
		
		BulletBuilder bulletbuilder(g_bullet_speed);
		
		std::mutex bullet_mutex;

		std::thread bullet_thread([&]() {
			
			std::unique_lock<std::mutex> bulltes_mutex_lock(bullet_mutex);
			for (int i = 0; i < g_bullets_num; i++)
			{
				bullet_magazine.InsertBullet(bulletbuilder.BuildBullet());
			}

			bullet = bullet_magazine.GetMagazine()->begin();//вставляєм пергий патрон в обойму
		});
		
		Point point;
		AsteroidBuilder builder(g_asteroid_speed);
		/**/
		std::thread th([&]() {
			for (int i = 0; i < g_asteroids_num; i++)
			{
				
				std::unique_lock<std::mutex> lg(bullet_mutex);
				
				switch (rand() % 2)
				{
					case 0:
					{
						asteroid_field.InsertAsteroid(builder.BuildBigAsteroid());
					}break;
					case 1:
					{
						asteroid_field.InsertAsteroid(builder.BuildSmallAsteroid());
					}break;

				}
				lg.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			});
		bullet_thread.join();
		th.join();
		return true;
	}

	virtual void Close() {

	}

	virtual bool Tick()//це функція яка відображає головний ігровий цикл 
	{
		background.DrawBackground();

		


		asteroid_field.DrawAsteroidField();
		asteroid_field.MoveAsteroidField();
		asteroid_field.CheckForCollision();
		if (asteroid_field.GetAsteroids()->size() < 3)
		{
			

			AsteroidBuilder ab(g_asteroid_speed);
			Point p(50, 50);
			asteroid_field.GetAsteroids()->push_back(ab.BuildSmallAsteroidWithStart(p.RandomPointAroundTheScreen(50, 50)));

			for (std::list<Asteroid*>::iterator it = asteroid_field.GetAsteroids()->begin(); it != asteroid_field.GetAsteroids()->end(); )
			{
				bool flag = false;

				if ((*it)->GetCurrent().GetX() > 2000 || (*it)->GetCurrent().GetX() < -2000 || (*it)->GetCurrent().GetY() < -2000 || (*it)->GetCurrent().GetY() > 2000)
				{
					flag = true;
					
				}

				if (flag == true)
				{
					std::list<Asteroid*>::iterator prev = it;
					it++;
					asteroid_field.GetAsteroids()->remove(*prev);
				}
				else
				{
					it++;
				}
			}
		}

		

		//Player
		player.DrawPlayer();
		player.CheckForCollision(asteroid_field);
		//aim movement
		aim.DrawSprite();
		//bullet
		bullet_magazine.MoveMagazine();
		bullet_magazine.CheckForCollision(asteroid_field);
		bullet_magazine.DrawMagazine();

		//;inertia
		
		if (inertia_l)
		{
			onKeyPressed(key_l);
		}
		else
		{
			onKeyReleased(key_l);
		}
		
		if (inertia_r)
		{
			onKeyPressed(key_r);
		}
		else
		{
			onKeyReleased(key_r);
		}

		if (inertia_u)
		{
			onKeyPressed(key_u);
		}
		else
		{
			onKeyReleased(key_u);
		}

		if (inertia_d)
		{
			onKeyPressed(key_d);
		}
		else
		{
			onKeyReleased(key_d);
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)//намалювати ціль і саме по цбому буде вилітати пуля
	{//х і у це координати мишки в вікні, хрел і урел це координати відносно щойно зрушеної точки
		//std::cout << x << " " << y <<std::endl;
		aim.MoveAim(Point(x, y));
	}

	//інертіа
	static float power_l;//крок для гравця
	static float power_r;
	static float power_d;//крок для гравця
	static float power_u;

	static bool inertia_l;//чи нажато шось чи вже отпущено
	static bool inertia_r;//чи нажато шось чи вже отпущено
	static bool inertia_d;//чи нажато шось чи вже отпущено
	static bool inertia_u;//чи нажато шось чи вже отпущено

	static FRKey key_l;
	static FRKey key_r;
	static FRKey key_u;
	static FRKey key_d;
	static FRKey* key;

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) 
	{
		if (button == FRMouseButton::LEFT && isReleased)
		{//треба відпрацювати
			
			if (bullet != bullet_magazine.GetMagazine()->end())
			{
				bullet_magazine.FireFirstBullet(*bullet, player.GetCurrent(), aim.GetCurrent());
				bullet++;
			}
			else if(bullet_magazine.GetMagazine()->size() == 0)
			{
				BulletBuilder bulletbuilder(g_bullet_speed);
				bullet_magazine.InsertBullet(bulletbuilder.BuildBullet(player.GetCurrent(), aim.GetCurrent()));
				bullet_magazine.FireFirstBullet(*bullet_magazine.GetMagazine()->begin(), player.GetCurrent(), aim.GetCurrent());
			}
		}
	}

	virtual void onKeyPressed(FRKey k) 
	{
		
		switch (k)
			{
			case FRKey::LEFT:
			{
				key_l = k;
				inertia_l = true;
				player.MovePlayer(Point(-power_l, 0));
				//key_l = k;
				power_l += g_space_ship_speed_increase;
				
			}break;
			case FRKey::RIGHT:
			{
				key_r = k;
				inertia_r = true;
				player.MovePlayer(Point(power_r, 0));
				//key_r = k;
				power_r += g_space_ship_speed_increase;
			}break;

			case FRKey::DOWN:
			{
				key_d = k;
				inertia_d = true;
				player.MovePlayer(Point(0, power_d));
				//key_d = k;
				power_d += g_space_ship_speed_increase;
			}break;

			case FRKey::UP:
			{
				key_u = k;
				inertia_u = true;
				player.MovePlayer(Point(0, -power_u));
				//key_u = k;
				power_u += g_space_ship_speed_increase;
			}break;
			}
		
	}

	virtual void onKeyReleased(FRKey k) 
	{
		switch (k)
		{
		case FRKey::LEFT:
		{
			inertia_l = false;
			if (power_l > 0) 
			{
				player.MovePlayer(Point(-power_l, 0));
				power_l -= g_space_ship_speed_decrease;
			}

		}break;

		case FRKey::RIGHT:
		{
			inertia_r = false;
			if (power_r > 0)
			{
				player.MovePlayer(Point(power_r, 0));
				//key_r = k;
				power_r -= g_space_ship_speed_decrease;
			}
		}break;

		case FRKey::DOWN:
		{
			inertia_d = false;
			if (power_d > 0)
			{
				player.MovePlayer(Point(0, power_d));
				power_d -= g_space_ship_speed_decrease;
			}
		}break;

		case FRKey::UP:
		{
			inertia_u = false;
			if (power_u > 0)
			{
				player.MovePlayer(Point(0, -power_u));
				power_u -= g_space_ship_speed_decrease;
			}
		}break;
		}
	}
	
	virtual const char* GetTitle() override
	{
		return "Asteroids";
	}
};

//Для намалювання астероїдів найлогічніше буде використати паттерн легковаговик
//оскільки ми маєм ситуацію коли в нас одинакові обэкти,в якиї спрайт однаковий а 
//розташування різне
//
//Також потрібно використати паттерн компоновщик оскільки ми будем мати
//ситуацію астероїд і два підастероїди
//в них повинна бути операція пересування по мапі
//операція знищення кожного, 
//
//Шо стосується гравця то це паттерн синглтон оскільки гравець один
//
//
//також подумати про паттерн команда бо в нас буде команда стриляти по відношенню до астероїда а стриляти буте гравець(менеджер в команді)
//
//
//для підрахунка збитих комет можна буде використати паттерн обзервер
//при кожному збитому астероїді буде надсилатись сповыдення счетчику

std::map<std::string, BulletType*> BulletFactory::bulletype_map;
std::map<std::string, AsteroidType*> AsteroidTypeFactory::asteroidtype_map;//
std::string AsteroidType::picture_folder = "data\\";
//Sprite* BackGround::sprite;
float MyFramework::power_l = 0;//крок для гравця
float MyFramework::power_r = 0;
float MyFramework::power_d = 0;//крок для гравця
float MyFramework::power_u = 0;

bool MyFramework::inertia_l = false;//чи нажато шось чи вже отпущено
bool MyFramework::inertia_r = false; 
bool MyFramework::inertia_u = false; 
bool MyFramework::inertia_d = false;

FRKey MyFramework::key_l = FRKey::COUNT;
FRKey MyFramework::key_r= FRKey::COUNT;
FRKey MyFramework::key_u = FRKey::COUNT;
FRKey MyFramework::key_d= FRKey::COUNT;
FRKey* MyFramework::key = nullptr;

void GetFromCommandLine(int argc, char* argv[], std::vector<float>& params)
{
	
	for (int i = 2; i < argc; i++)//from first
	{
		std::string str = argv[i];
		if (i == 2) {//width of screen
			std::string s_width = str.substr(str.find_first_of("0,1,2,3,4,5,6,7,8,9"), str.find_first_of('x'));
			
			int width_screen  = stoi(s_width);
			params.push_back(width_screen);

			std::string s_height = str.substr(str.find_first_of('x')+1);
			int heigth_screen = stoi(s_height);

			params.push_back(heigth_screen);
		}

		if (i == 4)//size of map
		{
			std::string s_width = str.substr(str.find_first_of("0,1,2,3,4,5,6,7,8,9"), str.find_first_of('x'));
			int width_screen = stoi(s_width);
			params.push_back(width_screen);
			std::string s_height = str.substr(str.find_first_of('x') + 1);
			int heigth_screen = stoi(s_height);
			params.push_back(heigth_screen);
		}

		if (i == 6)
		{
			std::string s_width = str.substr(str.find_first_of("0,1,2,3,4,5,6,7,8,9"));
			
			int number_of_asteroids = stoi(s_width);
			params.push_back(number_of_asteroids);
		}

		if (i == 8)
		{
			std::string s_width = str.substr(str.find_first_of("0,1,2,3,4,5,6,7,8,9"));

			int number_of_bullets = stoi(s_width);
			params.push_back(number_of_bullets);
		}

		if (i == 10)
		{
			std::string s_width = str.substr(str.find_first_of("0,1,2,3,4,5,6,7,8,9"));

			float probability = stof(s_width);
			params.push_back(probability);
		}
	}
}

void SetMainGameProperties(std::vector<float>& params)
{
	for (int i = 0; i < params.size(); i++)
	{
		if (params[0] != 0)//width
		{
			g_width = params[0];
		}
		else
		{
			g_width = 800;
		}

		if (params[1] != 0)//height
		{
			g_height = params[1];
		}
		else
		{
			g_height = 600;
		}

		if (params[2] != 0)//map width
		{
			g_map_width = params[2];
		}
		else
		{
			g_map_width = 600;
		}

		if (params[3] != 0)//map height
		{
			g_map_height = params[3];
		}
		else
		{
			g_map_height = 600;
		}

		if (params[4] != 0)
		{
			g_asteroids_num = params[4];
		}
		else
		{
			g_asteroids_num = 6;
		}

		if (params[5] != 0)
		{
			g_bullets_num = params[5];
		}
		else
		{
			g_bullets_num = 6;
		}

		if (params[6] != 0)
		{
			g_probability_num = params[6];
		}
		else
		{
			g_probability_num = 0.3;
		}

	}
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	std::vector<float> params(7,0);
	GetFromCommandLine(argc, argv, params);
	SetMainGameProperties(params);

	std::mutex mtx;
	mtx.lock();
	run(new MyFramework);
	mtx.unlock();
	return EXIT_SUCCESS;
}
