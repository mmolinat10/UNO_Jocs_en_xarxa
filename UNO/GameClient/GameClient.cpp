#pragma once
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#define MAX_MESSAGES 30
#define DEFAULT_PORT 5000
std::mutex mutex;
void ReceiveUNOGame(sf::TcpSocket* socket, size_t* received);
void UNOGame(sf::TcpSocket::Status socketStatus, sf::TcpSocket* socket, size_t*  received);
void ReceiveChat(sf::TcpSocket* socket, size_t* received, std::vector<std::string>* messages, sf::RenderWindow* window);
void Chat(sf::TcpSocket::Status socketStatus, sf::TcpSocket* socket, size_t*  received);

int main() {

	PlayerInfo playerInfo;
	sf::TcpSocket::Status socketStatus = sf::TcpSocket::Status::NotReady;
	sf::TcpSocket socket;
	//socket.setBlocking(false);

	size_t received;
	sf::IpAddress ip = sf::IpAddress::getLocalAddress(); // ip local
	socket.connect(ip, DEFAULT_PORT);

	UNOGame(socketStatus, &socket, &received);
	//std::thread threadUNOChat(Chat, socketStatus, &socket, &received);
	
	
	//Chat(socketStatus, &socket, &received);
	//UNOGame(socketStatus, &socket, &received);

	// Cleanup
	//threadUNO.join();
	//threadUNOChat.join();

	socket.disconnect();
	
	return 0;
}

void ReceiveUNOGame(sf::TcpSocket* socket, size_t* received) {

	bool endGame = false;
	bool firstTime = true;

	while (!endGame) {
		char buffer[2000];
		sf::Socket::Status status = socket->receive(buffer, sizeof(buffer), *received);
		std::string str = buffer;

		// Conversion de string a lo que necesitamos dividido entre '_'
		int playerReceived = 0;
		int userNum = 0;
		std::string playerNum = "", myTurn = "";
		int newWord = 0;
		if (status == sf::Socket::Status::Done) {
			mutex.lock();

			// La primera vez que el servidor nos envia un mensaje recogemos el numero de cliente que somos
			// para determinar si el mensaje va dirigido a nosotros, es decir, si somos el cliente1
			// solo printaremos aquellos mensajes del cliente1, dejando de lado los mensajes del cliente2,3,4..
			if (firstTime) {
				for (char& c : str) {
					if (c == '_')
						newWord++;

					// Si el mensaje del Server esta entre la primera '_' y la segunda '_'
					if (newWord == 1) {
						userNum = (int)c - 48;
						playerReceived = (int)c - 48;
					}
					// Si el mensaje del Server esta entre la segunda '_' y la tercera '_'
					if (newWord == 2) {
						myTurn += c;
					}
				}
				firstTime = false;
			}
			// Si no es la primera vez, recogemos en un int playerReceived, el usuario al que va dirigido el mensaje
			else {
				for (char& c : str) {
					if (c == '_')
						newWord++;

					if (newWord == 1) {
						playerReceived = (int)c - 48;
					}
					if (newWord == 2) {
						myTurn += c;
					}
				}
				firstTime = false;
			}


			// Solo printamos el mensaje si va dirigido a nosotros
			if (userNum == playerReceived) std::cout << str << std::endl;
			
			//std::cout << "userNum: " << userNum << " playerReceived: " << playerReceived << std::endl;
			mutex.unlock();
		}

	}

}

void UNOGame(sf::TcpSocket::Status socketStatus, sf::TcpSocket* socket, size_t*  received) {
	bool endGame = false;

	std::thread threadUNOReceive(ReceiveUNOGame, socket, received);
	while (!endGame) {
		// Send
		std::string messageStr; 
		std::cin >> messageStr;
		socket->send(messageStr.c_str(), messageStr.size() + 1);
	}
	
	threadUNOReceive.join();
}

void ReceiveChat(sf::TcpSocket* socket, size_t* received, std::vector<std::string>* messages, sf::RenderWindow* window) {

	while (window->isOpen()) {
		char buffer[2000];
		sf::Socket::Status status = socket->receive(buffer, sizeof(buffer), *received);
		std::string str = buffer;

		if (status == sf::Socket::Status::Done) {
			mutex.lock();
			messages->push_back(str);
			if (messages->size() > MAX_MESSAGES)
				messages->erase(messages->begin(), messages->begin() + 1);
			mutex.unlock();
		}
	}
}

void Chat(sf::TcpSocket::Status socketStatus, sf::TcpSocket* socket, size_t*  received) {

	std::vector<std::string> messages;
	sf::Vector2i screenDimensions(800, 600);
	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Cliente");

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

	std::string messageStr;

	// Receive
	std::thread threadChatReceive(ReceiveChat, socket, received, &messages, &window);

	while (window.isOpen())
	{
		sf::Event evento;
		while (window.pollEvent(evento))
		{
			switch (evento.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (evento.key.code == sf::Keyboard::Escape)
					window.close();
				else if (evento.key.code == sf::Keyboard::Return)
				{
					// Send
					messageStr = "Cliente:"; //+ std::to_string(userNum);
					messageStr += mensaje;
					socketStatus = socket->send(messageStr.c_str(), messageStr.size() + 1);

					if (messageStr == ">exit" || messageStr == " >exit") {
						socket->disconnect();
						exit(0);
					}

					if (socketStatus == sf::TcpSocket::Status::Disconnected) {
						messageStr = "Servidor desconectado!";
						messages.push_back(messageStr);
						if (messages.size() > MAX_MESSAGES)
							messages.erase(messages.begin(), messages.begin() + 1);
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
		for (size_t i = 0; i < messages.size(); i++) {
			std::string chatting = messages[i];
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
	threadChatReceive.join();
}