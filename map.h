#pragma once

#include<SFML/Graphics.hpp>
#include"art.h"
#include"basic.h"
#include"window.h"

#define addRect(rectName) sf::RectangleShape rectName(tile::size)
#define addRid(id) rigidId.push_back(id);
#define addFid(id) foreId.push_back(id);
#define addId(idName) id.push_back(idName)
namespace maps
{
	int mapX = 0, mapY = 0;
	sf::Vector2f mapCenter;
	std::map<std::pair<int, int>, char> spriteMap, foreMap;
	sf::RectangleShape background(window::size);
	MAP mainMap;
	namespace IDs
	{
		std::vector<char> id, foreId, rigidId;
	}
	
	addRect(glass);
	addRect(stair);
	sf::RectangleShape dec1(sf::Vector2f(tile::size.x * 3, tile::size.y * 3));

	addRect(dec2);
	addRect(dec2b);
	sf::Vector2f spawnOver(char id)
	{
		int x = 0, y = 0;
		do
		{
			x = randomize(0, mapX);
			y = randomize(0, mapY);
		} while (spriteMap[std::make_pair(x,y)]!=id);
		using namespace tile;
		return Vector2f(x * tileSize, y * tileSize);
	}
	void loadMaps()
	{
		using namespace art;
		background.setTexture(&textureMap["space"]);

		using namespace IDs;
		sf::Texture* gt = &textureMap["ground"];

		glass.setTexture(gt);
		glass.setTextureRect(sf::IntRect(16 * 16, 16 * 1, 16, 16));

		addId('0');

		dec1.setTexture(gt);//font???
		dec1.setTextureRect(sf::IntRect(16 * 16, 16 * 7, 16 * 3, 16 * 3));
		addId('1');

		dec2.setTexture(gt); //pillar
		dec2.setTextureRect(sf::IntRect(16 * 16, 16 * 4, 16, 16));
		addId('2');

		dec2b.setTexture(gt); //pillar's base
		dec2b.setTextureRect(sf::IntRect(16 * 16, 16 * 5, 16, 16));
		addId('b');
		addFid('b');
		addRid('b');

		addRid(' ');

		stair.setTexture(gt);//stairs
		stair.setTextureRect(sf::IntRect(16 * 19, 0, 16, 16));
		addId('3');

		int nextX = 0, nextY = 0;
		int sizeRoom = 4; randomize(4, 8);
		if (sizeRoom % 2 != 0)
			sizeRoom++;
		int bridgeX = true, bridgeY = true;
		const int dungeonSize = 8;
		int link[dungeonSize][dungeonSize];
		
		int nextRX = -1, nextRY = -1;
		bool negativeX = false;
		bool generate = true;
		sf::Vector2i cRoom;
		//---------------------------------------------------------------
		sf::Vector2f currentIndex;
		for (int ry = 1; ry < (dungeonSize) + 1; ry++)
		{
			for (int rx = 1; rx < (dungeonSize) + 1; rx++)
			{
				bridgeX = randomize(0, 1);
				
				bridgeY = randomize(0, 1);
				link[rx][ry] = bridgeX;
				//print info
				print("Y:" << ry << ", X:" << rx << "[" << link[rx][ry] << "]");
				
				if ((ry == 1 && rx == 1) || (rx == nextRX && ry == nextRY) )
				{

					for (int y = nextY; y < nextY + sizeRoom; y++)
					{
						cRoom.x = sizeRoom + nextX - 1;
						cRoom.y = sizeRoom + nextY - 1;
						for (int x = nextX; x < nextX + sizeRoom; x++)
						{
							//---?
							if (bridgeX == 0)
							{
								//bridge X
								if (ry != dungeonSize + 1 && rx != dungeonSize)
								{
									int range = y % sizeRoom;
									for (int b = cRoom.x + 1; b <= cRoom.x + sizeRoom; b++)
										if (range > sizeRoom / 2 / 2 - 1 && range < sizeRoom - sizeRoom / 2 / 2)
											spriteMap[std::make_pair(b, y)] = '0';
										else
											spriteMap[std::make_pair(b, y)] = ' ';
									
									nextRX = rx + 1;
									nextRY = ry;
								}
							}
							else
							{
								//bridge Y	
								if (ry != dungeonSize)
								{
									int range = x % sizeRoom;
									for (int b = cRoom.y + 1; b <= cRoom.y + sizeRoom; b++)
										if (range > sizeRoom / 2 / 2 - 1 && range < sizeRoom - sizeRoom / 2 / 2)
											spriteMap[std::make_pair(x, b)] = '0';
										else
											spriteMap[std::make_pair(x, b)] = ' ';
								}
								nextRY = ry + 1;
								nextRX = rx;
							}

							//---!
							currentIndex.x = x;
							currentIndex.y = y;

							spriteMap[std::make_pair(x, y)] = '0';

							mapCenter.x = x / 2;
							mapCenter.y = y / 2;
							mapX = x;
							mapY = y;
						}
					}
				}
				else
				{
					
				}
				nextX = sizeRoom * (rx * 2);
			}
			nextY = sizeRoom * (ry * 2);
			nextX = 0;
		}

		print("\nmap x = " << mapX << ", y = " << mapY);
	}
	void UpdateMainMap(sf::Vector2f index, sf::RenderWindow* app)
	{
		sf::RectangleShape block;
		block.setSize(tile::size);
		using namespace IDs;
		int x = 0, y = 0;
		sf::Vector2f currentPosition(x, y);

		for (auto& s : spriteMap)
		{		
			
			const int pre = 1, factorX = cam::map.getSize().x / tile::tileSize + pre, factorY = cam::map.getSize().y / tile::tileSize + pre;
			if (s.first.first < index.x + factorX && s.first.first > index.x - factorX && s.first.second < index.y + factorY && s.first.second > index.y - factorY)
			{
				char i = s.second;
				BlendMode blend;
				if (i == '2')
					foreMap[std::make_pair(x, y)] = 'b';
				for (auto& j : id)
					if (i == j)
					{
						if (i == '0')
						{
							block = glass;
							blend = BlendAdd;
						}
						else if (i == '1')
						{
							block = dec1;
						}

						else if (i == 'b')
						{
							block = dec2b;
						}

						else if (i == '3')
							block = stair;
						block.setPosition(s.first.first * tile::size.x, s.first.second * tile::size.x);
						app->draw(block, blend);
					}
			
			}
		}	
	}
	void UpdateForeMap(sf::RenderWindow* app)
	{
		sf::RectangleShape tile(tile::size);
		for (auto& itr : foreMap)
		{
			sf::Vector2f position(sf::Vector2f(itr.first.first, itr.first.second));
			char tileid = itr.second;
			tile = dec2;
			tile.setPosition(position);
			app->draw(tile);
		}
	}
	bool isCollided(sf::Vector2f rect, float predictX, float predictY)
	{
		for (auto& id : IDs::rigidId)
		{
			if (spriteMap[std::make_pair(rect.x + predictX, rect.y + predictY)] == id)
				return true;
		}	
		return false;
	}
}