#pragma once
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#define MAX_MESSAGES 30

void Receive(std::vector<sf::TcpSocket*> socket, std::vector<std::string>* messages, sf::SocketSelector* ss) {

	std::string msn;
	size_t recived;
	char buffer[2000];

	while (ss->wait()) {

		for (int i = 0; i < socket.size(); i++) {

			if (ss->isReady(*socket[i])) {

				sf::Socket::Status st = socket[i]->receive(buffer, sizeof(buffer), recived);

				if (st == sf::Socket::Status::Done) {
					msn = buffer;
					messages->push_back(msn);

					if (messages->size() > 25)
						messages->erase(messages->begin(), messages->begin() + 1);
				}
			}
		}
	}
}

int main() {

	PlayerInfo playerInfo;

	// Variables
	sf::TcpSocket socket;
	sf::TcpListener listener;
	sf::SocketSelector ss;
	sf::Socket::Status socketStatus;
	sf::Packet packet;
	std::vector<Direction> directionList;
	std::vector<sf::TcpSocket*> socketList;

	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	socket.connect(ip, 50000);

	socketStatus = socket.receive(packet);
	if (socketStatus != sf::TcpSocket::Status::Done)
		std::cout << "Error" << std::endl;
	else {

		int size;
		packet >> size;

		if (size != 0) {

			for (int i = 0; i < size; i++) {

				//Receive packets
				socketStatus = socket.receive(packet);

				if (socketStatus != sf::TcpSocket::Status::Done)
					std::cout << "Error";
				else {
					Direction direction;
					packet >> direction.ip >> direction.port;
					directionList.push_back(direction);
				}
			}

			// For each direction
			for (Direction i : directionList) {

				sf::TcpSocket* newSocket = new sf::TcpSocket();
				newSocket->connect(i.ip, i.port);

				ss.add(*newSocket);
				socketList.push_back(newSocket);
			}
		}

		std::cout << "Se ha conectado el cliente " << size + 1 << "/4" << std::endl;
	}

	int portLocal = socket.getLocalPort();

	// Free
	socket.disconnect();
	listener.listen(portLocal);

	for (int i = socketList.size(); i < 3; i++) {
		sf::TcpSocket* s = new sf::TcpSocket();

		listener.accept(*s);
		socketList.push_back(s);
		ss.add(*s);
	}

	listener.close();
	std::vector<std::string> aMensajes;

	sf::Vector2i screenDimensions(800, 600);

	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Chat");

	sf::Font font;
	if (!font.loadFromFile("courbd.ttf"))
		std::cout << "Can't load the font file" << std::endl;

	sf::String mensaje = " >";

	sf::Text chattingText(mensaje, font, 24);
	chattingText.setFillColor(sf::Color(0, 160, 0));
	chattingText.setStyle(sf::Text::Bold);

	sf::Text text(mensaje, font, 24);
	text.setFillColor(sf::Color(0, 160, 0));
	text.setStyle(sf::Text::Bold);
	text.setPosition(0, 560);

	sf::RectangleShape separator(sf::Vector2f(800, 5));
	separator.setFillColor(sf::Color(200, 200, 200, 255));
	separator.setPosition(0, 550);

	std::string msn;

	// Thread
	std::thread t(Receive, socketList, &aMensajes, &ss);

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
				else if (evento.key.code == sf::Keyboard::Return)
				{
					aMensajes.push_back(mensaje);
					if (aMensajes.size() > MAX_MESSAGES) {
						aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
					}
					// SEND
					msn = mensaje;

					for (sf::TcpSocket* s : socketList) {
						socketStatus = s->send(msn.c_str(), msn.size() + 1);
						if (socketStatus != sf::TcpSocket::Status::Done)
							std::cout << "Error" << std::endl;
					}

					mensaje = ">";
				}
				break;
			case sf::Event::TextEntered:
				if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
					mensaje += (char)evento.text.unicode;
				else if (evento.text.unicode == 8 && mensaje.getSize() > 0)
					mensaje.erase(mensaje.getSize() - 1, mensaje.getSize());
				break;
			}
		}

		window.draw(separator);
		for (size_t i = 0; i < aMensajes.size(); i++) {
			std::string chatting = aMensajes[i];
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

	// Cleanup
	ss.clear();
	t.join();
	socket.disconnect();
	return 0;
}