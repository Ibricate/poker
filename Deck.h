#pragma once
#include <string>
#include <vector>
#include <random>

class Deck
{
public:
    enum Value { six, seven, eight, nine, ten, jack, queen, king, ace };
    enum Suit { Hearts, Diamonds, Spades, Clubs };

    struct Card
    {
        Value v;
        Suit  s;

        std::string RankStr() const
        {
            const char* names[] = { "6","7","8","9","10","J","Q","K","A" };
            return names[v];
        }

        std::string SuitStr() const
        {
            const char* names[] = { "H","D","S","C" };
            return names[s];
        }

        bool IsRed() const { return s == Hearts || s == Diamonds; }
 
        std::string ColorSuit() const
        {
            std::string sym = SuitStr();
            if (IsRed()) return "\33[31m" + sym + "\33[0m";
            return "\33[37m" + sym + "\33[0m";
        }

         std::vector<std::string> Lines() const
        {
            std::string rank = RankStr();
            while (rank.size() < 2) rank += " ";
            return {
                ".-----.",
                "| " + rank + " " + ColorSuit() + " |",
                "'-----'"
            };
        }

        static std::vector<std::string> HiddenLines()
        {
            return { ".-----.", "| ### |", "'-----'" };
        }
    };

    Deck() { Reset(); }

    void Reset()
    {
        cards.clear();
        top = 0;
        for (int v = 0; v < 9; v++)
            for (int s = 0; s < 4; s++)
                cards.push_back({ (Value)v, (Suit)s });
    }

    void Shuffle()
    {
        Reset();
        static std::mt19937 rng(std::random_device{}());
        for (int i = 0; i < (int)cards.size(); i++)
        {
            int j = std::uniform_int_distribution<int>(0, (int)cards.size() - 1)(rng);
            std::swap(cards[i], cards[j]);
        }
    }

    Card Deal() { return cards[top++]; }
    int Left() const { return (int)cards.size() - top; }

private:
    std::vector<Card> cards;
    int top = 0;
};
