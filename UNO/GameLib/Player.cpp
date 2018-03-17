#include "Player.h"
#include <random>

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