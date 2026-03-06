#pragma once
#include <iostream>
class Deck
{
public:
    enum ValueCard
    {
        six, seven, eight, nine, ten, jack, queen, king, ace
    };
    enum SuitCard
    {
        herts, diamonds, spades, clubs
    };

    struct Card
    {
        ValueCard vc;
        SuitCard sc;
        std::string ToString()
        {
            std::string str;
            switch (vc)
            {
            case Deck::six:
                str += "6";
                break;
            case Deck::seven:
                str += "7";
                break;
            case Deck::eight:
                str += "8";
                break;
            case Deck::nine:
                str += "9";
                break;
            case Deck::ten:
                str += "10";
                break;
            case Deck::jack:
                str += "J";
                break;
            case Deck::queen:
                str += "Q";
                break;
            case Deck::king:
                str += "K";
                break;
            case Deck::ace:
                str += "A"; 
                break;
            }
            switch (sc)
            {
            case Deck::herts:
                str += ((char)(3));
                break;
            case Deck::diamonds:
                str += ((char)(4));
                break;
            case Deck::spades:
                str += ((char)(6));
                break;
            case Deck::clubs:
                str += ((char)(5));
                break;
            }
            return str;
        }
    };
private:
    Card* deck;
    Card* myHand;
    int cardPointer = 0;
public:
    Deck()
    {
        myHand = nullptr;
        deck = new Card[36];
        for (size_t i = 0; i < 9; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                deck[i + j * 4].vc = (ValueCard)i;
                deck[i + j * 4].sc = (SuitCard)j;
            }
        }
    }

    void Shuffle()
    {
        srand(time(NULL));
        for (size_t i = 0; i < 37; i++)
        {
            int index = rand() % 37;
            Card temp = deck[i];
            deck[i] = deck[index];
            deck[index] = temp;
        }
    }

    void Print()
    {
        for (size_t i = 0; i < 9; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                std::cout << deck[i * 9 + j].ToString() << "\t";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    Card* Deal()
    {
        Card* hand = new Card[5];
        for (size_t i = 0; i < 5; i++)
        {
            hand[i] = deck[i + cardPointer];
        }
        cardPointer = 5;
        return hand;

    }
};
