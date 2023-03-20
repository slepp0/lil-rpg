#pragma once
#include<SFML/Window/Keyboard.hpp>
#include<vector>
#include<mutex>
#include<random>
#include<time.h>
#include<iostream>
#define print(x) std::cout << x << std::endl
const float PI = 3.14159;
bool finishedThread = false;
std::mutex threadMutex;
sf::Font dragon;
int lockMouseScroll = 0;
void loadBasic()
{
	srand(time(NULL));
	dragon.loadFromFile("dragon.ttf");
}
bool clicked = false;
bool click()
{
	return clicked;
}
void deClick()
{
	clicked = false;
}
int randomize(int from, int to)
{
	return rand()%(to+1) + from;
}

bool closeGame = false;
namespace tile
{
	const float tileSize = 100;
	sf::Vector2f size(tileSize, tileSize);
}
sf::Vector2f indexToPosition(float x, float y)
{
	using namespace tile;
	return sf::Vector2f(x * size.x, y * size.y);
}

sf::Vector2i indexToPosition(int x, int y)
{
	using namespace tile;
	return sf::Vector2i(x * size.x, y * size.y);
}

sf::Vector2f positionToIndex(sf::Vector2f position)
{
	using namespace tile;
	return sf::Vector2f(position.x / size.x, position.y / size.y);
}

sf::Vector2i positionToIndex(sf::Vector2i position)
{
	using namespace tile;
	return sf::Vector2i(position.x / size.x, position.y / size.y);
}

sf::Vector2f VectorIntToFloat(sf::Vector2i convertVector)
{
	return sf::Vector2f(float(convertVector.x), float(convertVector.y));
}

sf::Vector2u randomizeVector(sf::Vector2u vector, sf::Vector2u exception = sf::Vector2u(-1,-1))
{
	int x = 0, y = 0;
	do
	{
		int x = randomize(1, vector.x), y = randomize(1, vector.y);
		sf::Vector2u temp(x, y);
		if (temp == exception)
		{
			print("exception :P");
			continue;
		}
		else
			return temp;
	} while (true);
}

bool getKey(sf::Keyboard::Key key)
{
	using namespace sf;
	return Keyboard::isKeyPressed(key);
}


typedef int INDEX;
typedef std::vector<char> MAP;