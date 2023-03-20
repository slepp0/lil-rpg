#pragma once
#include"window.h"
#include<map>
namespace cam
{
	namespace settings
	{
		namespace limits
		{
			float zoomOut = 0;
			float zoomIn = 0;
		}
		namespace zoomSpeed
		{
			const float zoomOut = 1.1f;
			const float zoomIn = 0.9f;
		}
	}
	sf::View background(sf::FloatRect(0, 0, window::size.x, window::size.y));
	sf::View map(sf::FloatRect(0, 0, window::size.x, window::size.y));
	sf::View player(sf::FloatRect(0, 0, window::size.x, window::size.y));
	sf::View objectStack(sf::FloatRect(sf::Vector2f(0, 0), window::size));
	std::map<std::string, sf::View*> camMap;

	void loadCamMap()
	{
		background.zoom(0.5);
		camMap["background"] = &background;
		camMap["map"] = &map;
		camMap["player"] = &player;
	}
	void zoomCams(float factor)
	{
		using namespace settings;
		float buffer = factor;
		for (auto itr = camMap.begin(); itr != camMap.end(); itr++)
		{
			float factor = buffer;
			if (itr->first != "background")
			itr->second->zoom(factor);
		}
	}
}