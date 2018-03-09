#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#define MAX_CLIENTS 4

struct Direction {
	std::string ip;
	unsigned short port;

	Direction::Direction() {}
	Direction::Direction(std::string ip, unsigned short port) {
		this->ip = ip;
		this->port = port;
	}
};

int main() {

	PlayerInfo playerInfo;

	// Listener that will wait for the 4 clients (corresponding port)
	sf::TcpListener listener;
	listener.listen(50000);
	std::vector<Direction> dList;

	// Wait for connections
	for (int i = 0; i < MAX_CLIENTS; i++) {

		sf::TcpSocket socket;
		sf::Packet packet;
		listener.accept(socket);

		// Number of packets it will receive
		int size = dList.size();
		std::cout << "Numero total de clientes: " << size + 1 << std::endl;
		packet << size;
		socket.send(packet);

		if (i != 0) {
			// Send for each
			for (int j = 0; j < dList.size(); j++) {
				packet.clear();
				packet << dList[j].ip << dList[j].port;
				socket.send(packet);
			}
		}

		Direction direction((socket.getRemoteAddress()).toString(), socket.getRemotePort());
		dList.push_back(direction); // Add it to our vector

		socket.disconnect();
	}

	// Cleanup
	listener.close();
	return 0;
}