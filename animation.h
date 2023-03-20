#pragma once
#include<SFML/Graphics.hpp>
#include<vector>
#include<iostream>
#include"basic.h"
#include"art.h"
#define print(x) std::cout << x << std::endl
class animation
{
private:
	sf::Vector2f factor;

	sf::Vector2u size;
	sf::Clock clock;
private:
	static int amount;
	int animationID = 0;
public:
	float switchTime = 150;
	sf::Vector2u indexRange, currentIndex;
	sf::Vector2u originAnimation;
	sf::IntRect rectView;
	static std::vector<sf::RectangleShape*> playerSprites;
	std::vector<sf::RectangleShape*>::iterator ownPtr;
	animation(sf::Vector2u indexRange, sf::Vector2u spriteSheetOrigin, std::string playerTexture, sf::RectangleShape* playerRect) :indexRange(indexRange), originAnimation(spriteSheetOrigin)
	{
		animationID = amount;
		amount++;

		playerSprites.push_back(playerRect);
		ownPtr = playerSprites.end()-1;
		print("new animator created with id=" << animationID << " at address " << playerRect << " and vector address " << playerSprites[animationID]);
		size = art::textureMap[playerTexture].getSize();

		factor.x = (size.x / indexRange.x);
		factor.y = (size.y / indexRange.y);
	}
	~animation()
	{
		
	}
	static sf::IntRect setSpriteIndex(sf::Vector2u index, const int resolution, sf::RectangleShape* RECT)
	{
		sf::IntRect rectView;
		rectView.width = resolution;
		rectView.height = resolution;
		rectView.left = resolution * (index.x - 1);
		rectView.top =  resolution * (index.y - 1);
		RECT->setTextureRect(rectView);
		return rectView;
	}

	void setSpriteIndex(sf::Vector2u index)
	{
		currentIndex = index;
		rectView.width = size.x / indexRange.x;
		rectView.height = size.y / indexRange.y;
		rectView.left = factor.x * (index.x - 1);
		rectView.top = factor.y * (index.y - 1);
		print(*ownPtr);
		(*ownPtr)->setTextureRect(rectView);
	}

	void animate(int y, int xBegin, int xEnd)
	{
		int plusX = (originAnimation.x - 1), plusY = (originAnimation.y - 1);
		xBegin += plusX;
		xEnd += plusX;
		currentIndex.y = (y + plusY);
		if (clock.getElapsedTime().asMilliseconds() > switchTime)
		{
			if (currentIndex.x < xEnd)
			{
				(currentIndex.x++);
			}
			else
			{
				currentIndex.x = xBegin;
			}
			setSpriteIndex(currentIndex);
			clock.restart();
		}
	}
};
int animation::amount = 0;
std::vector<sf::RectangleShape*> animation::playerSprites;