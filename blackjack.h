#ifndef BLACKJACK_H
#define BLACKJACK_H

#include "includes_BJ.h"

class Hand
{
public:
	Hand() : bet(0)
	{
		for (int i = 0; i < HANDSIZE; i++) cards[i] = 0;
	}
public:
	uint8_t cards[HANDSIZE];
	int bet;
};

class Blackjack
{
public:
	Blackjack(int startingMoney = BEGINMONEY);

	inline uint8_t getHandValue(Hand h) const;
	inline uint8_t getCardValue(uint8_t card) const;
	uint8_t getGameStateAtHindex(uint8_t hindex, uint16_t gameState) const;
	inline void resetHand(Hand& h);

	uint8_t getPlayerBJValue(int hindex = 0) const; // Gives how much the player's hand is worth, specify hand with the hindex argument
	uint8_t getDealerBJValue() const; // Gives how much the dealer's hand is worth
	uint8_t playerHit(int hindex = 0);
	uint8_t playerStands(int hindex = 0);
	uint8_t playerSplit(int hindex = 0);
	uint8_t playerDoubleDown(int hindex = 0);

	uint8_t getPlayerCard(int hindex, int cindex)const { return this->playerHands[hindex].cards[cindex]; }
	uint8_t getDealerCard(int cindex)const { return this->dealerHand.cards[cindex]; }
	int getPlayerBalance()const { return this->playerMoney; }
	bool canSplit(int hindex = 0)const;

	inline void clearGameAndShuffle(); // Sets the taken array to all falses, resets the random seed
	bool isHandEmpty(Hand h)const;
	bool isHandEmpty(int hindex)const;
	uint8_t pickFromDeck();
	bool beginGame(int bet);
	uint16_t finishGame();
	bool hasPlayerBusted(int hindex = 0);
	bool isGamePlaying() { return isPlaying; }

	int getHandBet(int hindex = 0);

	std::ostream& display(std::ostream& os) const;
	std::ostream& displayCard(std::ostream& os, uint8_t card) const;

private:
	inline bool addCard(Hand& h, uint8_t card);
	inline void playerWin(int hindex = 0);
	inline void playerPush(int hindex = 0);
	inline void playerLost(int hindex = 0);

private:
	Hand playerHands[MAXHANDS];
	int playerMoney;
	bool taken[DECKSIZE];
	Hand dealerHand;
	bool isPlaying;
};
#endif //BLACKJACK_H
