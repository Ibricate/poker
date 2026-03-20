#include "Game.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

void Game::Clear()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Game::PrintCards(const std::vector<Deck::Card>& cards, bool hidden)
{
    std::vector<std::vector<std::string>> rows;
    for (auto& c : cards)
        rows.push_back(hidden ? Deck::Card::HiddenLines() : c.Lines());

    for (int line = 0; line < 3; line++)
    {
        for (auto& r : rows)
            std::cout << r[line] << " ";
        std::cout << "\n";
    }
}

void Game::PrintTable(int revealed, bool showBot, const std::string& msg)
{
    Clear();
    std::cout << "=== Покер ===\n";
    std::cout << "Вы: " << playerMoney << " | Бот: " << botMoney
        << " | Банк: " << pot << "\n\n";

    std::cout << "Стол:\n";
    std::vector<Deck::Card> visible(table.begin(), table.begin() + revealed);
    std::vector<Deck::Card> hidden(table.begin() + revealed, table.end());
    PrintCards(visible);
    if (!hidden.empty()) PrintCards(hidden, true);

    std::cout << "\n";

    std::cout << "Бот:\n";
    PrintCards(bot, !showBot);
    std::cout << "\n";

    std::cout << "Вы:\n";
    PrintCards(player);
    std::cout << "\n";

    if (!msg.empty())
        std::cout << msg << "\n\n";
}


int Game::AskInt(const std::string& prompt, int lo, int hi)
{
    while (true)
    {
        std::cout << prompt;
        int n;
        if (std::cin >> n && n >= lo && n <= hi)
            return n;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Введите число от " << lo << " до " << hi << "\n";
    }
}

bool Game::AskYN(const std::string& prompt)
{
    while (true)
    {
        std::cout << prompt << " (y/n): ";
        char c;
        std::cin >> c;
        if (c == 'y' || c == 'Y') return true;
        if (c == 'n' || c == 'N') return false;
    }
}


void Game::DealAll()
{
    player.clear(); bot.clear(); table.clear();
    player.push_back(deck.Deal()); player.push_back(deck.Deal());
    bot.push_back(deck.Deal()); bot.push_back(deck.Deal());
    for (int i = 0; i < 5; i++) table.push_back(deck.Deal());
}

void Game::NewRound()
{
    pot = 0;
    deck.Shuffle();
    DealAll();
}

bool Game::BettingRound(const std::string& phase)
{
    PrintTable((phase == "preflop") ? 0 : 5,
        false,
        "--- " + phase + " ---");

    const int minBet = 10;

    std::cout << "1 - Поставить  2 - Пас\n";
    int choice = AskInt("Ваш выбор: ", 1, 2);
    if (choice == 2)
    {
        botMoney += pot;
        PrintTable(5, false, "Вы сбросили карты. Бот забирает банк.");
        return false;
    }

    int maxBet = std::min<int>(playerMoney, 200);
    int bet = AskInt("Ставка (" + std::to_string(minBet) + "-"
        + std::to_string(maxBet) + "): ",
        minBet, maxBet);
    playerMoney -= bet;
    pot += bet;

    int botBet = std::min<int>(bet, botMoney);
    botMoney -= botBet;
    pot += botBet;

    std::cout << "Бот уравнивает " << botBet << ".\n";
    return true;
}

void Game::Showdown()
{
    PrintTable(5, true, "--- Вскрытие ---");

    std::vector<Deck::Card> pCards = player;
    pCards.insert(pCards.end(), table.begin(), table.end());

    std::vector<Deck::Card> bCards = bot;
    bCards.insert(bCards.end(), table.begin(), table.end());

    HandResult pr = BestHand(pCards);
    HandResult br = BestHand(bCards);
    int cmp = Compare(pr, br);

    std::string result;
    if (cmp > 0)
    {
        playerMoney += pot;
        result = "Вы выиграли банк " + std::to_string(pot) + "!";
    }
    else if (cmp < 0)
    {
        botMoney += pot;
        result = "Бот выиграл банк " + std::to_string(pot) + ".";
    }
    else
    {
        playerMoney += pot / 2;
        botMoney += pot / 2;
        result = "Ничья! Банк поделён.";
    }
    pot = 0;

    PrintTable(5, true, result);
    std::cin.ignore(10000, '\n');
    std::cin.get(); 
}

bool Game::PlayAgain()
{
    return AskYN("Сыграть ещё раз?");
}

void Game::Start()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    while (true)
    {
        if (playerMoney < 10)
        {
            std::cout << "У вас кончились деньги. Игра окончена.\n";
            break;
        }
        if (botMoney < 10)
        {
            std::cout << "У бота кончились деньги. Вы победили!\n";
            break;
        }

        NewRound();

        if (!BettingRound("preflop")) { if (!PlayAgain()) break; continue; }

        if (!BettingRound("postflop")) { if (!PlayAgain()) break; continue; }

        Showdown();

        if (!PlayAgain()) break;
    }
}


int Game::CardPower(const Deck::Card& c) { return (int)c.v; }


Game::HandResult Game::Evaluate5(const std::array<Deck::Card, 5>& h)
{
    std::vector<int> vals;
    for (auto& c : h) vals.push_back(CardPower(c));
    std::sort(vals.rbegin(), vals.rend());

    bool flush = true;
    for (int i = 1; i < 5; i++)
        if (h[i].s != h[0].s) { flush = false; break; }

    bool straight = (vals[0] - vals[4] == 4 &&
        std::adjacent_find(vals.begin(), vals.end()) == vals.end());

    std::vector<int> counts(9, 0);
    for (int v : vals) counts[v]++;
    std::vector<int> groups;
    for (int c : counts) if (c > 0) groups.push_back(c);
    std::sort(groups.rbegin(), groups.rend());

    HandResult r;
    r.tieBreaker = vals;

    if (straight && flush) { r.rank = 8; return r; }
    if (groups[0] == 4) { r.rank = 7; return r; }
    if (groups[0] == 3 && groups[1] == 2) { r.rank = 6; return r; }
    if (flush) { r.rank = 5; return r; }
    if (straight) { r.rank = 4; return r; }
    if (groups[0] == 3) { r.rank = 3; return r; }
    if (groups[0] == 2 && groups[1] == 2) { r.rank = 2; return r; }
    if (groups[0] == 2) { r.rank = 1; return r; }
    r.rank = 0;
    return r;
}

Game::HandResult Game::BestHand(const std::vector<Deck::Card>& seven)
{
    HandResult best{ -1, {} };
    for (int i = 0; i < 7; i++)
        for (int j = i + 1; j < 7; j++)
        {
            std::array<Deck::Card, 5> hand;
            int k = 0;
            for (int x = 0; x < 7; x++)
                if (x != i && x != j) hand[k++] = seven[x];
            HandResult hr = Evaluate5(hand);
            if (Compare(hr, best) > 0) best = hr;
        }
    return best;
}

int Game::Compare(const HandResult& a, const HandResult& b)
{
    if (a.rank != b.rank) return a.rank - b.rank;
    for (int i = 0; i < (int)std::min<size_t>(a.tieBreaker.size(), b.tieBreaker.size()); i++)
    {
        if (a.tieBreaker[i] != b.tieBreaker[i])
            return a.tieBreaker[i] - b.tieBreaker[i];
    }
    return 0;
}
