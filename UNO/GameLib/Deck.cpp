#include "Deck.h"

Deck::Deck() {
	Random = rand();
	FillDeck();
	Shuffle();
}

void Deck::FillDeck() {
	for (int i = 1; i <= 9; i++)
	{
		for (int j = 0; j < NUMBER_OF_DUP_REGULAR_CARDS; j++)
		{
			cards.push_back(new Card(RED, i));
			cards.push_back(new Card(YELLOW, i));
			cards.push_back(new Card(BLUE, i));
			cards.push_back(new Card(GREEN, i));
		}
	}

	// There are fewer "0" cards than other numbers.
	for (int j = 0; j < NUMBER_OF_DUP_ZERO_CARDS; j++)
	{
		cards.push_back(new Card(RED, 0));
		cards.push_back(new Card(YELLOW, 0));
		cards.push_back(new Card(BLUE, 0));
		cards.push_back(new Card(GREEN, 0));
	}

	for (int j = 0; j < NUMBER_OF_DUP_SPECIAL_CARDS; j++)
	{
		cards.push_back(new Card(RED, SKIP));
		cards.push_back(new Card(YELLOW, SKIP));
		cards.push_back(new Card(GREEN, SKIP));
		cards.push_back(new Card(BLUE, SKIP));
		cards.push_back(new Card(RED, REVERSE));
		cards.push_back(new Card(YELLOW, REVERSE));
		cards.push_back(new Card(GREEN, REVERSE));
		cards.push_back(new Card(BLUE, REVERSE));
		cards.push_back(new Card(RED, DRAW_TWO));
		cards.push_back(new Card(YELLOW, DRAW_TWO));
		cards.push_back(new Card(GREEN, DRAW_TWO));
		cards.push_back(new Card(BLUE, DRAW_TWO));
	}

	for (int i = 0; i < NUMBER_OF_WILD_CARDS; i++)
	{
		cards.push_back(new Card(NONE, WILD));
	}

	for (int i = 0; i < NUMBER_OF_WILD_D4_CARDS; i++)
	{
		cards.push_back(new Card(NONE, WILD_D4));
	}
}

void Deck::Shuffle() {
	for (int i = 0; i < SHUFFLE_FACTOR * cards.size(); i++)
	{
		std::random_shuffle(cards.begin(), cards.end());
	}
}

Card Deck::Draw() {
	if (cards.size() == 0)
	{
		std::cout << "Empty Deck!!"; 
	}
	Card temp_card = *cards.at(0);
	cards.erase(cards.begin());
	return temp_card;
}

void Deck::Remix() {
	cards.insert(cards.end(), discardedCards.begin(), discardedCards.end());
	discardedCards.clear();
	Shuffle();
}