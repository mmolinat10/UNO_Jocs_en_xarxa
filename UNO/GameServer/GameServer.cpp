#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "Deck.h"

#define MAX_CLIENTS 4
#define MAX_MESSAGES 30
#define DEFAULT_PORT 5000
int userNum = 1;
void Disconnected(std::vector<sf::TcpSocket*>& list, sf::SocketSelector *ss, int userNum);

int main() {

	PlayerInfo playerInfo;

	sf::SocketSelector sSelector;
	//Jugadors llista sockets
	std::vector<sf::TcpSocket*> sList;

	sf::TcpSocket::Status sStatus;
	sf::TcpListener listener;
	sf::TcpListener::Status lStatus;

	char buffer[2000];
	std::string message;
	size_t received;

	lStatus = listener.listen(DEFAULT_PORT);
	sSelector.add(listener); // pass listener to selector

	while (lStatus != sf::TcpListener::Status::Disconnected) {

		// while not disconnected wait for a message from socket
		while (sSelector.wait()) {

			// check if it's listener
			if (sSelector.isReady(listener)) {
				if (sList.size() < 4) {
					sf::TcpSocket* socket = new sf::TcpSocket();
					sf::TcpListener::Status st = listener.accept(*socket);
					if (st == sf::TcpListener::Status::Done) {
						sList.push_back(socket);
					}


					sSelector.add(*socket);

					std::cout << "Se ha conectado el cliente " + std::to_string(userNum) + "!" << std::endl;
					userNum++;
				}
				else {
					message = "Empieza la partida!";
					for (sf::TcpSocket* s : sList)
						s->send(message.c_str(), message.size() + 1);
					
					listener.close();
				}
				
			}

			// despues del mensaje de bienvenida, procedemos a recibir mensajes de clientes i a enviarlo a todos
			else {

				for (int i = 0; i < sList.size(); i++) {

					if (sSelector.isReady(*sList[i])) {

						// Receive
						sStatus = sList[i]->receive(buffer, sizeof(buffer), received);

						if (sStatus == sf::TcpSocket::Status::Done) {
							message = buffer;
							for (int j = 0; j < sList.size(); j++) {
								sf::TcpSocket::Status st = sList[j]->send(message.c_str(), message.size() + 1);

								if (st == sf::TcpSocket::Status::Error)
									std::cout << "Error" << std::endl;
							}
						}
						else if (sStatus == sf::TcpSocket::Status::Disconnected)
							Disconnected(sList, &sSelector, i);
						break;
					}
				}
			}
		}
	}

	// Cleanup
	sSelector.clear();
	listener.close();
	for (sf::TcpSocket* &socket : sList)
		socket->disconnect();
	sList.clear();
	
	return 0;
}

void Disconnected(std::vector<sf::TcpSocket*>& list, sf::SocketSelector *ss, int userNum) {
	ss->remove(*list[userNum]); // remove from ss, because it has disconnected
	list[userNum]->disconnect(); // disconnect

	delete(list[userNum]);
	list.erase(list.begin() + userNum, list.begin() + userNum + 1);

	for (sf::TcpSocket* socket : list) {
		std::string outMsn = "El cliente " + std::to_string(userNum + 1) + " se ha desconectado!";
		socket->send(outMsn.c_str(), outMsn.size() + 1);
	}
	std::cout << "El cliente " + std::to_string(userNum + 1) + " se ha desconectado!" << std::endl;
}