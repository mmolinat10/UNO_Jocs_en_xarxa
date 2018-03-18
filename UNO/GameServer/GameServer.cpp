#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <Hand.h>
#include <Card.h>
#include <Deck.h>
#include <iostream>
#include <list>
#include <vector>

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

class Player {
public:
	Player() {}
	~Player() {}

	std::string name;
	std::vector<Card> hand;
	sf::TcpSocket* sock;
	Hand myHand;
};

void SendAllPlayers(std::string text);

sf::Packet packetReceive, packetSend;
sf::TcpListener listener;
sf::SocketSelector ss;
std::vector<Card> deck;
Player jug;
std::vector<Player> aPlayers;
int userNum = 1;

// MAIN
int main() {

	jug.name = "nombreJug";

	sf::Socket::Status status = listener.listen(5000);

	if (status != sf::Socket::Done) {
		std::cout << "Error al abrir listener" << std::endl;
		exit(0);
	}

	ss.add(listener);
	std::cout << "ESTO ES EL JUEGO DEL UNO!" << std::endl << "Esperando a que se conecten los jugadores..." << std::endl;

	bool notEnd = true;

	while (notEnd) {

		if (ss.wait()) {

			/////////////////////////////////REBRE CLIENTS PER PRIMERA VEGADA/////////////////////////////////////////////
			if (ss.isReady(listener)) {

				Player newPlayer;
				newPlayer.sock = new sf::TcpSocket;

				if (listener.accept(*newPlayer.sock) == sf::Socket::Done) {
					std::cout << "Se ha conectado el jugador " << userNum << "!" << std::endl;
					aPlayers.push_back(newPlayer);
					ss.add(*newPlayer.sock);
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

				for (std::vector<Player>::iterator it = aPlayers.begin(); it != aPlayers.end(); it++) {
					userNum++;
					if (ss.isReady(*it->sock)) {

						status = it->sock->receive(packetReceive);

						if (status == sf::Socket::Done) {
							std::string receiveStr;
							int receiveInt, enumVar;
							Command command;
							packetReceive >> enumVar;
							command = (Command)enumVar;

							/////////////////////////////AQUI ES ON REBEM ELS PACKETS DEL CLIENT////////////////////////////////////
							switch (command) {

								case Inicio_:
									packetReceive >> receiveStr;
									it->name = receiveStr;
									SendAllPlayers("El jugador " + std::to_string(userNum) + " " + it->name + " se ha conectado!");

									//Al llegar a 4 jugadores empieza la partida 
									if (aPlayers.size() >= 4) {
										Deck myDeck;
										for (int i = 0; i < aPlayers.size(); i++) {
											// Llenar mano para cada jugador
											aPlayers[i].myHand.FillHand(myDeck);
											//aPlayers[i].myHand.DisplayHand();
											//enviar 7 cartas a la mano del cliente, el deck solo esta en el server!
											//packetOut << Commands::RepartirCartas_ << players[i].myHand;
											packetSend << Command::RepartirCartas_;
											aPlayers[i].sock->send(packetSend);
										}
									}
									break;

								case Chat_:
									packetReceive >> receiveStr;
									SendAllPlayers(receiveStr);
									break;

								case Exit_:
									SendAllPlayers("El jugador " + it->name + " se ha desconectado!");
									break;

								default:
									break;
							}
							packetSend.clear();
						}
						else if (status == sf::Socket::Disconnected) {
							ss.remove(*it->sock);
							std::cout << "Se ha eliminado el socket que se ha desconectado" << std::endl;
						}
						else std::cout << "Error al recibir " <<  std::endl;
						packetReceive.clear();
					}
				}
			}
		}
	}
}

void SendAllPlayers(std::string text) {
	sf::Packet packet;
	packet << Command::Chat_ << text;

	for (auto &player : aPlayers) {
		player.sock->send(packet);
	}
	packet.clear();
}
