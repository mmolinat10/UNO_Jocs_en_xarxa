#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <Player.h>
#include <Card.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#define MAX_MESSAGES 25

enum Commands {
	JoinTable_, ExitTable_, DecideEntryMoney_, EntryMoney_, PlaceBetOrder_, PlaceBet_, GiveInitialCards_, IncorrectBet_, StartPlayerTurn_, AskForCard_, NomoreCards_, DoubleBet_, EndRound_, ChatMSG_
};

sf::IpAddress ip = sf::IpAddress::getLocalAddress();
sf::Socket::Status status;
std::vector<std::string> aMessages;
std::mutex mut;
sf::Packet packetOut;
Player local;

void ReceiveChat(std::string text);
void ReceiveUNO();

// MAIN
int main() {

	// Guardamos el nombre del usuario, nos conectamos al servidor y abrimos el chat
	std::cout << "Por favor introduce tu nombre de usuario: ";
	std::cin >> local.name;
	status = local.sock.connect(ip, 5000, sf::seconds(5.f));

	if (status == sf::Socket::Done) {
		std::cout << "Conectado al Servidor " << ip << "\n";
		packetOut << Commands::JoinTable_ << local.name; ////LOS COMANDS SON PAR EL ENVIO Y RECEPCION DE PACKETS, NO TIENE QUE VER CON EL USUARIO
		local.sock.send(packetOut);
		packetOut.clear();
	}
	else {
		std::cout << "Fallo al conectar con el Servidor " << ip << "\n";
		system("pause");
		exit(0);
	}

	//******************************CHAT*******************************************//
	sf::Vector2i screenDimensions(800, 600);
	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), ("Chat (" + local.name + ")"));

	sf::Font font;
	if (!font.loadFromFile("courbd.ttf")) 
		std::cout << "Can't load the font file" << std::endl;
	
	sf::String mensaje = ">";

	sf::Text chattingText(mensaje, font, 14);
	chattingText.setFillColor(sf::Color(0, 160, 0));
	chattingText.setStyle(sf::Text::Bold);

	sf::Text text(mensaje, font, 14);
	text.setFillColor(sf::Color(0, 160, 0));
	text.setStyle(sf::Text::Bold);
	text.setPosition(0, 560);

	sf::RectangleShape separator(sf::Vector2f(800, 5));
	separator.setFillColor(sf::Color(200, 200, 200, 255));
	separator.setPosition(0, 550);

	// THREAD JUEGO
	std::thread t(&ReceiveUNO); 

	while (window.isOpen()) {

		sf::Event evento;
		while (window.pollEvent(evento)) {

			switch (evento.type) {

				case sf::Event::Closed:
					window.close();
					break;

				case sf::Event::KeyPressed:
					if (evento.key.code == sf::Keyboard::Escape)
						window.close();
					else if (evento.key.code == sf::Keyboard::Return) {
						///////////////////////////CONTROLAMOS LOS MENSAJES DEL CHAT/////////////////////////////////
						//SEND
						mensaje.erase(0, 1);
						if (mensaje == "exit") {
							window.close();
							continue;
						}
						mensaje = local.name + ": " + mensaje;
						std::cout << "a" + mensaje.toAnsiString() << std::endl;
						packetOut << Commands::ChatMSG_ << mensaje.toAnsiString().c_str();
						local.sock.send(packetOut);
						packetOut.clear();
					
						//SEND END
						mensaje = ">";
					}
					break;

				case sf::Event::TextEntered:
					if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
						mensaje += (char)evento.text.unicode;
					else if (evento.text.unicode == 8 && mensaje.getSize() > 1)
						mensaje.erase(mensaje.getSize() - 1, mensaje.getSize());
					break;
			}
		}

		window.draw(separator);
		for (size_t i = 0; i < aMessages.size(); i++) {
			std::string chatting = aMessages[i];
			chattingText.setPosition(sf::Vector2f(0, 20 * i));
			chattingText.setString(chatting);
			window.draw(chattingText);
		}
		std::string mensaje_ = mensaje + "_";
		text.setString(mensaje_);

		window.draw(text);
		window.display();
		window.clear();
	}
	packetOut << Commands::ExitTable_;
	local.sock.send(packetOut);

	// Cleanup
	t.join(); 
	local.sock.disconnect();
	return 0;
}

void ReceiveChat(std::string text) {
	std::cout << text << std::endl;
	aMessages.push_back(text);
	if (aMessages.size() > 25) 
		aMessages.erase(aMessages.begin(), aMessages.begin() + 1);
}

void ReceiveUNO() {
	sf::Packet packetIn;
	std::string strRec;
	int intRec;
	int enumVar;
	Commands com;

	/////////////////////////////////////RECOGEMOS EL COMMAND QUE NOS HA ENVIADO EL SERVER////////////////////////////////////
	do {
		status = local.sock.receive(packetIn);
		packetIn >> enumVar;
		com = (Commands)enumVar;

		switch (com) {

			case DecideEntryMoney_:
				std::cout << "Introduce el dinero inicial de la mesa: ";
				std::cin >> local.money;

				packetOut << Commands::EntryMoney_ << local.money;
				local.sock.send(packetOut);
				break;

			case PlaceBetOrder_:
				std::cout << "Introduce tu apuesta: ";
				std::cin >> local.bet;

				packetOut << Commands::PlaceBet_ << local.bet;
				local.sock.send(packetOut);
				break;

			case IncorrectBet_:
				std::cout << "Error en tu apuesta, dinero insuficiente\nIntroduce tu apuesta: ";
				std::cin >> local.bet;

				packetOut << Commands::PlaceBet_ << local.bet;
				local.sock.send(packetOut);
				break;

			case GiveInitialCards_:
				break;

			case StartPlayerTurn_:
				break;

			case EndRound_:
				break;

			case ChatMSG_:
				packetIn >> strRec;
				ReceiveChat(strRec);
				break;

			default:
				break;
		}
		// Cleanup
		packetIn.clear();
		packetOut.clear();
	} while (status == sf::Socket::Done);
	if (status == sf::Socket::Disconnected) 
		ReceiveChat("Se ha perdido la conexion con el servidor");
}