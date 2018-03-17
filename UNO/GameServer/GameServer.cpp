#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
//#include <Player.h>
#include <Card.h>
#include <iostream>
#include <list>
#include <vector>

#define Blackjack 21;
#define CasinoStop 17;

enum Commands {
	JoinTable_, ExitTable_, DecideEntryMoney_, EntryMoney_, PlaceBetOrder_, PlaceBet_, GiveInitialCards_, IncorrectBet_, StartPlayerTurn_, AskForCard_, NomoreCards_, DoubleBet_, EndRound_, ChatMSG_
};

class Player {
public:
	Player() {}
	~Player() {}

	std::string name;
	int money = 20;
	int bet = 0;
	std::vector<int> score;
	std::vector<Card> hand;

	sf::TcpSocket* sock;

	std::string showCards();
	void calculateScore();
	std::string showScore();

};

sf::Packet packetIn, packetOut;
sf::TcpListener listener;
sf::SocketSelector selector;
std::vector<Card> deck;
Player crupier;
std::vector<Player> players;
int initMoney, userNum = 1;

void CreateDeck();
void SendAllPlayers(std::string text);
Card GiveRandomCard();

std::string Player::showCards() {
	std::string text = "El jugador " + name + " tiene:";
	for each (Card card in hand) {
		std::string num;
		switch (card.number) {
		case 0:
			num = "A";
			break;
		case 11:
			num = "J";
			break;
		case 12:
			num = "Q";
			break;
		case 13:
			num = "K";
			break;
		default:
			num = std::to_string(card.number);
			break;
		}
		switch (card.suit) {
		case 0:
			num += " de Diamantes";
			break;
		case 1:
			num += " de Corazones";
			break;
		case 2:
			num += " de Picas";
			break;
		case 3:
			num += " de Treboles";
			break;
		default:
			break;
		}
		text += " " + num;
	}
	return text;
}
void Player::calculateScore() {
	std::vector<int> s;
	s.push_back(0);
	for each (Card card in hand) {
		std::string num;
		switch (card.number) {
		case 1:
			s[0] += card.number;
			for (int i = 0; i < s.size(); i++) s[i] += card.number;
			s.push_back(s[0] + 10);
			break;
		case 11:
		case 12:
		case 13:
			for (int i = 0; i < s.size(); i++) s[i] += 10;
			break;
		default:
			for (int i = 0; i < s.size(); i++) s[i] += card.number;
			break;
		}
	}
	score = s;
}
std::string Player::showScore() {
	std::string text;
	for (int i = 0; i < score.size(); i++) {
		text += "Score(" + std::to_string(i) + "): " + std::to_string(score[i]) + "\n";
	}
	return text;
}

// MAIN
int main() {

	// Crear mazo de la partida y escuchar para recoger los jugadores
	srand(time(NULL));
	CreateDeck();
	crupier.name = "Crupier";
	bool running = true;
	sf::Socket::Status status = listener.listen(5000);
	if (status != sf::Socket::Done) {
		std::cout << "Error al abrir listener\n";
		exit(0);
	}

	// Add the listener to the selector
	selector.add(listener);
	std::cout << "ESTO ES EL JUEGO DEL UNO!" << std::endl << "Esperando a los jugadores..." << std::endl;

	// Endless loop that waits for new connections
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

								case JoinTable_:
									//Si es el primer jugador se le pide la cantidad inicial de dinero de la mesa
									if (players.size() == 1) {
										packetOut << Commands::DecideEntryMoney_;
										it->sock->send(packetOut);
										packetOut.clear();
									}
									packetIn >> strRec;
									it->name = strRec;
									SendAllPlayers("El jugador " + std::to_string(userNum) + " " + it->name + " se ha conectado!");

									//Al llegar a 4 jugadores empieza la partida
									if (players.size() >= 2) {
										for (int i = 0; i < players.size(); i++) {
											players[i].money = initMoney;
											packetOut << Commands::PlaceBetOrder_;
											players[i].sock->send(packetOut);
										}
									}
									break;

								case ChatMSG_:
									packetIn >> strRec;
									SendAllPlayers(strRec);
									break;

								case ExitTable_:
									SendAllPlayers("El jugador " + it->name + " se ha desconectado!");
									break;

								case EntryMoney_:
									packetIn >> intRec;
									initMoney = intRec;
									break;

								case PlaceBet_:
								{
									packetIn >> it->bet;
									std::cout << it->bet << " : " << it->money << std::endl;
									if (it->bet > it->money) {
										packetOut << Commands::IncorrectBet_;
										it->sock->send(packetOut);
										packetOut.clear();
										SendAllPlayers("El jugador " + it->name + " no tiene ni idea!");
									}
									else {
										SendAllPlayers("El jugador " + it->name + " ha apostado: " + std::to_string(it->bet) + "!");
									}
									bool allBetOk = true;
									for (auto &player : players) {
										if (player.bet == 0) allBetOk = false;
									}
									if (allBetOk) {
										for (int i = 0; i < players.size(); i++) {
											players[i].hand.push_back(GiveRandomCard());
											players[i].hand.push_back(GiveRandomCard());
											SendAllPlayers(players[i].showCards());
										}
										crupier.hand.push_back(GiveRandomCard());
										SendAllPlayers(crupier.showCards());
										packetOut << Commands::StartPlayerTurn_;
										players.front().sock->send(packetOut);
										SendAllPlayers(" Es el turno del jugador: " + players.front().name + "\n" + players.front().showCards() + " con puntuacion: " + players.front().showScore());
									}
								}
								break;

								case AskForCard_:
									break;

								case NomoreCards_:
									break;

								case DoubleBet_:
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

void CreateDeck() {
	deck.clear();
	for (int i = 0; i < 4; i++) {
		for (int j = 1; j <= 13; j++) {
			Card newCard;
			newCard.suit = (Card::Suits)i;
			newCard.number = j;
			deck.push_back(newCard);
		}
	}
}

//Send a message to all clients
void SendAllPlayers(std::string text) {
	sf::Packet packet;
	packet << Commands::ChatMSG_ << text;

	for (auto &player : players) {
		player.sock->send(packet);
	}
	packet.clear();
}

Card GiveRandomCard() {
	int pos = rand() % (deck.size() + 1);
	Card card = deck.at(pos);
	deck.erase(deck.begin() + pos);
	return card;
}