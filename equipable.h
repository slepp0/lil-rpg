#pragma once
#include<SFML/Graphics.hpp>
#include<vector>
#include<sstream>
#include"art.h"
#include"map.h"
#include"basic.h"
#include"animation.h"
#include"uiLogic.h"

class Weapon;
namespace objectSystem
{
	std::map<std::pair<int, int>, std::vector<Weapon>> WeaponStack;
}
enum class WeaponClass
{
	sword,
	staff
};
struct applyClass
{
	sf::Vector2u exception;
	sf::Vector2u spriteSheetSize;
	std::string key;
	const WeaponClass CLASS;
	applyClass() :CLASS(WeaponClass(rand()%2))
	{ 
		sf::Vector2u t;
		if (CLASS == WeaponClass::staff)
		{
			t.x = 6;
			t.y = 7;
			key = "staves-16x16";
		}
		else 
		{
			t.x = 6;
			t.y = 5;
			key = "swords-16x16";
			exception.x = 6;
			exception.y = 5;
		}

		spriteSheetSize = randomizeVector(t, exception);
	}
};

class Weapon: applyClass
{
private:
	sf::IntRect rectView;
	bool inAction = false;
	sf::Vector2f target;
private:
	sf::Vector2f position, bPosition;
	bool equipped = false;
	int ID = 0;
	static int EquippedWeaponID;
	static int amount;
	sf::RectangleShape solid;
	int attack = randomize(4, 999);
	bool stacked = false;
private:
	sf::Vector2f radians;
	float angle = 45, backupAngle = angle;
	sf::Clock animationTime;
	void WeaponEvent(sf::RenderWindow* app)
	{
		if (click()&&!inAction)
		{
			backupAngle = angle;
			inAction = true;
			solid.rotate(-90);
			position = bPosition;
			target = -(window::center - static_cast<sf::Vector2f>(sf::Mouse::getPosition(*app)));
			deClick();
		}

		if (inAction)
		{
			if (animationTime.getElapsedTime().asMilliseconds() > 180)
			{
				inAction = false;
			}
		}
		else
		{
			animationTime.restart();
			angle = backupAngle;
		}
	}
public:
	Weapon() :ID(amount), solid(tile::size)
	{
		print("new weapon summoned with the class " << int(CLASS) << ", " << key <<", " << spriteSheetSize.x << "-" << spriteSheetSize.y);
		solid.setTexture(&art::textureMap[key]);

		rectView = animation::setSpriteIndex(sf::Vector2u(randomize(1,spriteSheetSize.x), randomize(1, spriteSheetSize.y)), 16, &solid);
		
		int x = randomize(0, maps::mapX), y = randomize(0, maps::mapY);
		while (maps::spriteMap[std::make_pair(x, y)] != '0')
		{
			x = randomize(0, maps::mapX);
			y = randomize(0, maps::mapY);
		}
		using namespace tile;
		position.x = x * tile::tileSize;
		position.y = y * tile::tileSize;

		solid.setPosition(position);

		amount++;
	}

	static void summonWeapon()
	{
		Weapons.push_back(Weapon());
	}

	void Equip()
	{
		Drop();
		stacked = false;
		equipped = true;
		EquippedWeaponID = ID;
	}

	void Calculate(sf::RenderWindow* app)
	{
		if (!uiSettings::isDisplaying(-1))
		{
			sf::Vector2i mPos(sf::Mouse::getPosition(*app));
			solid.setOrigin(50, 50);
			using namespace window;
			angle = atan2(-(mPos.x - center.x), mPos.y - center.y);
			angle *= (180 / PI);

			solid.setRotation(angle + 180 + 45);
		}
	}

	static void Equip(sf::Vector2f index)
	{
		for (auto itr = Weapon::Weapons.begin(); itr != Weapon::Weapons.end(); itr++)
		{
			if (sf::Vector2f(int(index.x), int(index.y)) == positionToIndex(itr->solid.getPosition()))
			{
				Drop();
				itr->stacked = false;
				itr->equipped = true;
				EquippedWeaponID = itr->ID;
				break;
			}
		}
	}
	static void Drop()
	{
		if (EquippedWeaponID != -1)
		{
			Weapon* ptr = &Weapons[EquippedWeaponID];
			ptr->solid.setOrigin(0, 0);
			ptr->equipped = false;
			using namespace cam;
			ptr->solid.setRotation(0);
			ptr->position = indexToPosition(float(int(positionToIndex(map.getCenter()).x)), float(int(positionToIndex(map.getCenter()).y)));
			using namespace objectSystem;
		}
		EquippedWeaponID = -1;
	}
	static std::vector<Weapon> Weapons;
	static void uiObjectStack(sf::RenderWindow* app, sf::Vector2f index)
	{
		int nextBound = 0;
		using namespace cam;
		sf::Vector2f INDEXX = indexToPosition(float(int(positionToIndex(map.getCenter()).x)), float(int(positionToIndex(map.getCenter()).y)));
		
		sf::Vector2f loc = VectorIntToFloat(sf::Mouse::getPosition(*app));
		
		for (auto s = Weapon::Weapons.begin(); s != Weapon::Weapons.end(); s++)
		{
			std::stringstream info;
			info << "ATK " << s->attack;
			sf::Text specs(info.str().c_str(), dragon);
			specs.setFillColor(sf::Color::Red);
			specs.setStyle(sf::Text::Italic);
			
			if (s->position == INDEXX)
			{
				if (s->stacked)
				{
					specs.setOrigin(specs.getLocalBounds().width - 6, -6);
					sf::RectangleShape bound(sf::Vector2f(specs.getOrigin().x + 100, 100));
					bound.setFillColor(sf::Color(0, 0, 0, 127));
					bound.setOutlineThickness(-6);
					bound.setOrigin(specs.getOrigin().x + 100 + 6, 0);

					sf::RectangleShape icon(sf::Vector2f(80, 80));
					icon.setOrigin(90, -6);
					icon.setTexture(s->solid.getTexture());
					icon.setTextureRect(s->rectView);

					sf::Vector2f pos(window::size.x, nextBound * 100);
					bound.setPosition(pos);
					icon.setPosition(pos);
					specs.setPosition(window::size.x - 100, pos.y);
				
					lockMouseScroll = 0;
					if (window::size.y < pos.y+100)
					{
						lockMouseScroll = 1;
					}
				
					if (bound.getGlobalBounds().intersects(sf::FloatRect(loc , sf::Vector2f(1, 1))))
					{
						bound.setOutlineColor(sf::Color(255, 255, 255, 127));
						bound.setFillColor(sf::Color(0, 0, 0, 200));
						if (click())
						{
							s->Equip();
							deClick();
						}
					}
					app->setView(cam::objectStack);
					app->draw(bound);
					app->draw(icon);
					app->draw(specs);

					nextBound++;
				}
				else
				{
					using namespace tile;
					sf::RectangleShape bound(sf::Vector2(specs.getLocalBounds().width+6, specs.getLocalBounds().height+6));
					sf::Vector2f pos(s->position.x, s->position.y - tileSize);
					bound.setFillColor(sf::Color::Black);
					bound.setOutlineThickness(6);
					bound.setPosition(pos);
					specs.setOrigin(-2, 2);
					specs.setPosition(pos);
					app->setView(cam::map);
					app->draw(bound);
					app->draw(specs);

					break;
				}
			}
		}
	}
	void Update(sf::RenderWindow* app)
	{
		if (equipped)
		{
			position = sf::Vector2f(cam::map.getCenter());
			if (!uiSettings::isDisplaying(-1))
			{
				if (CLASS == WeaponClass::sword)
				{
					WeaponEvent(app);
				}
			}

			if (!inAction)
			{
				solid.setPosition(position);
				Calculate(app);
			}
			else
			{
				solid.setOrigin(solid.getSize()-sf::Vector2f(20,20));
				solid.rotate(1);
			}
		}
		if(!equipped)
		{
			stacked = false;
			for (auto i = Weapons.begin(); i != Weapons.end(); i++)
			{
				if (i->ID != this->ID)
				{
					if (position == i->position)
					{
						stacked = true;
						i->stacked = true;
					}
				}
			}
		}
		solid.setPosition(position);
		app->draw(solid);
	}
	static void UpdateEquippedWeapon(sf::RenderWindow* app)
	{
		if (EquippedWeaponID != -1)
		{
			Weapons[EquippedWeaponID].Update(app);
		}
	}
};
int Weapon::amount = 0, Weapon::EquippedWeaponID = -1;
std::vector<Weapon> Weapon::Weapons;
