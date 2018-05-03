#include <stdlib.h>
// #include <unistd.h>
#include <fcntl.h>
#include <signal.h>
// #include <wait.h>
#include <thread>
#include <stdio.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

/*
struct Player
{
	int ID;
	float posX, posY;
	std::string name;
};
struct Movment
{
	float movX, movY;
	int IDMove;
};
*/
/**
* Si vale true --> nos permite marcar casilla con el mouse
* Si vale false --> No podemos interactuar con el tablero y aparece un letrero de "esperando"
*/

//bool tienesTurno = true;



/**
* Ahora mismo no tiene efecto, pero luego lo necesitarás para validar los movimientos
* en función de si eres el gato o el ratón.
*/
//TipoProceso quienSoy = TipoProceso::RATON;


/**
* Cuando el jugador clica en la pantalla, se nos da una coordenada del 0 al 500.
* Esta función la transforma a una posición entre el 0 y el 24
*/
/*sf::Vector2f TransformaCoordenadaACasilla(int _x, int _y)
{
	float xCasilla = _x / LADO_CASILLA;
	float yCasilla = _y / LADO_CASILLA;
	sf::Vector2f casilla(xCasilla, yCasilla);
	return casilla;
}*/

/**
* Si guardamos las posiciones de las piezas con valores del 0 al 7,
* esta función las transforma a posición de ventana (pixel), que va del 0 al 512
*/
/*sf::Vector2f BoardToWindows(sf::Vector2f _position)
{
	return sf::Vector2f(_position.x*LADO_CASILLA, _position.y*LADO_CASILLA);
}*/

/*
sf::UdpSocket socket;
Player player;
std::map<int, Player>Players;
int ID;
Movment movActual;
std::vector<Movment>listMovments;
*/
/*
void old_resetMov(Movment* mov) {
	mov->movX = 0;
	mov->movY = 0;
}
void old_Connection(){
	bool send = false;
	sf::Packet packetLog;
	std::string name;
	std::cout << "Introduce tu nombre" << std::endl;
	std::cin >> name;
	packetLog << name;
	sf::Clock c;
	c.restart();
	while (!send) {
		if (c.getElapsedTime().asMilliseconds() >= 500) {
			if (socket.send(packetLog, "localhost", 50000) != sf::Socket::Done) {
				std::cout << "Error al enviar" << std::endl;
			}
			c.restart();
			send = true;
		}
	}
	packetLog.clear();

	sf::IpAddress IP;
	unsigned short port;
	sf::Packet packR;
	if (socket.receive(packR, IP, port) != sf::Socket::Done) {
		std::cout << "Error al recivir";
	}
	std::string welcome = "";
	int size = 0;
	packR >> size;
	for (int i = 0; i < size; i++) {
		packR >> welcome >> player.name >> player.ID >> player.posX >> player.posY;
		if (welcome == "CMD_WELCOME") {
			//Players.(player);
			Players.insert(std::pair<int, Player>(player.ID, player));
			std::cout << Players[i].name << " ID:" << Players[i].ID << " POS: " << Players[i].posX << " " << Players[i].posY << std::endl;
		}
	}
	packR.clear();
}
*/
/**
* Contiene el código SFML que captura el evento del clic del mouse y el código que pinta por pantalla
*/
/*
void old_Gameplay()
{
	socket.setBlocking(false);
	sf::Packet packS;
	int idAux2 = 0;
	sf::Vector2f casillaOrigen, casillaDestino;
	bool casillaMarcada = false;
	sf::Clock clockMov;
	clockMov.restart();
	sf::RenderWindow window(sf::VideoMode(500, 500), "El Gato y el Raton");
	while (window.isOpen())
	{
		sf::Event event;
		sf::Packet packMov;
		sf::Packet pack;

		std::string cmd;
		sf::IpAddress _IP;
		unsigned short _port;

		if (socket.receive(pack, _IP, _port) != sf::Socket::Done) {
			//std::cout << "Error al recivir";
		}
		pack >> cmd; 

		if (cmd == "CMD_NEW_PLAYER") {
			Player newPlayer;
			pack >> newPlayer.name >> newPlayer.ID >> newPlayer.posX >> newPlayer.posY;
			std::cout << " > " << cmd << " ID: " << newPlayer.ID << " POS: " << newPlayer.posX << newPlayer.posY << std::endl;
			Players.insert(std::pair<int, Player>(newPlayer.ID, newPlayer));
		//	Players.push_back(newPlayer);
		}
		else if (cmd == "CMD_PING") {
			sf::Clock clock;
			clock.restart();
			packS << "CMD_ACK" << player.ID;
			//if (clock.getElapsedTime().asMilliseconds() >= 100) {
			if (socket.send(packS, "localhost", 50000) != sf::Socket::Done) {
				std::cout << "Error al enviar" << std::endl;
			}
			clock.restart();
		}
		else if (cmd == "CMD_DESC") {
			std::string a;
			int idAux;
			pack >> a >> idAux;
			for (int i =0 ; i < Players.size(); i++) {
				if (Players[i].ID == idAux) {
					Players.erase(Players[i].ID);
				}
			}
		}
		//std::cout << cmd;
		else if (cmd == "CMD_OK_MOVE") {
			//int idAux2 = 0;
			int idMoveAux = 0;
			pack >> idAux2 >> idMoveAux;
			std::cout << idAux2 << player.ID <<  std::endl;
			for (std::map<int, Player>::iterator it = Players.begin(); it != Players.end(); ++it) {
			//	std::cout << " X " << it->second.posX << " Y " << it->second.posY << std::endl;
				if (it->first == idAux2) {
					pack >> it->second.posX >> it->second.posY;
					//std::cout << " X " << it->second.posX << " Y " << it->second.posY << std::endl;
				}
			}
			pack.clear();
		}
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				socket.unbind();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Right) {
					movActual.movX++;
				}
				if (event.key.code == sf::Keyboard::Left) {
					movActual.movX--;
				}
				if (event.key.code == sf::Keyboard::Up) {
					movActual.movY--;
				}
				if (event.key.code == sf::Keyboard::Down) {
					movActual.movY++;
				}

				if (clockMov.getElapsedTime().asMilliseconds() > 200) {
					movActual.IDMove++;
					listMovments.push_back(movActual);
					packMov << "CMD_MOV" << movActual.IDMove << player.ID << movActual.movX << movActual.movY;

					if (socket.send(packMov, "localhost", 50000) != sf::Socket::Done) {
						std::cout << "Error al enviar la posicion" << std::endl;
					}
					else {
						//std::cout << "ID " << player.ID << " IDM " << movActual.IDMove << " X " << movActual.movX << " Y " << movActual.movY << std::endl;
						old_resetMov(&movActual);
						clockMov.restart();
					}
				}
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left && tienesTurno)
				{
					int x = event.mouseButton.x;
					int y = event.mouseButton.y;
					if (!casillaMarcada)
					{
						casillaOrigen = TransformaCoordenadaACasilla(x, y);
						casillaMarcada = true;
						//TODO: Comprobar que la casilla marcada coincide con las posición del raton (si le toca al ratón)
						//o con la posicion de alguna de las piezas del gato (si le toca al gato)

					}
					else
					{
						casillaDestino = TransformaCoordenadaACasilla(x, y);
						if (casillaOrigen.x == casillaDestino.x && casillaOrigen.y == casillaDestino.y)
						{
							casillaMarcada = false;
							//Si me vuelven a picar sobre la misma casilla, la desmarco
						}
						else
						{
							if (quienSoy == TipoProceso::RATON)
							{
								//TODO: Validar que el destino del ratón es correcto

								//TODO: Si es correcto, modificar la posición del ratón y enviar las posiciones al padre

							}
							else if (quienSoy == TipoProceso::GATO)
							{
								//TODO: Validar que el destino del gato es correcto

								//TODO: Si es correcto, modificar la posición de la pieza correspondiente del gato y enviar las posiciones al padre
							}
						}
					}
				}
				break;
			default:
				break;

			}
		}

		window.clear();

		//A partir de aquí es para pintar por pantalla
		//Este FOR es para el tablero
		for (int i = 0; i<SIZE_FILA_TABLERO; i++)
		{
			for (int j = 0; j<SIZE_FILA_TABLERO; j++)
			{
				sf::RectangleShape rectBlanco(sf::Vector2f(LADO_CASILLA, LADO_CASILLA));
				rectBlanco.setFillColor(sf::Color::White);
				if (i % 2 == 0)
				{
					//Empieza por el blanco
					if (j % 2 == 0)
					{
						rectBlanco.setPosition(sf::Vector2f(i*LADO_CASILLA, j*LADO_CASILLA));
						window.draw(rectBlanco);
					}
				}
				else
				{
					//Empieza por el negro
					if (j % 2 == 1)
					{
						rectBlanco.setPosition(sf::Vector2f(i*LADO_CASILLA, j*LADO_CASILLA));
						window.draw(rectBlanco);
					}
				}
			}
		}

		//TODO: Para pintar el circulito del ratón
		/*sf::CircleShape shapeRaton(RADIO_AVATAR);
		shapeRaton.setFillColor(sf::Color::Blue);
		sf::Vector2f posicionRaton(player.posX, player.posY);
		posicionRaton = BoardToWindows(posicionRaton);
		shapeRaton.setPosition(posicionRaton);
		window.draw(shapeRaton);*/

		//for (std::map<int, Player>::iterator it = Players.begin(); it != Players.end(); ++it) {
			//sf::CircleShape shapeRaton(RADIO_AVATAR);
			//if (it->first == player.ID) {
				//shapeRaton.setFillColor(sf::Color::Blue);
			//}
			//else {
			//	shapeRaton.setFillColor(sf::Color::Red);
			//}
			//sf::Vector2f positionGato1(it->second.posX, it->second.posY);
			//positionGato1 = BoardToWindows(positionGato1);
			//shapeRaton.setPosition(positionGato1);

			//window.draw(shapeRaton);
		//}
	
	/*	//Pintamos los cuatro circulitos del gato
		//sf::CircleShape shapeGato(RADIO_AVATAR);
		//shapeGato.setFillColor(sf::Color::Red);

		//sf::Vector2f positionGato1(1.f, 0.f);
		//positionGato1 = BoardToWindows(positionGato1);
		//shapeGato.setPosition(positionGato1);

		//window.draw(shapeGato);

		//sf::Vector2f positionGato2(3.f, 0.f);
		//positionGato2 = BoardToWindows(positionGato2);
		//shapeGato.setPosition(positionGato2);

		//window.draw(shapeGato);

		//sf::Vector2f positionGato3(5.f, 0.f);
		//positionGato3 = BoardToWindows(positionGato3);
		//shapeGato.setPosition(positionGato3);

		//window.draw(shapeGato);
		if (!tienesTurno)
		{
			//Si no tengo el turno, pinto un letrerito de "Esperando..."
			sf::Font font;
			std::string pathFont = "liberation_sans/LiberationSans-Regular.ttf";
			if (!font.loadFromFile(pathFont))
			{
				std::cout << "No se pudo cargar la fuente" << std::endl;
			}


			sf::Text textEsperando("Esperando...", font);
			textEsperando.setPosition(sf::Vector2f(180, 200));
			textEsperando.setCharacterSize(30);
			textEsperando.setStyle(sf::Text::Bold);
			textEsperando.setFillColor(sf::Color::Green);
			window.draw(textEsperando);
		}
		else
		{
			//Si tengo el turno y hay una casilla marcada, la marco con un recuadro amarillo.
			if (casillaMarcada)
			{
				sf::RectangleShape rect(sf::Vector2f(LADO_CASILLA, LADO_CASILLA));
				rect.setPosition(sf::Vector2f(casillaOrigen.x*LADO_CASILLA, casillaOrigen.y*LADO_CASILLA));
				rect.setFillColor(sf::Color::Transparent);
				rect.setOutlineThickness(5);
				rect.setOutlineColor(sf::Color::Yellow);
				window.draw(rect);
			}
		}
		window.display();
	}
}
*/


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

struct Player {
	int id;
	std::string name;
	int xPos;
	int yPos;
	bool isInfected;
};

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

	// Paquete critico
	aPackets_Sended.insert(std::pair<int, sf::Packet>(id_Packet, Packet));

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
	std::string base = "Game Window - ";
	std::string window_name = base + localPlayer.name;
	sf::RenderWindow window(sf::VideoMode(500, 500), "Game Window - " + localPlayer.name);

	// socket.setBlocking(false);

	while (window.isOpen())
	{
		if (!CheckPlayersAlive()) {
			std::cout << "<INFO> Checking players ..." << std::endl;
			return;
		}

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
					localPlayer.yPos -= 2;

					sf::Packet Packet;
					std::string cmd = "MOVEMENT";

					Packet << cmd;
					Packet << id_Packet;
					Packet << localPlayer.name;
					Packet << localPlayer.xPos;
					Packet << localPlayer.yPos;


					if (socket.send(Packet, "localhost", 50000) != sf::Socket::Done) {
						std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
					}
					id_Packet++;

				}
				else if (event.key.code == sf::Keyboard::Down) {
					localPlayer.yPos += 2;

					sf::Packet Packet;
					std::string cmd = "MOVEMENT";

					Packet << cmd;
					Packet << id_Packet;
					Packet << localPlayer.name;
					Packet << localPlayer.xPos;
					Packet << localPlayer.yPos;


					if (socket.send(Packet, "localhost", 50000) != sf::Socket::Done) {
						std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
					}
					id_Packet++;
				}
				else if (event.key.code == sf::Keyboard::Right) {
					localPlayer.xPos += 2;

					sf::Packet Packet;
					std::string cmd = "MOVEMENT";

					Packet << cmd;
					Packet << id_Packet;
					Packet << localPlayer.name;
					Packet << localPlayer.xPos;
					Packet << localPlayer.yPos;


					if (socket.send(Packet, "localhost", 50000) != sf::Socket::Done) {
						std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
					}
					id_Packet++;
				}
				else if (event.key.code == sf::Keyboard::Left) {
					localPlayer.xPos -= 2;

					sf::Packet Packet;
					std::string cmd = "MOVEMENT";

					Packet << cmd;
					Packet << id_Packet;
					Packet << localPlayer.name;
					Packet << localPlayer.xPos;
					Packet << localPlayer.yPos;


					if (socket.send(Packet, "localhost", 50000) != sf::Socket::Done) {
						std::cout << "<ERROR> An error has ocurred when sending a packet" << std::endl;
					}
					id_Packet++;
				}
			default:
				break;

			}
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
			else {
				remotePlayerCircle.setFillColor(sf::Color::Blue);
			}
			sf::Vector2f remotePlayerPos(it->second.xPos, it->second.yPos);
			remotePlayerCircle.setPosition(remotePlayerPos);

			window.draw(remotePlayerCircle);
		}

		window.display();
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

		int temp_xPos;
		int temp_yPos;
		int temp_id;

		Packet >> cmd;
		Packet >> temp_id_Packet;


		if (cmd == "MOVEMENT_FROM_PLAYER") {
			Packet >> temp_id;
			Packet >> temp_xPos;
			Packet >> temp_yPos;

			for (std::map<int, Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
				if (it->second.id == temp_id) {
					it->second.xPos = temp_xPos;
					it->second.yPos = temp_yPos;
				}
			}
		}
		Packet.clear();
	}
}

int main()
{
	Connection();
	WaitToNewConnections();
	std::thread tr(ReceiveFromServerInGameLoop);
	GameLoop();
	std::cout << "<INFO> The Game is Over, Goodbye" << std::endl;
	system("pause");
	return 0;
}