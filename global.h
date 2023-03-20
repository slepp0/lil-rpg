#pragma once
#include"art.h"
#include"window.h"
#include"player.h"
#include"cam.h"
#include"map.h"
#include"multiplayer.h"
#include"ui.h"
#include"basic.h"
#include"equipable.h"
#include"uiLogic.h"
#include"enemy.h"
void loadGameResources()
{
	uiSettings::initMenu();
	
	loadBasic();
	multiplayer::thread::commandThread.detach();
	multiplayer::thread::update.detach();
	art::loadTextureMap();
	cam::loadCamMap();
	maps::loadMaps();
}

void renderGame(Player* you, sf::RenderWindow* app)
{
	app->setView(cam::background);
	app->draw(maps::background);

	app->setView(cam::map);
	maps::UpdateMainMap(you->indexPosition, app);
	for (auto itr = Weapon::Weapons.begin(); itr != Weapon::Weapons.end(); itr++)
	{
		itr->Update(app);
	}

	if (multiplayer::isHost())
	{
		for (auto itr = multiplayer::playerList.begin(); itr != multiplayer::playerList.end(); itr++)
		{
			itr->first.Update(app);
		}
	}

	else if (multiplayer::isClient())
	{
		for (auto itr = multiplayer::dummies.begin(); itr != multiplayer::dummies.end(); itr++)
		{
			itr->second.Update(app);
		}
	}
	Enemy::Update(app);


	//-------------------------------------------------------------------------------------------------------
	app->setView(cam::player);
	you->Update(app);

	app->setView(cam::map);
	Weapon::UpdateEquippedWeapon(app);
	maps::UpdateForeMap(app);

	Weapon::uiObjectStack(app, you->indexPosition);
	app->setView(app->getDefaultView());
	Button::draw(app);
}