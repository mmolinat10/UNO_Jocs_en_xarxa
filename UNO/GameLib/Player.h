#pragma once
#include <SFML\Network.hpp>
#include "Card.h"
#include "Hand.h"

class Player {
public:
	// Constructor
	Player() {}
	~Player(){}

	// Variables
	std::string name;
	std::vector<Card> hand;
	sf::TcpSocket sock;
	sf::TcpSocket* sock2;
	Hand myHand;
};
