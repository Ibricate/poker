#pragma once
#include "Deck.h"
class Game
{
private:
	Deck* deck = new Deck();
	Deck::Card* myHand;
	Deck::Card* yourHand;
public:
	void Start()
	{
		deck->Shuffle();
		deck->Print();

		myHand = deck->Deal();
		std::cout << "My Hand\n";
		for (size_t i = 0; i < 5; i++)
		{
			std::cout << myHand[i].ToString() << "\t";
		}
		std::cout << std::endl;

		yourHand = deck->Deal();
		std::cout << "Your Hand\n";
		for (size_t i = 0; i < 5; i++)
		{
			std::cout << yourHand[i].ToString() << "\t";
		}
		std::cout << std::endl;

	}
};

