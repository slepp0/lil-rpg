#pragma once
#include<array>
#include<algorithm>
namespace uiSettings
{
	//0 main menu
	//1 multiplayer
	std::array<bool, 2> menus;
	void initMenu()
	{
		std::fill(menus.begin(), menus.end(), false);
	}
	bool isDisplaying(const int menuIndex)
	{
		if (menuIndex == -1)
			for (int i = 0; i < menus.size(); i++)
			{
				if (menus[i])
					return true;
			}
		else
			return menus[menuIndex];
	}
	void display(const Int8 menuIndex, bool open = true)
	{
		menus[menuIndex] = open;
		if (open)
			for (int i = 0; i < menus.size(); i++)
			{
				if (i != menuIndex)
					menus[i] = false;
			}
	}
	
}