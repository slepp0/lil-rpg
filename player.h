#pragma once
#include<SFML/Graphics.hpp>
#include<array>
#include"animation.h"
#include"art.h"
#include"window.h"
#include"basic.h"
#include"cam.h"
#include"equipable.h"

namespace defaults
{
	float delayOnlineUpdate = 600;
	sf::Vector2f playerSize(tile::tileSize, tile::tileSize);
}
class Player :public animation
{
private:
	float defaultSpeed = 1.f, springSpeed = 2.f, currentSpeed = defaultSpeed, parallax = 0.06;
	bool dummy;
	int ID = 0;
	int EquippedWeaponID = NULL;
private:
	sf::Clock delay;
public:
	bool inMovement = false;
	sf::Vector2f position, indexPosition;
public:
	sf::Text nickText;
	std::string nick;
	bool connected = false;
	int clientID = 0;
	static int amount;
	sf::RectangleShape playerRect;
	Player(std::string nick, sf::Vector2u spriteSheetRange, sf::Vector2u spriteSheetOrigin, std::string spriteName, sf::Vector2f spawnIndex, bool dummy = true) :dummy(dummy), 
		animation( spriteSheetRange, spriteSheetOrigin, spriteName, &playerRect)
	{
		this->nick = nick;
		nickText.setFont(dragon);
		nickText.setCharacterSize(30);
		nickText.setScale(0.5, 0.5);

		ID = amount;
		amount++;
		//std::cout << "new player criated with id: " << ID << std::endl;

		if (!dummy)
		{
			playerRect.setPosition(window::size.x / 2, window::size.y / 2);
			cam::map.setCenter(indexToPosition(float(spawnIndex.x + 0.5), float(spawnIndex.y + 0.5)));//===-----
		}
		else
			playerRect.setPosition(spawnIndex.x, spawnIndex.y);

		this->playerRect.setOrigin(defaults::playerSize.x / 2, defaults::playerSize.y / 2);
		this->playerRect.setTexture(&art::textureMap[spriteName]);
		
		this->playerRect.setSize(defaults::playerSize);
		
		animation::setSpriteIndex(originAnimation);
	}
	~Player()
	{
		//2print("Player destroyer called! " << ID);
	}
	void goForward()
	{
		playerRect.move(0.f, -currentSpeed);
	}
	void goBack()
	{
		playerRect.move(0.f, currentSpeed);
	}
	void goLeft()
	{
		playerRect.move(-currentSpeed, 0.f);
	}
	void goRight()
	{
		playerRect.move(currentSpeed, 0.f);
	}
	bool isMoving()
	{
		using namespace sf;
		if (getKey(Keyboard::W) || getKey(Keyboard::A) || getKey(Keyboard::S) || getKey(Keyboard::D))
		{
			return true;
		}
		else
			return false;
	}
	void move(sf::RenderWindow* app);

	void Update(sf::RenderWindow* app);
};

int Player::amount = 0;

#include"multiplayer.h"
void Player::move(sf::RenderWindow* app)
{
	using namespace multiplayer;
	if ((!dummy) ? app->hasFocus() && !uiSettings::isDisplaying(-1) : true)
	{
		using namespace sf;
		using namespace defaults;
		if ((!dummy) && getKey(Keyboard::LShift))
		{
			switchTime = 60;
			currentSpeed = springSpeed;
		}
		else
		{
			switchTime = 150;
			currentSpeed = defaultSpeed;
		}

		using namespace maps;
		using namespace cam;
		if ((!dummy) && getKey(Keyboard::W))
		{
			animate(4, 1, 3);
			if (!isCollided(indexPosition, 0, -0.5))
			{
				cam::background.move(0.f, -(parallax * currentSpeed));
				map.move(0.f, -currentSpeed);
			}
		}
		else if (!dummy && getKey(Keyboard::S))
		{
			animate(1, 1, 3);
			if (!isCollided(indexPosition, 0, 0.5))
			{
				cam::background.move(0.f, parallax * currentSpeed);
				map.move(0.f, currentSpeed);
			}

		}

		if (!dummy && getKey(Keyboard::A))
		{
			animate(2, 1, 3);
			if (!isCollided(indexPosition, -0.5, 0))
			{
				cam::background.move(-(parallax * currentSpeed), 0.f);
				map.move(-currentSpeed, 0.f);
			}

		}
		else if (!dummy && (getKey(Keyboard::D)))
		{
			animate(3, 1, 3);
			if (!isCollided(indexPosition, 0.5f, 0))
			{
				cam::background.move(parallax * currentSpeed, 0.f);
				map.move(currentSpeed, 0.f);
			}
		}

		position = map.getCenter();
		indexPosition = positionToIndex(sf::Vector2f(int(position.x + 0.5), int(position.y + 0.5)));
		using namespace multiplayer;
		if (!dummy)
		{
			float x = position.x, y = position.y;
			if (isMoving())
			{
				if (connected)
				{
					if (delay.getElapsedTime().asMilliseconds() > delayOnlineUpdate)
					{
						if (isHost())
						{
							for (std::vector<std::pair<Player, sf::TcpSocket*>>::iterator itr = playerList.begin(); itr != playerList.end(); itr++)
							{
								sendData<int>(int(signal::UPDATE_POSITION), *itr->second);//MRC
								sendData<int>(100, *itr->second);
								sendVector(position, *itr->second);
							}
						}
						else if (isClient())
						{
							sendData<int>(int(signal::UPDATE_POSITION), socket); // MR
							sendVector(position, socket);
						}
					}
				}
			}
		}
	}
	if (connected && !dummy)
	{
		if (isClient())
		{
			app->setTitle("Autumn Ghosts - client");
		}
		else if (isHost())
		{
			app->setTitle("Autumn Ghosts - server");
		}
	}
}

void Player::Update(sf::RenderWindow* app)
{
	move(app);
	app->draw(playerRect);

	nickText.setString(nick);
	nickText.setOrigin(nickText.getLocalBounds().width / 2, nickText.getLocalBounds().height);
	nickText.setPosition(int(playerRect.getPosition().x), int((playerRect.getPosition().y - 6) - (defaults::playerSize.y / 2)));

	app->draw(nickText);
}