#pragma once
#include<SFML/Graphics.hpp>
#include"uiLogic.h"
#include<map>
#include<vector>
#include"multiplayer.h"

class Button;

std::map<Button*, std::string> buttonMap;

class RoomButton
{
private:
	//count
	const int ID;
	static int amount;
public:
	//graphic
	sf::RectangleShape shape;
public:
	RoomButton():ID(amount), shape(sf::Vector2f(200,100))
	{
		shape.setFillColor(Color::Black);
		amount++;
	}
	void update(sf::FloatRect rect, sf::RenderWindow* app)
	{
		shape.setPosition(rect.left, rect.top-rect.height);
		app->draw(shape);
	}
public:
	static std::vector<std::string> vector;
	static std::vector<RoomButton> rooms;
};
int RoomButton::amount = 0;
std::vector<std::string> RoomButton::vector;
std::vector<RoomButton> RoomButton::rooms;

void uiDisplayHostRoom()
{
	RoomButton::vector = multiplayer::discoveryLAN();
	uiSettings::display(1);
}


class Button
{
public:
	Button(std::string label, void(*execute)()) :UIID(amount), shape(size),label(label, dragon), command(execute)
	{
		shape.setOrigin(size/2.f);

		this->label.setOrigin(size/2.f);
		this->label.setFillColor(sf::Color::Black); 
	
		amount++;

		buttonMap[this] = label;
	}
	void update()
	{
		size = Vector2f(window::size / 6.f);
		shape.setSize(size);
		shape.setOrigin(size / 2.f);
		label.setCharacterSize(shape.getSize().x / 14.4f * 2);
		label.setOrigin(size / 2.f);
	}
	void onHover()
	{
		if (label.getString() == "quit")
			shape.setFillColor(red);
		else
			shape.setFillColor(black);
		label.setFillColor(white);
	}
	void notFocused()
	{
		shape.setFillColor(white);
		label.setFillColor(black);
	}
	static void draw(sf::RenderWindow* app)
	{
		sf::Vector2f loc = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*app));
		if (uiSettings::isDisplaying(0))
			for (auto button = buttonMap.begin(); button != buttonMap.end(); button++)
			{
				auto b = button->first;
				b->update();

				if (b->shape.getGlobalBounds().intersects(sf::FloatRect(loc, sf::Vector2f(1, 1))))
				{
					b->onHover();
					if (click())
					{
						b->command();
						deClick();
					}
				}
				else
					b->notFocused();
				app->draw(b->shape);
				app->draw(b->label);
				b->pos = sf::Vector2f(window::center.x, window::center.y + (b->UIID * size.y));
				b->shape.setPosition(b->pos);
				b->label.setPosition(b->pos);
			}
		else if (uiSettings::isDisplaying(1))
		{

			sf::RectangleShape background(Vector2f(window::size / 2.f));
			background.setPosition(background.getSize() / 2.f);

			sf::RectangleShape b(Vector2f(100,50));
			b.setFillColor(Color::Black);

			sf::FloatRect rect = background.getLocalBounds();
			b.setPosition(background.getSize()/ 2.f);
			app->draw(background);
			
			for (auto itr = RoomButton::vector.begin(); itr < RoomButton::vector.end(); itr++)
			{
				app->draw(b);
			}
		}
	}
	~Button()
	{

	}
private:
	static sf::Color black, white, red;
	void(*command)();
	sf::Text label;
	const int UIID;
	static sf::Vector2f size;
	sf::Vector2f pos;
	static int amount;
	sf::RectangleShape shape;
};

int Button::amount = 0;
sf::Vector2f Button::size(200, 100);
using namespace sf;
sf::Color Button::black(Color::Black), Button::white(Color::White), Button::red(Color::Red);

void quitGame()
{
	closeGame = true;
	print("game closed");
}

Button b_multiplayer("multiplayer", uiDisplayHostRoom), quit("quit", quitGame);