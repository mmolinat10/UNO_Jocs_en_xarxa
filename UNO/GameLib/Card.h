#pragma once
#include <iostream>
#include <sstream>

class Card {
	
protected:
	
public:
	enum Color { RED, YELLOW, GREEN, BLUE, NONE };
	enum Rank { NUMBER, SKIP, REVERSE, DRAW_TWO, WILD, WILD_D4 };
	enum Valid { VALID, INVALID };

	Color color;
	Rank rank;
	Valid validity;
	int number;
	Card();
	Card(Valid validity);


	/* non-number cards */
	Card(Color color, Rank rank);
	

	/* number cards */
	Card(Color color, int number);

	Card(Color color, int number, Rank rank);

	/* cost that ending the game with this card would force upon the player */
	int ForfeitCost();

	/* checks to see if the card is a valid card to be played */
	bool CanPlayOn(Card c);

	bool CanPlayColor(Color c);

	Valid GetValidity(){return this->validity;}

	/* returns color of the card */
	Color GetColor(){return this->color;}

	/* returns rank of the card */
	Rank GetRank(){return this->rank;}

	/* returns number of the card (-1 for non-nuber cards */
	int GetNumber(){return this->number;}

	/* method to print information about the card */
	void PrintCard();
};

