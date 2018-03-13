#pragma once
#include <SFML\Graphics.hpp>

struct Direction {
	std::string ip;
	unsigned short port;

	Direction::Direction() {}
	Direction::Direction(std::string ip, unsigned short port) {
		this->ip = ip;
		this->port = port;
	}
};

class PlayerInfo
{
	std::string name;
	sf::Vector2i position;
	int lives;
public:
	PlayerInfo();
	~PlayerInfo();
};