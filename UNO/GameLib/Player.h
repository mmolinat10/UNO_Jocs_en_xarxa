#pragma once
#include <SFML\Network.hpp>
#include "Card.h"

class Player {
public:
	// Constructor
	Player() {}
	~Player(){}

	// Methods
	std::string showCards();
	void calculateScore();
	std::string showScore();

	// Variables
	std::string name;
	int money = 20, bet = 0;
	std::vector<int> score;
	std::vector<Card> hand;
	sf::TcpSocket sock;
	sf::TcpSocket* sock2;
};
