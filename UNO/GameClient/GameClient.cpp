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

enum Command { Inicio_, Exit_, RepartirCartas_, Chat_ };
enum Color { RED, YELLOW, GREEN, BLUE, NONE };
enum Rank { NUMBER, SKIP, REVERSE, DRAW_TWO, WILD, WILD_D4 };
enum Valid { VALID, INVALID };

sf::Packet& operator <<(sf::Packet& packet, const Color& c) {
	return packet << c;
};

sf::Packet& operator >>(sf::Packet& packet, Rank& r) {
	return packet >> r;
}

sf::Packet& operator <<(sf::Packet& packet, const Rank& r) {
	return packet << r;
}

sf::Packet& operator >>(sf::Packet& packet, Valid& v) {
	return packet >> v;
}

sf::Packet& operator <<(sf::Packet& packet, const Valid& v) {
	return packet << v;
}

sf::Packet& operator >>(sf::Packet& packet, Color& c) {
	return packet >> c;
}

sf::Packet& operator <<(sf::Packet& packet, const Card& c) {
	return packet << c.color << c.number << c.rank << c.validity;
}

sf::Packet& operator >>(sf::Packet& packet, Card& c) {
	return packet >> c;
}

sf::Packet& operator <<(sf::Packet& packet, const Hand& h) {
	for (int i = 0; i < h.cards.size(); i++) {
		packet << h.cards[i];
	}
	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, Hand& h) {
	for (int i = 0; i < h.cards.size(); i++) {
		packet >> h.cards[i];
	}
	return packet;
}

sf::IpAddress ip = sf::IpAddress::getLocalAddress();
sf::Socket::Status status;
std::vector<std::string> aMessages;
std::mutex mut;
sf::Packet packetSend;
Player local;

void ReceiveChat(std::string text);
void ReceiveUNO();

// MAIN
int main() {

	// Guardamos el nombre del usuario, nos conectamos al servidor y abrimos el chat
	std::cout << "Nombre de usuario: ";
	std::cin >> local.name;
	status = local.sock.connect(ip, 5000, sf::seconds(5.f));

	if (status == sf::Socket::Done) {
		std::cout << "Conectado al server!" << std::endl;
		packetSend << Command::Inicio_ << local.name; ////LOS COMANDS SON PAR EL ENVIO Y RECEPCION DE PACKETS, NO TIENE QUE VER CON EL USUARIO
		local.sock.send(packetSend);
		packetSend.clear();
	}
	else {
		std::cout << "Fallo al conectar al server! " << std::endl;
		system("pause");
		exit(0);
	}

	sf::Vector2i screenDimensions(800, 600);
	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), ("Chat de " + local.name));

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
						// SEND
						mensaje.erase(0, 1);
						if (mensaje == "exit") {
							window.close();
							continue;
						}
						mensaje = local.name + ": " + mensaje;
						std::cout << mensaje.toAnsiString() << std::endl;
						packetSend << Command::Chat_ << mensaje.toAnsiString().c_str();
						local.sock.send(packetSend);
						packetSend.clear();
					
						// SEND END
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

	packetSend << Command::Exit_;
	local.sock.send(packetSend);

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
	int intRec, enumVar;
	sf::Packet packetReceive, packetHand;
	Command command;
	std::string receiveStr;

	/////////////////////////////////////RECOGEMOS EL COMMAND QUE NOS HA ENVIADO EL SERVER////////////////////////////////////
	do {
		status = local.sock.receive(packetReceive);
		packetReceive >> enumVar;
		command = (Command)enumVar;

		switch (command) {

			case RepartirCartas_:
				local.sock.receive(packetHand);
				packetHand >> local.myHand;
				local.myHand.DisplayHand();
				//... a continuar
				break;

			case Chat_:
				packetReceive >> receiveStr;
				ReceiveChat(receiveStr);
				break;

			default:
				break;
		}

		// Clean
		packetReceive.clear();
		packetSend.clear();

	} while (status == sf::Socket::Done);

	if (status == sf::Socket::Disconnected) 
		ReceiveChat("Se ha perdido la conexion con el servidor!\n");
}