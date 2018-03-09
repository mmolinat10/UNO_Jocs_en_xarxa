#pragma once
#include "Card.h"
#include <vector>
#include <random>

class Deck : public Card
{

private:
	std::vector <Card*> cards; /* array of cards */
	std::vector <Card*> discardedCards;
	int Random = rand();

public:
	int NUMBER_OF_DUP_REGULAR_CARDS = 2;
	int NUMBER_OF_DUP_ZERO_CARDS = 1;
	int NUMBER_OF_DUP_SPECIAL_CARDS = 2;
	int NUMBER_OF_WILD_CARDS = 4;
	int NUMBER_OF_WILD_D4_CARDS = 4;
	int SHUFFLE_FACTOR = 1;

	/* constructs a new deck */
	Deck();

	/* fills decks according to above information regarding number of each type of card */
	void FillDeck();

	int NextInt(int size){return (int)rand() % size;}

	/* shuffles deck as long as it is partially filled */
	void Shuffle();

	bool IsEmpty(){return cards.size() == 0;}

	long Size(){return cards.size();}

	Card Draw();

	void Discard(Card c){discardedCards.push_back(&c);}

	void Remix();

	/* how to specify that an array is being returned */
	std::vector<Card *> getDiscardedCards() {return discardedCards;}
};
