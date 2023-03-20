#pragma once
#include<SFML/Network.hpp>
#include<vector>
#include<map>
#include<thread>
#include<iostream>
#include<string>
#include"basic.h"
#include"cam.h"

//#include"player.h"
class RoomButton;
class Player;

namespace multiplayer
{
	Player* you;
	void setYouPointer(Player* youPtr)
	{
		you = youPtr;
	}
	sf::Clock delay;
	bool shutdown = false;
	enum class signal
	{
		UPDATE_POSITION,
		WEAPON_POSITION,
		///////////
		NEW_PLAYER,
		REQUEST_PLAYER_INFO,
		CHAT
	};
	enum class status
	{
		SERVER,
		CLIENT,
		NO_DEFINED
	};
	status currentStatus;
	bool isClient()
	{
		return currentStatus == status::CLIENT;
	}
	bool isHost()
	{
		return currentStatus == status::SERVER;
	}
	unsigned short port = 636, portr = 646, ports = 656;
	sf::IpAddress broadcastAddress("localhost");
	namespace server
	{
		sf::IpAddress ip(sf::IpAddress::getLocalAddress());
		sf::Clock updateServerDelay;
		sf::UdpSocket broadcast;
		sf::Packet packet;
		sf::TcpListener listener;
		sf::SocketSelector otherPlayersSockets;
	}
	namespace client
	{
		sf::UdpSocket LANrequest;
	}
	std::vector<std::pair<Player, sf::TcpSocket*>> playerList;
	std::map<int, Player> dummies;
	sf::TcpSocket socket;
	
	template<typename dataType> bool sendData(dataType data, sf::TcpSocket& socket)
	{
		sf::Packet p;
		p << data;
		if (socket.send(p) == sf::Socket::Done)
		{
			return true;
		}
		else
			return false;
	}
	template<typename dataType> dataType receiveData(sf::TcpSocket& socket)
	{
		dataType buffer;
		sf::Packet p;
		socket.receive(p);
		p >> buffer;
		return buffer;
	}

	void sendVector(sf::Vector2f v, sf::TcpSocket& socket)
	{
		sf::Packet p;
		p << v.x << v.y;
		socket.send(p);
	}

	sf::Vector2f receiveVector(sf::TcpSocket& socket)
	{
		float x, y;
		sf::Packet p;
		socket.receive(p);
		p >> x >> y;
		return sf::Vector2f(x, y);
	}

	void host()
	{
		printf("nick: ");
		you->nick = "noob";
		//std::getline(std::cin, you->nick);
		using namespace server;
		if (listener.listen(port) == sf::Socket::Done)
		{
			print("[host] waiting for players...");
			otherPlayersSockets.add(listener);
			currentStatus = status::SERVER;
			broadcast.bind(portr);
			otherPlayersSockets.add(broadcast);
			//broadcast.setBlocking(false);
		}
	}

	void join(sf::IpAddress host_ip, std::string nick)
	{
		if (socket.connect(host_ip, port) == sf::Socket::Done)
		{
			sendData<std::string>(nick, socket);

			//META-DATA
			you->clientID = receiveData<int>(socket);
			std::string playerNick = receiveData<std::string>(socket);

			print("ID:" << you->clientID);

			print("loading players...");
			{
				using namespace sf;
				dummies.insert(std::make_pair(100, Player(playerNick, Vector2u(12, 8), Vector2u(1, 1), "ghost", Vector2f(3, 3))));
			}
		
			for (int i = 0; i < you->clientID-1; i++)
			{
				using namespace sf;
				int ID = receiveData<int>(socket);
				playerNick = receiveData<std::string>(socket);
				print("adding dummy #" << ID);
				dummies.insert(std::make_pair(ID, Player(playerNick, Vector2u(12, 8), Vector2u(1, 1), "ghost", Vector2f(3, 3))));
			}

			server::otherPlayersSockets.add(socket);
			currentStatus = status::CLIENT;
			you->connected = true;
			print("loaded!");
		}
	}

	std::vector<std::string> discoveryLAN()
	{
		std::vector<std::string> roomList;
		bool discovered = false;
		print("looking for host on LAN...");
		using namespace client;

		sf::IpAddress myIP(sf::IpAddress::getLocalAddress());
		const int ipSize = myIP.toString().length();

		std::string ip; char i[15], t[15];
		
		LANrequest.send(myIP.toString().c_str(), 15, broadcastAddress, portr);
		std::size_t received;

		sf::SocketSelector temp;
		multiplayer::client::LANrequest.bind(multiplayer::ports);
		temp.add(LANrequest);
		while (temp.wait(sf::seconds(1)))
		{
			if (temp.isReady(LANrequest))
			{
				LANrequest.receive(i, 15, received, broadcastAddress, portr);
				ip = std::string(i).substr(0, std::string(i).find('-'));
				print(ip << "(server ip)");
				discovered = true;
			
				if (ip.length() > 8)
					roomList.push_back(ip);
			}
		}
		if (!discovered)
			print("no host found");
		LANrequest.unbind();
		return roomList;
	}

	void UpdateMultiplayer()
	{
		while (!shutdown)
		{
			using namespace server;
			if (currentStatus == status::SERVER)
			{
				if (otherPlayersSockets.wait())
				{
					if (otherPlayersSockets.isReady(listener))
					{
						sf::TcpSocket* socket = new sf::TcpSocket;
						print(socket);
						if (listener.accept(*socket) == sf::Socket::Done)
						{
							std::string nick = receiveData<std::string>(*socket);

							int ID = playerList.size() + 1;
							sendData<int>(ID, *socket);

							sendData<std::string>(you->nick, *socket);

							{
								using namespace sf;
								playerList.push_back(std::make_pair(Player(nick, Vector2u(12, 8), Vector2u(1, 1), "ghost", Vector2f(3, 3)), socket));
								auto last = (playerList.end() - 1);
								last->first.clientID = ID;
								last->first.nick = nick;
							}

							//init new player's map
							for (auto& itr : playerList)
							{
								sendData<int>(itr.first.clientID, *socket);
								sendData<std::string>(itr.first.nick, *socket);
							}

							//update the map of the other players
							for (auto& itr : playerList)
							{
								if (itr.first.clientID != ID)
								{
									sendData<int>(6, *itr.second);
									sendData<int>(ID, *itr.second);
									sendData<std::string>(nick, *itr.second);
								}
							}

							otherPlayersSockets.add(*socket);
							you->connected = true;
						}
						else
						{
							delete socket;
						}
					}
					else if (otherPlayersSockets.isReady(broadcast))
					{
						char t[15]; std::size_t received;

						sf::Packet requestPacket;
						sf::IpAddress tempIP, tempIP2;

						sf::Packet temp;
						unsigned short dest = port;
						if (broadcast.receive(t, 15, received, broadcastAddress, portr) == sf::Socket::Done)
						{
							print(t << "(client ip)");
							if (t == ip)
							{
								print("localhost detected");
								tempIP = tempIP2 = "localhost";
							}
							else
							{
								tempIP2 = std::string(ip.toString() + "-").c_str();
								tempIP = t;
							}


							if (broadcast.send(tempIP2.toString().c_str(), 15, tempIP, ports) == sf::Socket::Done)
								print("received and sent");
							else
								print("error");
						}
					}
					else
					{	
						int i = 0;
						for (std::vector<std::pair<Player, sf::TcpSocket*>>::iterator itr = playerList.begin(); itr != playerList.end(); itr++)
						{
							sf::TcpSocket& socket = *itr->second;
							if (otherPlayersSockets.isReady(socket))
							{
								int request = receiveData<int>(socket);
								if (request == 0)
								{
									int dir = 0;
									sf::Vector2f oldPosition = itr->first.playerRect.getPosition();
									sf::Vector2f newPosition = receiveVector(socket);
									itr->first.playerRect.setPosition(newPosition);
									if (oldPosition.y > newPosition.y)
									{
										itr->first.animate(4, 1, 3);
									}
									else if (oldPosition.y < newPosition.y)
									{
										itr->first.animate(1, 1, 3);
									}
									else if (oldPosition.x > newPosition.x)
									{
										itr->first.animate(2, 1, 3);
									}
									else if (oldPosition.x < newPosition.x)
									{
										itr->first.animate(3, 1, 3);
									}
									for (auto& itrj : playerList)
									{
										if (itr->first.clientID != itrj.first.clientID)
										{
											sf::TcpSocket& otherSocket = *itrj.second;
											sendData<int>(0, otherSocket);
											sendData<int>(itr->first.clientID, otherSocket);
											sendVector(itr->first.playerRect.getPosition(), otherSocket);
										}
									}
								}
							}
						}
					}
				}
			}
			else if (currentStatus == status::CLIENT&&you->connected)
			{	
				if (server::otherPlayersSockets.wait())
				{
					if (server::otherPlayersSockets.isReady(socket))
					{
						const int request = receiveData<int>(socket);
						if (request == 0)
						{
							const int updateID = receiveData<int>(socket);
							sf::Vector2f oldPosition = dummies.at(updateID).playerRect.getPosition();
							sf::Vector2f newPosition = receiveVector(socket);
							dummies.at(updateID).playerRect.setPosition(newPosition);
							if (oldPosition.y > newPosition.y)
							{
								dummies.at(updateID).animate(4, 1, 3);
							}
							else if (oldPosition.y < newPosition.y)
							{
								dummies.at(updateID).animate(1, 1, 3);
							}
							else if (oldPosition.x > newPosition.x)
							{
								dummies.at(updateID).animate(2, 1, 3);
							}
							else if (oldPosition.x < newPosition.x)
							{
								dummies.at(updateID).animate(3, 1, 3);
							}
						}
						else if (request == 6)
						{
							const int newPlayerID = receiveData<int>(socket);
							const std::string nick = receiveData<std::string>(socket);

							using namespace sf;
							dummies.insert(std::make_pair(newPlayerID, Player(nick, Vector2u(12, 8), Vector2u(1, 1), "ghost", Vector2f(3, 3))));
						}
						else if (request == 20)
						{
							const int updateID = receiveData<int>(socket);
							const int newAngle = receiveData<int>(socket);
							print("ID=" << updateID << ", angle=" << newAngle);
						}
					}
				}
			}
			sf::sleep(sf::milliseconds(1));
		}
		if(!playerList.empty())
			for (std::vector<std::pair<Player, sf::TcpSocket*>>::iterator itr = playerList.begin(); itr != playerList.end(); itr++)
			{
				print(itr->second << " deleted");
				delete itr->second;
			}
		finishedThread = true;
	}
	
	namespace thread
	{
		void commandInput()
		{
			currentStatus = status::NO_DEFINED;
			using namespace std;
			std::string command;
			print("command thread initialized!");
			while (!shutdown)
			{
				getline(cin, command);
				if (command == "host")
				{
					host();
				}
				else if (command == "join")
				{
					printf("host ip: ");
					std::string ip, nick;
					getline(cin, ip);
					printf("nick: ");
					getline(cin, you->nick);
					join(ip, you->nick);
				}
				else
					print("unknown command");
			}
		}
		//std::thread sendUpdateThread(&sendUpdate);
		std::thread commandThread(&commandInput);

		std::thread update(&UpdateMultiplayer);
	}
}