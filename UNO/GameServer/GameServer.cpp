#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <Hand.h>
#include <Card.h>
#include <Deck.h>
#include <iostream>
#include <list>
#include <vector>


enum Commands {
	Inicio_, Exit_, RepartirCartas_, Chat_
};
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

class Player {
public:
	Player() {}
	~Player() {}

	std::string name;

	std::vector<Card> hand;

	sf::TcpSocket* sock;
	Hand myHand;
};

sf::Packet packetIn, packetOut;
sf::TcpListener listener;
sf::SocketSelector selector;
std::vector<Card> deck;
Player jug;
std::vector<Player> players;
int userNum = 1;


void SendAllPlayers(std::string text);

// MAIN
int main() {
	jug.name = "nombreJug";
	bool running = true;
	sf::Socket::Status status = listener.listen(5000);
	if (status != sf::Socket::Done) {
		std::cout << "Error al abrir listener\n";
		exit(0);
	}

	// Add the listener
	selector.add(listener);
	std::cout << "ESTO ES EL JUEGO DEL UNO!" << std::endl << "Esperando a los jugadores..." << std::endl;

	// waits for new connections
	while (running) {

		if (selector.wait()) {

			/////////////////////////////////REBRE CLIENTS PER PRIMERA VEGADA/////////////////////////////////////////////
			if (selector.isReady(listener)) {

				Player newPlayer;
				newPlayer.sock = new sf::TcpSocket;

				if (listener.accept(*newPlayer.sock) == sf::Socket::Done) {
					std::cout << "Se ha conectado el jugador " << userNum << " con puerto: " << newPlayer.sock->getRemotePort() << "!" << std::endl;
					players.push_back(newPlayer);
					selector.add(*newPlayer.sock);
					userNum++;
				}
				else {
					std::cout << "Error al recoger conexion nueva\n";
					delete newPlayer.sock;
				}
			}

			///////////////////////AQUESTA PART ES ON GESTIONEM EL QUE REBEM DEL CLIENT///////////////////////////////////////////////
			else {
				userNum = 0;
				for (std::vector<Player>::iterator it = players.begin(); it != players.end(); it++) {
					userNum++;
					if (selector.isReady(*it->sock)) {

						status = it->sock->receive(packetIn);

						if (status == sf::Socket::Done) {
							std::string strRec;
							int intRec;
							int enumVar;
							Commands com;
							packetIn >> enumVar;
							com = (Commands)enumVar;

							/////////////////////////////AQUI ES ON REBEM ELS PACKETS DEL CLIENT////////////////////////////////////
							switch (com) {

								case Inicio_:
									packetIn >> strRec;
									it->name = strRec;
									SendAllPlayers("El jugador " + std::to_string(userNum) + " " + it->name + " se ha conectado!");

									//Al llegar a 4 jugadores empieza la partida
									//2 para debugar
									if (players.size() >= 4) {
										Deck myDeck;
										for (int i = 0; i < players.size(); i++) {
											// Llenar mano para cada jugador
											players[i].myHand.FillHand(myDeck);
											
											//enviar 7 cartas a la mano del cliente, el deck solo esta en el server!
											//packetOut << Commands::RepartirCartas_ << players[i].myHand;
											packetOut << Commands::RepartirCartas_;
											players[i].sock->send(packetOut);
										}
									}
									break;

								case Chat_:
									packetIn >> strRec;
									SendAllPlayers(strRec);
									break;

								case Exit_:
									SendAllPlayers("El jugador " + it->name + " se ha desconectado!");
									break;

								default:
									break;
							}
							packetOut.clear();
						}
						else if (status == sf::Socket::Disconnected) {
							selector.remove(*it->sock);
							std::cout << "Elimino el socket que se ha desconectado\n";
						}
						else std::cout << "Error al recibir de " << it->sock->getRemotePort() << std::endl;
						packetIn.clear();
					}
				}
			}
		}
	}
}

//Send a message
void SendAllPlayers(std::string text) {
	sf::Packet packet;
	packet << Commands::Chat_ << text;

	for (auto &player : players) {
		player.sock->send(packet);
	}
	packet.clear();
}
