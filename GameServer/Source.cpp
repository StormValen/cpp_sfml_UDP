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
};

std::map<int, Player> aPlayers;

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

		if (cmd == "HELLO") {
			Packet >> newPlayer.name;
			newPlayer.id = newPlayer.port = Port;
			newPlayer.ip = Ip;
			newPlayer.xPos = rand() % 499;
			newPlayer.yPos = rand() % 499;

			// A�adir nuevo jugador al array.
			aPlayers.insert(std::pair<int, Player>(newPlayer.id, newPlayer));

			// Print del packet.
			std::cout << "<PLAYER IP> " << Ip << " | " << "<PLAYER PORT & ID> " << Port << " | " << "PACKET: " << cmd << "_" << newPlayer.name << std::endl;
			Packet.clear();

			// Paquete WELCOME.
			cmd = "WELCOME";
			Packet << cmd << newPlayer.id << newPlayer.xPos << newPlayer.yPos;

			// Send WELCOME.
			if (socket.send(Packet, newPlayer.ip, newPlayer.port) != sf::Socket::Done) {
				std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
			}
			Packet.clear();

			// Enviar jugadores connectados anteriormente.
			for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
				cmd = "NEW_PLAYER";
				Packet << cmd << it->second.id << it->second.name << it->second.xPos << it->second.yPos;

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
				Packet << cmd << newPlayer.id << newPlayer.name << newPlayer.xPos << newPlayer.yPos;

				if (newPlayer.id != it->second.id) {
					if (socket.send(Packet, it->second.ip, it->second.port) != sf::Socket::Done) {
						std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
					}
				}
				Packet.clear();
			}
		}

		if (aPlayers.size() == MAX_PLAYERS) {
			allPlayersConnected = true;
			std::cout << "<INFO> All players are connected" << std::endl;
		}
	}
}


int main()
{
	srand(time(NULL));
	// 1. Establecimiento de connexi�n
	Connection();
	system("pause");
	// 2. Game Loop
	// 3. Gestion de desconnexiones
	return 0;
}