#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <thread>
#include <stdio.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>


#define MAX_ENEMIES 3

#define MAX 100
#define SIZE_TABLERO 625
#define SIZE_FILA_TABLERO 25
#define LADO_CASILLA 20
#define RADIO_AVATAR 10.f
#define OFFSET_AVATAR 1

#define SIZE_TABLERO 64
#define LADO_CASILLA 20
#define RADIO_AVATAR 10.f
#define OFFSET_AVATAR 1

enum TipoProceso { RATON, GATO, PADRE };
char tablero[SIZE_TABLERO];

struct Movement {
	int xPos;
	int yPos;
	int id_Move;
};

struct Player {
	int id;
	std::string name;
	int xPos;
	int yPos;
	bool isInfected;
	bool isDisconnected;
	std::vector<Movement> aMovements;
};


Movement localPlayerMovement;

int id_Packet = 0;
int temp_id_Packet;

Player localPlayer;

sf::UdpSocket socket;
std::map<int, Player> aPlayers;	// Array de jugadores.
std::map<int, sf::Packet> aPackets_Sended; // Array de paquetes criticos enviados.
std::map<int, sf::Packet> aPackets_Received; // Array de paquetes criticos recividos.
bool game_end = false;

void Connection() {
	// Reloj
	sf::Clock clock;
	clock.restart();
	bool end = false; 

	// Hello.
	sf::Packet Packet;
	std::string cmd = "HELLO";

	std::cout << "<INFO> Set your name: ";
	std::cin >> localPlayer.name;

	Packet << cmd;
	Packet << id_Packet;
	Packet << localPlayer.name;
	id_Packet++;

	while (!end) {
		if (socket.send(Packet, "localhost", 50000) != sf::Socket::Done && clock.getElapsedTime().asMilliseconds() >= 500) {
			std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
			clock.restart();
		}
		//Packet.clear();

		sf::Packet Packet_Aux;
		sf::IpAddress Ip;
		unsigned short Port;
		if (socket.receive(Packet_Aux, Ip, Port) != sf::Socket::Done) {
			std::cout << "<ERROR> An error has ocurred when receiveing a packet" << std::endl;
		}

		// Welcome.
		Packet_Aux >> cmd;
		Packet_Aux >> temp_id_Packet;
		if (cmd == "WELCOME") {
			Packet_Aux >> localPlayer.id;
			Packet_Aux >> localPlayer.xPos;
			Packet_Aux >> localPlayer.yPos;
			localPlayer.isInfected = false;
			localPlayer.isDisconnected = false;
			std::cout << "<INFO> Welcome to the game your id is [ " << localPlayer.id << " ]" << std::endl;
			end = true;
		}
		else if (cmd == "PLAYER_EXISTS") {
			std::cout << "<INFO> Sorry this player already exists, plase pick another username" << std::endl;
			Connection();
		}
		Packet_Aux.clear();

	}
}

void WaitToNewConnections() {
	//Recibe los jugadores conectados antes y los que se conectan despues.
	while (aPlayers.size() != MAX_ENEMIES) {

		sf::Packet Packet;
		sf::IpAddress Ip;
		unsigned short Port;
		std::string cmd;

		if (socket.receive(Packet, Ip, Port) != sf::Socket::Done) {
			std::cout << "<ERROR> An error has ocurred when receiveing a packet" << std::endl;
		}

		Packet >> cmd;
		Packet >> temp_id_Packet;
		if (cmd == "NEW_PLAYER") {
			Player newPlayer;
			Packet >> newPlayer.id;
			Packet >> newPlayer.name;
			Packet >> newPlayer.xPos;
			Packet >> newPlayer.yPos;
			newPlayer.isInfected = false;
			newPlayer.isDisconnected = false;
			std::cout << "<INFO> New player connected | NAME: " << newPlayer.name << " ID: " << newPlayer.id << " POS: " << newPlayer.xPos << " - " << newPlayer.yPos << " ID Packet: " << temp_id_Packet << std::endl;
			aPlayers.insert(std::pair<int, Player>(newPlayer.id, newPlayer));
			aPackets_Received.insert(std::pair<int, sf::Packet>(temp_id_Packet, Packet));
		}
	}
}

void PrintCriticos() {
	std::cout << "<INFO> Packets SENDED" << std::endl;
	for (std::map<int, sf::Packet>::iterator it = aPackets_Sended.begin(); it != aPackets_Sended.end(); ++it) {
		std::cout << it->first << std::endl;
	}

	std::cout << "<INFO> Packets RECEIVED" << std::endl;
	for (std::map<int, sf::Packet>::iterator it = aPackets_Received.begin(); it != aPackets_Received.end(); ++it) {
		std::cout << it->first << std::endl;
	}
}

void KillPlayers() {
	localPlayer.isInfected = true;
	for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
		it->second.isInfected = true;
	}
}

bool CheckPlayersAlive() {
	for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
		if (!it->second.isInfected) {
			return true;
		}
	}
	if (!localPlayer.isInfected) {
		return true;
	}
	return false;
}

void GameLoop()
{	
	sf::Clock movAcumClock;
	movAcumClock.restart();

	std::string base = "Game Window - ";
	std::string window_name = base + localPlayer.name;
	sf::RenderWindow window(sf::VideoMode(500, 500), "Game Window - " + localPlayer.name);

	// socket.setBlocking(false);

	while (window.isOpen())
	{
		

		sf::Event event;

		//Este primer WHILE es para controlar los eventos del mouse
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::K) {
					KillPlayers();
				}
				else if (event.key.code == sf::Keyboard::Up) {
					localPlayerMovement.yPos--;
				}
				else if (event.key.code == sf::Keyboard::Down) {
					localPlayerMovement.yPos++;
				}
				else if (event.key.code == sf::Keyboard::Right) {
					localPlayerMovement.xPos++;
				}
				else if (event.key.code == sf::Keyboard::Left) {
					localPlayerMovement.xPos--;
				}
			default:
				break;

			}
		}

		// Enviar la acumulacion de movimiento cuando llega al limite de tiempo.
		if (movAcumClock.getElapsedTime().asMilliseconds() > 50) {
			localPlayerMovement.id_Move++;
			localPlayer.aMovements.push_back(localPlayerMovement);

			sf::Packet Packet;
			std::string cmd = "ACUM_MOVEMENT";

			Packet << cmd;
			Packet << id_Packet;
			Packet << localPlayer.id;
			Packet << localPlayerMovement.id_Move;
			Packet << localPlayerMovement.xPos;
			Packet << localPlayerMovement.yPos;
			id_Packet++;

			if (localPlayerMovement.xPos != 0 || localPlayerMovement.yPos != 0) {
				if (socket.send(Packet, "localhost", 50000) != sf::Socket::Done) {
					std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
				}
			}

			movAcumClock.restart();
			localPlayerMovement.xPos = 0;
			localPlayerMovement.yPos = 0;

			aPackets_Sended.insert(std::pair<int, sf::Packet>(id_Packet, Packet));

			Packet.clear();
		}

		

		window.clear();

		//A partir de aquí es para pintar por pantalla
		//Este FOR es para el tablero
		for (int i = 0; i<SIZE_FILA_TABLERO; i++)
		{
			for (int j = 0; j<SIZE_FILA_TABLERO; j++)
			{
				sf::RectangleShape rectGris(sf::Vector2f(LADO_CASILLA, LADO_CASILLA));
				rectGris.setFillColor(sf::Color(255,255,255,25));
				if (i % 2 == 0)
				{
					//Empieza por el blanco
					if (j % 2 == 0)
					{
						rectGris.setPosition(sf::Vector2f(i*LADO_CASILLA, j*LADO_CASILLA));
						window.draw(rectGris);
					}
				}
				else
				{
					//Empieza por el negro
					if (j % 2 == 1)
					{
						rectGris.setPosition(sf::Vector2f(i*LADO_CASILLA, j*LADO_CASILLA));
						window.draw(rectGris);
					}
				}
			}
		}

		// Pintar posicion del jugador local
		sf::CircleShape localPlayerCircle(RADIO_AVATAR);
		if (localPlayer.isInfected) {
			localPlayerCircle.setFillColor(sf::Color::Green);
		}
		else if (localPlayer.isDisconnected) {
			localPlayerCircle.setFillColor(sf::Color::Yellow);
		}
		else {
			localPlayerCircle.setFillColor(sf::Color::Cyan);
		}
		sf::Vector2f localPlayerPos(localPlayer.xPos, localPlayer.yPos);
		localPlayerCircle.setPosition(localPlayerPos);
		window.draw(localPlayerCircle);

		// Pintar posicion de los jugadores remotos
		for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
			sf::CircleShape remotePlayerCircle(RADIO_AVATAR);
			if (it->second.isInfected) {
				remotePlayerCircle.setFillColor(sf::Color::Red);
			}
			else if (it->second.isDisconnected) {
				remotePlayerCircle.setFillColor(sf::Color::Transparent);
			}
			else {
				remotePlayerCircle.setFillColor(sf::Color::Blue);
			}
			sf::Vector2f remotePlayerPos(it->second.xPos, it->second.yPos);
			remotePlayerCircle.setPosition(remotePlayerPos);

			window.draw(remotePlayerCircle);
		}

		window.display();

		if (localPlayer.isDisconnected == true) {
			window.close();
		}
	}
}

void ReceiveFromServerInGameLoop() {
	bool end = false;
	while (!end) {
		sf::Packet Packet;
		sf::IpAddress Ip;
		unsigned short Port;
		std::string cmd;

		Packet.clear();
		if (socket.receive(Packet, Ip, Port) != sf::Socket::Done) {
			std::cout << "<ERROR> An error has ocurred when receiveing a packet" << std::endl;
		}

		Packet >> cmd;
		Packet >> temp_id_Packet;

		if (cmd == "SET_INFECTED") {
			int temp_id_Player;
			Packet >> temp_id_Player;



			for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
				if (it->second.id == temp_id_Player) {
					it->second.isInfected = true;
					std::cout << "<GAMEPLAY> You're not infected, escape from the other players " << std::endl;
				}
			}

			if (temp_id_Player == localPlayer.id) {
				localPlayer.isInfected = true;
				std::cout << "<GAMEPLAY> You're infected, hunt the other players " << std::endl;
			}
			Packet.clear();
		}

		if (cmd == "PLAYER_DISCONNECTED") {
			int temp_id_Player;
			Packet >> temp_id_Player;

			for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
				if (it->first == temp_id_Player) {
					it->second.isDisconnected = true;
				}
			}

			if (temp_id_Player == localPlayer.id) {
				localPlayer.isDisconnected = true;
				end = true;
				for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
					it->second.isDisconnected = true;
				}
			}
			Packet.clear();
		}

		if (cmd == "MOVEMENT_FROM_PLAYER") {
			int temp_xPosMov;
			int temp_yPosMov;
			int temp_id;

			Packet >> temp_id;
			Packet >> temp_xPosMov;
			Packet >> temp_yPosMov;

			for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
				if (it->second.id == temp_id) {
					it->second.xPos += temp_xPosMov;
					it->second.yPos += temp_yPosMov;
				}
			}
			Packet.clear();
		}

		if (cmd == "ACK_ACUM_MOV") {
			int temp_id_Move;
			Packet >> temp_id_Move;

			for (std::vector<Movement>::iterator it = localPlayer.aMovements.begin(); it != localPlayer.aMovements.end();) {
				if (it->id_Move == temp_id_Move) {
					localPlayer.xPos += it->xPos;
					localPlayer.yPos += it->yPos;

					it = localPlayer.aMovements.erase(it);
				}
				else {
					++it;
				}
			}
			
		}
	}
}

int main()
{
	Connection();
	WaitToNewConnections();
	std::thread tr(ReceiveFromServerInGameLoop);
	GameLoop();
	std::cout << "<INFO> The Game is Over, Goodbye" << std::endl;
	tr.detach();
	//system("pause");
	return 0;
}