#pragma once
#include <array>
#include <string>
#include <vector>
#include "Deck.h"

class Game
{
public:
    void Start();

private:
    Deck deck;
    std::vector<Deck::Card> player;   
    std::vector<Deck::Card> bot;      
    std::vector<Deck::Card> table;   
    int playerMoney = 500;
    int botMoney = 500;
    int pot = 0;

    struct HandResult
    {
        int rank;                    
        std::vector<int> tieBreaker; 
    };

    int CardPower(const Deck::Card& c);
    HandResult Evaluate5(const std::array<Deck::Card, 5>& h);
    HandResult BestHand(const std::vector<Deck::Card>& seven);
    int Compare(const HandResult& a, const HandResult& b);

    void Clear();
    void PrintCards(const std::vector<Deck::Card>& cards, bool hidden = false);
    void PrintTable(int revealed, bool showBot, const std::string& msg);

    void NewRound();
    void DealAll();
    bool BettingRound(const std::string& phase);
    void Showdown();
    bool PlayAgain();

    int AskInt(const std::string& prompt, int lo, int hi);
    bool AskYN(const std::string& prompt);
};
