#pragma once
#include<SFML/Graphics.hpp>
#include<filesystem>
#include<string>
#include<map>
#include<vector>
#include<iostream>
#define addTexture(name) textureNames.push_back(name)
using namespace sf;
sf::IntRect viewTile() 
{
	sf::IntRect viewArea;

	return viewArea;
}

namespace art
{
	std::vector<std::string> textureNames;
	std::map<std::string, sf::Texture> textureMap;

	void loadTextureName()
	{
		addTexture("ghost.png");
		addTexture("ground.png");
		addTexture("space.png");
	
		addTexture("staves-16x16.png");
		addTexture("swords-16x16.png");
	}
	
	void loadTextureMap()
	{
		using namespace std::filesystem;
		loadTextureName();
		for (auto& itr : std::filesystem::recursive_directory_iterator("art"))
		{
			for (auto& name : textureNames)
			{
				path file(itr.path().string() + '\\' + name);
				if (exists(file) && !is_directory(file))
				{
					std::string id;
					id = name.substr(0, name.find_first_of('.'));
					if (textureMap[id].loadFromFile(file.string()))
					{
						std::cout << '[' << id << ']' << ", path=" << file.string() << std::endl;
					}
				}
			}
		}
	}
}