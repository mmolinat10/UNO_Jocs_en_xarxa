#include "Card.h"

Card::Card()
{
}

Card::Card(Valid validity) {
	if (validity == INVALID)
	{
		this->validity = INVALID;
	}
	else
	{
		this->validity = VALID;
	}
}

Card::Card(Color color, Rank rank) {
	this->color = color;
	this->rank = rank;
	this->number = -1;
	this->validity = VALID;
}

Card::Card(Color color, int number) {
	this->color = color;
	this->rank = NUMBER;
	this->number = number;
	this->validity = VALID;
}

Card::Card(Color color, int number, Rank rank) {
	this->color = color;
	this->number = number;
	this->rank = rank;
	this->validity = VALID;
}

int Card::ForfeitCost() {
	if (this->rank == SKIP || this->rank == REVERSE || this->rank == DRAW_TWO)
	{
		return 20;
	}
	else if (this->rank == WILD || this->rank == WILD_D4)
	{
		return 50;
	}
	else if (this->rank == NUMBER)
	{
		return this->number;
	}
	else
	{
		return 0;
	}
}

bool Card::CanPlayOn(Card c) {
	if (this->rank == WILD || this->rank == WILD_D4 || this->color == c.color || (this->rank == c.rank && this->rank != NUMBER) || (this->rank == NUMBER &&  c.rank == NUMBER && this->number == c.number))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Card::CanPlayColor(Color c) {
	if (this->color == c || this->color == NONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Card::PrintCard() {
	std::string c;
	if (GetColor() == RED)
	{
		c = "RED";
	}
	else if (GetColor() == BLUE)
	{
		c = "BLUE";
	}
	else if (GetColor() == GREEN)
	{
		c = "GREEN";
	}
	else if (GetColor() == YELLOW)
	{
		c = "YELLOW";
	}
	else if (GetColor() == NONE)
	{
		c = "NONE";
	}

	std::string r;
	if (GetRank() == WILD)
	{
		r = "WILD";
	}
	else if (GetRank() == DRAW_TWO)
	{
		r = "DRAW_TWO";
	}
	else if (GetRank() == WILD_D4)
	{
		r = "WILD_D4";
	}
	else if (GetRank() == NUMBER)
	{
		r = "NUMBER";
	}
	else if (GetRank() == SKIP)
	{
		r = "SKIP";
	}
	else if (GetRank() == REVERSE)
	{
		r = "REVERSE";
	}
	if (r.compare("NUMBER") == 0)
	{
		std::cout << c << " " << GetNumber();
	}
	else if (r.compare("DRAW_TWO") == 0 || r.compare("SKIP") == 0 || r.compare("REVERSE") == 0)
	{
		std::cout << c << " " << r;
	}
	else
	{
		std::cout << r;
	}
}