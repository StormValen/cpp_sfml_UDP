#include <stdlib.h>
// #include <unistd.h>
#include <fcntl.h>
#include <signal.h>
// #include <wait.h>
#include <thread>
#include <stdio.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML\Network.hpp>

#define MAX 100
#define SIZE_TABLERO 64
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
/**
* Si vale true --> nos permite marcar casilla con el mouse
* Si vale false --> No podemos interactuar con el tablero y aparece un letrero de "esperando"
*/
bool tienesTurno = true;

/**
* Ahora mismo no tiene efecto, pero luego lo necesitar�s para validar los movimientos
* en funci�n de si eres el gato o el rat�n.
*/
TipoProceso quienSoy = TipoProceso::RATON;


/**
* Cuando el jugador clica en la pantalla, se nos da una coordenada del 0 al 500.
* Esta funci�n la transforma a una posici�n entre el 0 y el 24
*/
sf::Vector2f TransformaCoordenadaACasilla(int _x, int _y)
{
	float xCasilla = _x / LADO_CASILLA;
	float yCasilla = _y / LADO_CASILLA;
	sf::Vector2f casilla(xCasilla, yCasilla);
	return casilla;
}

/**
* Si guardamos las posiciones de las piezas con valores del 0 al 7,
* esta funci�n las transforma a posici�n de ventana (pixel), que va del 0 al 512
*/
sf::Vector2f BoardToWindows(sf::Vector2f _position)
{
	return sf::Vector2f(_position.x*LADO_CASILLA, _position.y*LADO_CASILLA);
}

sf::UdpSocket socket;
Player player;
std::map<int, Player>Players;
int ID;
Movment movActual;
std::vector<Movment>listMovments;

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
/**
* Contiene el c�digo SFML que captura el evento del clic del mouse y el c�digo que pinta por pantalla
*/
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
						//TODO: Comprobar que la casilla marcada coincide con las posici�n del raton (si le toca al rat�n)
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
								//TODO: Validar que el destino del rat�n es correcto

								//TODO: Si es correcto, modificar la posici�n del rat�n y enviar las posiciones al padre

							}
							else if (quienSoy == TipoProceso::GATO)
							{
								//TODO: Validar que el destino del gato es correcto

								//TODO: Si es correcto, modificar la posici�n de la pieza correspondiente del gato y enviar las posiciones al padre
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

		//A partir de aqu� es para pintar por pantalla
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

		//TODO: Para pintar el circulito del rat�n
		/*sf::CircleShape shapeRaton(RADIO_AVATAR);
		shapeRaton.setFillColor(sf::Color::Blue);
		sf::Vector2f posicionRaton(player.posX, player.posY);
		posicionRaton = BoardToWindows(posicionRaton);
		shapeRaton.setPosition(posicionRaton);
		window.draw(shapeRaton);*/

		for (std::map<int, Player>::iterator it = Players.begin(); it != Players.end(); ++it) {
			sf::CircleShape shapeRaton(RADIO_AVATAR);
			if (it->first == player.ID) {
				shapeRaton.setFillColor(sf::Color::Blue);
			}
			else {
				shapeRaton.setFillColor(sf::Color::Red);
			}
			sf::Vector2f positionGato1(it->second.posX, it->second.posY);
			//positionGato1 = BoardToWindows(positionGato1);
			shapeRaton.setPosition(positionGato1);

			window.draw(shapeRaton);
		}
	
	/*	//Pintamos los cuatro circulitos del gato
		sf::CircleShape shapeGato(RADIO_AVATAR);
		shapeGato.setFillColor(sf::Color::Red);

		sf::Vector2f positionGato1(1.f, 0.f);
		positionGato1 = BoardToWindows(positionGato1);
		shapeGato.setPosition(positionGato1);

		window.draw(shapeGato);

		sf::Vector2f positionGato2(3.f, 0.f);
		positionGato2 = BoardToWindows(positionGato2);
		shapeGato.setPosition(positionGato2);

		window.draw(shapeGato);

		sf::Vector2f positionGato3(5.f, 0.f);
		positionGato3 = BoardToWindows(positionGato3);
		shapeGato.setPosition(positionGato3);

		window.draw(shapeGato);*/
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





void Connection() {
	sf::Packet packetHELLOE;

	sf::Packet packetLog;
	std::string name;
	std::cout << "Introduce tu nombre" << std::endl;
	std::cin >> name;
	packetLog << name;
	if (socket.send(packetLog, "localhost", 50000) != sf::Socket::Done) {
		std::cout << "ERROR: An error has ocurred when sending a packet" << std::endl;
	}
	packetHELLOE.clear();
}

int main()
{
	old_Connection();
	return 0;
}