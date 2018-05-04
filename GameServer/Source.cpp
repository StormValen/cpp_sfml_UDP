#include <SFML\Network.hpp>
#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <cstring>
#include <string>
#include <random>

#define MAX_PLAYERS 4
#define PERCENT_PACKETLOSS 0.5

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
	bool isDisconnected;
};

int id_Packet = 0;
int temp_id_Packet;

std::map<int, Player> aPlayers;
std::map<int, sf::Packet> aPackets_Sended;
std::map<int, sf::Packet> aPackets_Received;

static float GetRandomFloat() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	//static std::uniform_real_distribution<float>dis(since, to);
	static std::uniform_real_distribution<float>dis(0.f, 1.f);
	return dis(gen);
}

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
		else {
			float loss_Packet = GetRandomFloat();

			if (loss_Packet < PERCENT_PACKETLOSS) {
				std::cout << "<INFO> Packet lost" << std::endl;
			}
			else {
				// Creacion del nuevo jugador.
				Player newPlayer;
				Packet >> cmd;
				Packet >> temp_id_Packet;
				if (cmd == "HELLO") {
					bool playerAlreadyExists = false;

					srand(time(NULL));
					Packet >> newPlayer.name;
					newPlayer.id = newPlayer.port = Port;
					newPlayer.ip = Ip;
					newPlayer.xPos = rand() % 499;
					newPlayer.yPos = rand() % 499;
					newPlayer.isInfected = false;
					newPlayer.isDisconnected = false;

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
			}

		

		}

		if (aPlayers.size() == MAX_PLAYERS) {
			allPlayersConnected = true;
			std::cout << "<INFO> All players are connected" << std::endl;
		}


		// Calcular quien esta infectado al inicio de forma aleatoria y avisar a los todos los jugadores.
		srand(NULL);

		int randInfected = rand() % 4;
		
		int counter = 0;
		for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
			if (counter == randInfected) {
				it->second.isInfected = true;
				std::cout << "<INFECTED> Name: " << it->second.name;
				
				for (std::map<int, Player>::iterator it_2 = aPlayers.begin(); it_2 != aPlayers.end(); ++it_2) {
					Packet.clear();
					cmd = "SET_INFECTED";
					Packet << cmd << id_Packet << it->second.id;
					id_Packet++;

					if (socket.send(Packet, it_2->second.ip, it_2->second.port) != sf::Socket::Done) {
						std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
					}
					Packet.clear();
				}
			}
			counter++;
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

		Packet.clear(); // Esto no hace falta.

		// Receive indicaciones cliente.
		if (socket.receive(Packet, Ip, Port) != sf::Socket::Done) {
			std::cout << "<ERROR> An error has ocurred when receiveing a packet" << std::endl;
		}

		Packet >> cmd;
		if (cmd == "MOVEMENT") {
			// Variables necesarias para actualizar valores.
			std::string temp_name;
			int temp_xPos, temp_yPos;

			// Extraccion del paquete.
			Packet >> temp_id_Packet;
			Packet >> temp_name;
			Packet >> temp_xPos;
			Packet >> temp_yPos;

			// Asignacion de los nuevos valores al jugador.
			Player temp_Player = aPlayers.find((int)Port)->second;
			temp_Player.xPos = temp_xPos;
			temp_Player.yPos = temp_yPos;
			//temp_Player.currentInactiveTime.restart();
			aPlayers.find((int)Port)->second.currentInactiveTime.restart();

			Packet.clear();
			
			// Generar nuevo paquete para informar a los demas jugadores del movimiento.
			sf::Packet newPacket;
			cmd = "MOVEMENT_FROM_PLAYER";
			newPacket << cmd;
			newPacket << id_Packet;
			newPacket << temp_Player.id;
			newPacket << temp_xPos;
			newPacket << temp_yPos;
			id_Packet++;

			// Send de la nueva posicion del jugador.
			for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
				if (temp_Player.id != it->second.id) {
					if (socket.send(newPacket, it->second.ip, it->second.port) != sf::Socket::Done) {
						std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
					}
				}
			}
			newPacket.clear();

		}


		if (cmd == "ACUM_MOVEMENT") {
			int temp_id_Player;
			int temp_id_Move;
			int temp_acumMovX;
			int temp_acumMovY;

			Packet >> temp_id_Packet;
			Packet >> temp_id_Player;
			Packet >> temp_id_Move;
			Packet >> temp_acumMovX;
			Packet >> temp_acumMovY;
			Packet.clear();

			aPlayers.find(temp_id_Player)->second.currentInactiveTime.restart();

			// Validar el movimiento con el mapa.
			bool greenLight = true;



			// Colisiones con las paredes.
			if ((aPlayers.find(temp_id_Player)->second.xPos += temp_acumMovX) <= 0 ) {
				greenLight = false;
			}

			if ((aPlayers.find(temp_id_Player)->second.xPos += temp_acumMovX) >= 512) {
				greenLight = false;
			}

			if ((aPlayers.find(temp_id_Player)->second.yPos += temp_acumMovY) <= 0) {
				greenLight = false;
			}

			if ((aPlayers.find(temp_id_Player)->second.yPos += temp_acumMovY) >= 512) {
				greenLight = false;
			}


			for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
				if (temp_id_Player == it->second.id) {
					it->second.xPos += temp_acumMovX;
					it->second.yPos += temp_acumMovY;
				}
			}


			//Colisiones entre jugadores.
			
			int PLAYER_INFECTED = NULL;
			// Recorre array de jugadores y comprueba si esta infectado.
			for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
				if (it->second.isInfected) {
					// Si esta infectado recorre el array de jugadores para comprobar distancias.
					for (std::map<int, Player>::iterator it_2 = aPlayers.begin(); it_2 != aPlayers.end(); ++it_2) {
						if (it != it_2) {
							if (it_2->second.xPos <= it->second.xPos+15 && it_2->second.xPos >= it->second.xPos - 15 && it_2->second.yPos <= it->second.yPos + 15 && it_2->second.yPos >= it->second.yPos - 15) {
								it_2->second.isInfected = true;
								PLAYER_INFECTED = it_2->second.id;

							}
						}
					}
				}

				// send new infected
				if (PLAYER_INFECTED != NULL) {
					for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
						sf::Packet newPacket;
						std::string new_cmd = "SET_INFECTED";

						newPacket << new_cmd;
						newPacket << temp_id_Packet;
						newPacket << PLAYER_INFECTED;

						if (socket.send(newPacket, it->second.ip, it->second.port) != sf::Socket::Done) {
							std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
						}
						newPacket.clear();
					}
				}
			}

			if (greenLight) {
				std::string new_cmd = "ACK_ACUM_MOV";
				Packet << new_cmd;
				Packet << temp_id_Packet;
				Packet << temp_id_Move;
			
				// ACK de la acumulacion del movimento
				if (socket.send(Packet, aPlayers.find(temp_id_Player)->second.ip, aPlayers.find(temp_id_Player)->second.port) != sf::Socket::Done) {
					std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
				}
				Packet.clear();

				// Generar nuevo paquete para informar a los demas jugadores del movimiento.
				sf::Packet newPacket;
				cmd = "MOVEMENT_FROM_PLAYER";
				newPacket << cmd;
				newPacket << id_Packet;
				newPacket << temp_id_Player;
				newPacket << temp_acumMovX;
				newPacket << temp_acumMovY;
				id_Packet++;

				// Send de la nueva posicion del jugador.
				for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
					if (temp_id_Player != it->second.id) {
						if (socket.send(newPacket, it->second.ip, it->second.port) != sf::Socket::Done) {
							std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
						}
					}
				}
				newPacket.clear();
			}
		}


		// Gestion de la desconexion.
		for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
			if (it->second.currentInactiveTime.getElapsedTime().asMilliseconds() >= 50000) {
				
				it->second.isDisconnected = true;

				// Informar a los demas jugadores de la desconexion.
				for (std::map<int, Player>::iterator it_2 = aPlayers.begin(); it_2 != aPlayers.end(); ++it_2) {
					cmd = "PLAYER_DISCONNECTED";
					Packet << cmd << id_Packet << it->second.id;
					id_Packet++;

					if (socket.send(Packet, it_2->second.ip, it_2->second.port) != sf::Socket::Done) {
						std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
					}
					

					Packet.clear();
				}
			}
		}

		// Comprobar usuarios desconectados para eliminarlos.
		for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end();) {
			if (it->second.isDisconnected == true) {
				std::cout << "<INFO> Client disconnected: " << it->second.name << std::endl;
				it = aPlayers.erase(it);
			}
			else {
				++it;
			}
		}
	}
}


int main()
{
	
	// 1. Establecimiento de connexión
	Connection();
	// 2. Game Loop
	Game();
	system("pause");
	return 0;
}