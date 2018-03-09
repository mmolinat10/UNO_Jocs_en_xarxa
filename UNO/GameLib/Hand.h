#pragma once
#include <iostream>

#include <sstream>
#include <vector>
#include "Card.h"
#include "Deck.h"

class Hand
{
private:
	std::vector <Card> cards;
	int INIT_HAND_SIZE = 7;
public:

	Hand(Deck deck)
	{
		FillHand(deck);
	}

	void AddCard(Card c){cards.push_back(c);}

	long Size(){return cards.size();}

	void DisplayHand();

	bool IsIn(Card card);

	void Remove(Card card);

	bool IsEmpty(){return cards.size() == 0;}

	int CountCards();

	std::vector <Card> GetCards() {return this->cards;}

	void FillHand(Deck deck);
};
