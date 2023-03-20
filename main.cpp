#include<SFML/Graphics.hpp>
#include<iostream>
#include"global.h"
void close(sf::RenderWindow* app)
{
	{
		using namespace multiplayer;
		socket.disconnect();
		client::LANrequest.unbind();
		server::broadcast.unbind();
		shutdown = true;
	}
	while (!finishedThread) {}
	app->close();
}

int main()
{
	loadGameResources();
	
	//-------------------------------------------------
	sf::RenderWindow app(sf::VideoMode(window::defaults::windowMode), "Autumn Ghosts", sf::Style::Close); sf::Event eve;
	app.setKeyRepeatEnabled(false);

	Player you("", sf::Vector2u(12, 8), sf::Vector2u(7, 1), "ghost", maps::mapCenter, false);
	multiplayer::setYouPointer(&you);
	sf::Clock clock;
	while (app.isOpen()&&!closeGame)
	{
		while (app.pollEvent(eve))
		{
			switch (eve.type)
			{
			case sf::Event::Closed:
				close(&app);
				break;
			case sf::Event::MouseWheelMoved:
			{
				using namespace cam::settings::limits;
				using namespace cam::settings;
				switch (lockMouseScroll)
				{
				case 0:
					if (eve.mouseWheel.delta > 0)
					{
						if (zoomIn < 6)
						{
							zoomOut--;
							zoomIn++;
							cam::zoomCams(zoomSpeed::zoomIn);
						}
					}
					else
					{
						if (zoomOut < 24)
						{
							zoomIn--;
							zoomOut++;
							cam::zoomCams(zoomSpeed::zoomOut);
						}
					}
					break;
				case 1:
					if (eve.mouseWheel.delta > 0)
					{
						cam::objectStack.move(0, -100);
					}
					else
					{
						cam::objectStack.move(0, 100);
					}
				}
				break;
			}
			case sf::Event::KeyPressed:
				if (getKey(sf::Keyboard::Escape))
				{
					using namespace uiSettings;
					if (!isDisplaying(0))
						display(0);
					else
						display(0, false);
				}
				else if (getKey(sf::Keyboard::F11))
				{
					using namespace window;
					sf::VideoMode currentVideoMode; 
					sf::Uint32 windowStyle;
					if (currentStyle == style::WINDOW)
					{
						currentVideoMode = VideoMode::getDesktopMode();
						windowStyle = Style::Fullscreen;
						currentStyle = style::FULLSCREEN;
					}
					else
					{
						windowStyle = sf::Style::Close;
						currentVideoMode = window::defaults::windowMode;
						currentStyle = style::WINDOW;
					}
					app.create(currentVideoMode, "Autumn Ghost", windowStyle);
					app.setKeyRepeatEnabled(false);
					Update(app);
						cam::objectStack.reset(FloatRect(sf::Vector2f(0,0), static_cast<Vector2f>(app.getSize())));
				}

				if (getKey(Keyboard::Q))
				{
					Weapon::Drop();
				}
				else if (getKey(Keyboard::E))
				{
					Weapon::Equip(you.indexPosition);
				}
				else if (getKey(Keyboard::Space))
				{
					Weapon::summonWeapon();
					Enemy::spawn();
				}
				break;
			case sf::Event::MouseButtonPressed:
			
				clicked = true;
				break;
			case sf::Event::MouseButtonReleased:
				clicked = false;
				break;
			}
		}
		app.clear();
		renderGame(&you, &app);
		///////////////////////////////
		app.setView(app.getDefaultView());

		
		app.display();
		sf::sleep(sf::milliseconds(1));
	}
	close(&app);
	return 0;
}