#include "Hand.h"

void Hand::DisplayHand() {
	cards[0].PrintCard();
	for (int i = 1; i < Size(); i++)
	{
		std::cout << ", ";
		cards[i].PrintCard();
	}
}

bool Hand::IsIn(Card card) {
	for (int i = 0; i < Size(); i++)
	{
		if (card.GetColor() == cards[i].GetColor())
		{
			if (card.GetRank() == cards[i].GetRank())
			{
				if (card.GetNumber() == cards[i].GetNumber())
				{
					return true;
				}
			}
		}
	}
	return false;
}

void Hand::Remove(Card card) {
	for (int i = 0; i < Size(); i++)
	{
		if (card.GetColor() == cards[i].GetColor())
		{
			if (card.GetRank() == cards[i].GetRank())
			{
				if (card.GetNumber() == cards[i].GetNumber())
				{
					cards.erase(cards.begin() + i);
					break;
				}
			}
		}
	}
}

int Hand::CountCards() {
	int total = 0;
	for (int i = 0; i < cards.size(); i++)
	{
		total += cards.at(i).ForfeitCost();
	}
	return total;
}

void Hand::FillHand(Deck deck) {
	for (int i = 0; i < INIT_HAND_SIZE; i++)
	{
		this->AddCard(deck.Draw());
	}
}

