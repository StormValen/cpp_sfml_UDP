#include <SFML\Network.hpp>
#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <cstring>
#include <string>

/*#define MAX_PLAYERS 4
sf::UdpSocket socket;

struct Movment
{
	float movX, movY;
	int IDMove;
};
struct Player
{
	sf::IpAddress IP;
	unsigned short port;
	float posX, posY;
	std::string name;
	sf::Clock timePing;
	Movment movment;
};

int ID;
std::map<int, Player> Players;
float maxY = 480;
float minY = 1.f;
float maxX = 480.f;
float minX = 1.f;
//StateModes --> chat_mode - countdown_mode - bet_money_mode - bet_number_mode - simulate_game_mode - bet_processor_mode

void old_Recorrer() {
	for (std::map<int, Player>::iterator it = Players.begin(); it != Players.end(); ++it) {
		for (std::map<int, Player>::iterator it2 = Players.begin(); it2 != Players.end(); ++it2) {
			sf::Packet pack;
			pack << it2->first << it2->second.posX << it2->second.posY;
			if (socket.send(pack, it->second.IP, it->second.port) != sf::Socket::Done) {
				std::cout << "error";
			}
		}
	}

}

void old_Connection() {
	srand(time(NULL));
	sf::Packet packetLog;
	sf::Packet newPlayerPack;

	socket.bind(50000);
	Player player;
	for (int i = 0; i < MAX_PLAYERS; i++) {
		sf::IpAddress IP;
		unsigned short port;
		if (socket.receive(packetLog, IP, port) != sf::Socket::Done) {
			std::cout << "Error al recivir" << std::endl;
		}
		packetLog >> player.name;
		player.IP = IP;
		player.port = port;
		player.posX = rand() % 499;
		player.posY = rand() % 499;

		for (std::map<int, Player>::iterator it = Players.begin(); it != Players.end(); ++it) {
			std::string cmd = "CMD_NEW_PLAYER";
			newPlayerPack << cmd << player.name  << i << player.posX << player.posY;
			if (socket.send(newPlayerPack, it->second.IP, it->second.port) != sf::Socket::Done) {
				std::cout << "Error al enviar nueva conexion" << std::endl;
			}
			newPlayerPack.clear();
		}

		Players.insert(std::pair<int, Player>(i, player));
		packetLog.clear();
		ID = i;
		packetLog << (int)Players.size();

		for (std::map<int, Player>::iterator it = Players.begin(); it != Players.end(); ++it) {
			std::string cmd = "CMD_WELCOME";
			packetLog << cmd << it->second.name << it->first << it->second.posX << it->second.posY;
			std::cout << "Se ha conectado : " << it->second.name << cmd << it->first << it->second.posX << it->second.posY << std::endl;
		}
		if (socket.send(packetLog, IP, port) != sf::Socket::Done) {
			std::cout << "error";
		}
		packetLog.clear();
	}
	
}

void old_sendAllPlayers(std::string msg, int id) {
	sf::Packet packDes;
	for (std::map<int, Player>::iterator it = Players.begin(); it != Players.end(); ++it) {
		std::string cmd = "CMD_DESC";
		packDes << cmd << msg << id;
		if (socket.send(packDes, it->second.IP, it->second.port) != sf::Socket::Done) {
			std::cout << "Error al enviar nueva conexion" << std::endl;
		}
		packDes.clear();
	}
}

void old_Game() {
	for (std::map<int, Player>::iterator it = Players.begin(); it != Players.end(); ++it) {
		it->second.timePing.restart();
	}
	socket.setBlocking(false);
	sf::IpAddress IP;
	unsigned short port;
	bool send = false;
	sf::Packet packR, packPingS;
	sf::Clock clockP;
	clockP.restart();
	std::string ping;
	int id;
	std::string cmd;
	while (true) {
		if (clockP.getElapsedTime().asMilliseconds() > 1000) {
			for (std::map<int, Player>::iterator it = Players.begin(); it != Players.end(); ++it) {
				ping = "CMD_PING";
				packPingS << ping;
				if (socket.send(packPingS, it->second.IP, it->second.port) != sf::Socket::Done) {
					std::cout << "Error al enviar el ping" << std::endl;
				}
				else {
					clockP.restart();
					packPingS.clear();
				}
			}
		}
		for (std::map<int, Player>::iterator it = Players.begin(); it != Players.end(); ++it) {
			if (it->second.timePing.getElapsedTime().asSeconds() >= 5) {
				old_sendAllPlayers("Desconectado con la ID: ", it->first);
				std::cout << "Desconexion con la ID: " << it->first << std::endl;
				Players.erase(it->first);
			}
		}
		if (socket.receive(packR, IP, port) != sf::Socket::Done) {
		}
		else {
			packR >> cmd;
			if (cmd == "CMD_ACK") {
				packR >> id;
				//std::cout << id << ping;
				Players.find(id)->second.timePing.restart();
			}
			if (cmd == "CMD_MOV") {
				int idMove, id;
				float deltaX, deltaY;
				packR >> idMove >> id >> deltaX >> deltaY;
				std::string a = "CMD_OK_MOVE";
				sf::Packet packM;
				packM << a;
				for (std::map<int, Player>::iterator it = Players.begin(); it != Players.end(); ++it) {
					for (std::map<int, Player>::iterator it2 = Players.begin(); it2 != Players.end(); ++it2) {
						if (it2->first == id) {
							it2->second.movment.IDMove = idMove;
							if ((it2->second.posX += deltaX) > maxX || (it2->second.posX += deltaX) < minX || (it2->second.posY += deltaY) > maxY || (it2->second.posY += deltaY) < minY) {
								it2->second.movment.movX = 0;
								it2->second.movment.movY = 0;
							}
							else {
								it2->second.movment.movX = deltaX;
								it2->second.movment.movY = deltaY;
								it2->second.posX += it2->second.movment.movX;
								it2->second.posY += it2->second.movment.movY;
								packM << it2->first << it2->second.movment.IDMove << it2->second.posX << it2->second.posY;
								//std::cout << " ID" << it2->first << " IDM " << it2->second.movment.IDMove << " X " << deltaX << " Y " << deltaY << " posX " << it2->second.posX << " posY " << it2->second.posY;
							}
						}
					}
					if (socket.send(packM, it->second.IP, it->second.port) != sf::Socket::Done) {
						std::cout << "Error al enviar mov" << std::endl;
					}
					//std::cout << it->second.name << it->second.IP << it->second.port << std::endl;
				}
				packM.clear();
			}
		}
		packR.clear();
	}
}
*/


#define MAX_PLAYERS 4

sf::UdpSocket socket;
bool allPlayersConnected = false;

struct Player {
	sf::IpAddress ip;
	int id;
	unsigned short port;
	std::string name;
	int xPos;
	int yPos;
	bool isInfected;
	sf::Clock currentInactiveTime;
};

int id_Packet = 0;
int temp_id_Packet;

std::map<int, Player> aPlayers;
std::map<int, sf::Packet> aPackets_Sended;
std::map<int, sf::Packet> aPackets_Received;


void Connection() {
	socket.bind(50000);

	// Bucle para conectar todos los jugadores.
	while (!allPlayersConnected) {

		// Variables para necesarias para el receive.
		sf::IpAddress Ip;
		unsigned short Port;
		sf::Packet Packet;
		std::string cmd;

		// Receive HELLO.
		if (socket.receive(Packet, Ip, Port) != sf::Socket::Done) {
			std::cout << "<ERROR> An error has ocurred when receiveing a packet" << std::endl;
		}

		// Creacion del nuevo jugador.
		Player newPlayer;
		Packet >> cmd;
		Packet >> temp_id_Packet;
		if (cmd == "HELLO") {
			bool playerAlreadyExists = false;

			Packet >> newPlayer.name;
			newPlayer.id = newPlayer.port = Port;
			newPlayer.ip = Ip;
			newPlayer.xPos = rand() % 499;
			newPlayer.yPos = rand() % 499;
			newPlayer.isInfected = false;

			// Comprovar que el jugador no existe.
			for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
				if (it->second.id == newPlayer.id || it->second.name == newPlayer.name) {
					playerAlreadyExists = true;
				}
			}

			if (!playerAlreadyExists) {
				// Añadir nuevo jugador al array.
				aPlayers.insert(std::pair<int, Player>(newPlayer.id, newPlayer));

				// Print del packet.
				std::cout << "<PLAYER IP> " << Ip << " | " << "<PLAYER PORT & ID> " << Port << " | " << "PACKET: " << temp_id_Packet << "_" << cmd << "_" << newPlayer.name << std::endl;
				Packet.clear();

				// Paquete WELCOME.
				cmd = "WELCOME";
				Packet << cmd << id_Packet << newPlayer.id << newPlayer.xPos << newPlayer.yPos;
				id_Packet++;

				// Send WELCOME.
				if (socket.send(Packet, newPlayer.ip, newPlayer.port) != sf::Socket::Done) {
					std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
				}
				Packet.clear();

				// Enviar jugadores connectados anteriormente.
				for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
					cmd = "NEW_PLAYER";
					Packet << cmd << id_Packet << it->second.id << it->second.name << it->second.xPos << it->second.yPos;
					id_Packet++;
					aPackets_Sended.insert(std::pair<int, sf::Packet>(id_Packet, Packet));

					if (newPlayer.id != it->second.id) {
						if (socket.send(Packet, newPlayer.ip, newPlayer.port) != sf::Socket::Done) {
							std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
						}
					}
					Packet.clear();
				}

				// Recorrer aPlayers y enviar nuevo jugador connectado.
				for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
					cmd = "NEW_PLAYER";
					Packet << cmd << id_Packet << newPlayer.id << newPlayer.name << newPlayer.xPos << newPlayer.yPos;
					id_Packet++;
					aPackets_Sended.insert(std::pair<int, sf::Packet>(id_Packet, Packet));

					if (newPlayer.id != it->second.id) {
						if (socket.send(Packet, it->second.ip, it->second.port) != sf::Socket::Done) {
							std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
						}
					}
					Packet.clear();
				}
			}
			else if (playerAlreadyExists) {
				// Enviar que ese jugador ya existe.
				Packet.clear();
				std::cout << "<INFO> A player with this username already exists" << std::endl;
				for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
					cmd = "PLAYER_EXISTS";
					Packet << cmd << id_Packet;
					id_Packet++;

					if (socket.send(Packet, newPlayer.ip, newPlayer.port) != sf::Socket::Done) {
						std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
					}
					Packet.clear();
				}
			}
		}

		if (aPlayers.size() == MAX_PLAYERS) {
			allPlayersConnected = true;
			std::cout << "<INFO> All players are connected" << std::endl;
		}
	}
}

void Game() {
	bool end = false;
	while (!end) {

		// Variables para necesarias para el receive.
		sf::IpAddress Ip;
		unsigned short Port;
		sf::Packet Packet;
		std::string cmd;

		Packet.clear();
		// Receive indicaciones cliente.
		if (socket.receive(Packet, Ip, Port) != sf::Socket::Done) {
			std::cout << "<ERROR> An error has ocurred when receiveing a packet" << std::endl;
		}

		Packet >> cmd;
		if (cmd == "MOVEMENT") {
			std::string temp_name;
			int temp_xPos, temp_yPos;

			Packet >> temp_id_Packet;
			Packet >> temp_name;
			Packet >> temp_xPos;
			Packet >> temp_yPos;

			std::cout << "<INFO> Player: " << temp_name << " is moving to X: " << temp_xPos << " Y: " << temp_yPos << std::endl;
			Player temp_Player = aPlayers.find((int)Port)->second;
			temp_Player.xPos = temp_xPos;
			temp_Player.yPos = temp_yPos;
			temp_Player.currentInactiveTime.restart();

			Packet.clear();
			
			sf::Packet newPacket;
			cmd = "MOVEMENT_FROM_PLAYER";
			newPacket << cmd;
			newPacket << id_Packet;
			newPacket << temp_Player.id;
			newPacket << temp_xPos;
			newPacket << temp_yPos;

			id_Packet++;

			for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
				if (temp_Player.id != it->second.id) {
					if (socket.send(newPacket, it->second.ip, it->second.port) != sf::Socket::Done) {
						std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
					}
				}
			}
			newPacket.clear();

		}

		for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
			if (it->second.currentInactiveTime.getElapsedTime().asMilliseconds() >= 50000) {
				std::cout << "<INFO> Player with ID: " << it->second.id << " and NAME: " << it->second.name << " has been disconnected for innactivity" << std::endl;
			}
		}
	}
}


int main()
{
	srand(time(NULL));
	// 1. Establecimiento de connexión
	Connection();
	Game();
	system("pause");
	// 2. Game Loop
	// 3. Gestion de desconnexiones
	return 0;
}