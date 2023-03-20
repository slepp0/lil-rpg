#pragma once
#include<SFML/Graphics.hpp>
namespace window
{
	namespace defaults
	{
		sf::VideoMode windowMode(sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height / 2);
	}
	enum class style
	{
		WINDOW,
		FULLSCREEN
	};
	sf::Vector2f size(defaults::windowMode.width, defaults::windowMode.height);
	sf::Vector2f center(size.x / 2, size.y / 2);
	const sf::Vector2f ccenter(center);
	void Update(sf::RenderWindow& app)
	{
		sf::Vector2u newSize = app.getSize();
		using namespace defaults;
		size.x = newSize.x;
		size.y = newSize.y;
		center.x = size.x / 2;
		center.y = size.y / 2;
	}
	style currentStyle = style::WINDOW;
}