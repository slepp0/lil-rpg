#pragma once
#include<vector>
#include<SFML/Graphics.hpp>
#include"art.h"
#include"animation.h"
#include"map.h"
class Enemy: animation
{
	sf::Texture sprite;
	sf::RectangleShape solid;
	Enemy():solid(Vector2f(100,100)), animation(Vector2u(12,8), Vector2u(7,5), "ghost", &solid)
	{
		solid.setPosition(maps::spawnOver('0'));
	}
	void update(sf::RenderWindow *app)
	{
		app->draw(solid);
	}
	static std::vector<Enemy> enemies;
public:
	static void spawn()
	{
		enemies.push_back(Enemy());
	}
	static void Update(sf::RenderWindow* app)
	{
		for (auto itr = enemies.begin(); itr != enemies.end(); itr++)
		{
			itr->update(app);
		}
	}
};
std::vector<Enemy> Enemy::enemies;