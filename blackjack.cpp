#include "blackjack.h"


Blackjack::Blackjack(int startingMoney)
{
	playerMoney = startingMoney;

	this->isPlaying = false;
}

bool Blackjack::beginGame(int b)
{
	this->isPlaying = true;

	if (this->playerMoney < b) return false;

	this->clearGameAndShuffle();

	playerMoney -= b;
	playerHands[0].bet = b;

	uint8_t tmpcard = pickFromDeck();
	addCard(playerHands[0], tmpcard);
	tmpcard = pickFromDeck();
	addCard(playerHands[0], tmpcard);

	tmpcard = pickFromDeck();
	addCard(dealerHand, tmpcard);

	return true;
}
inline uint8_t Blackjack::getCardValue(uint8_t card) const
{
	if (card > 52) return 0;
	if (card == 0) return 0;
	uint8_t digit = card % 13;
	if (digit >= 10 || digit == 0) return 10;
	return digit;
}
inline bool Blackjack::addCard(Hand& h, uint8_t card)
{
	if (!isPlaying) return false;
	if (card == 0) return false;
	for (int i = 0; i < HANDSIZE; i++)
	{
		if (h.cards[i] == 0)
		{
			h.cards[i] = card;
			return true;
		}
	}
	return false;
}
int Blackjack::getHandBet(int hindex)
{
	return playerHands[hindex].bet;
}
inline uint8_t Blackjack::getHandValue(Hand h) const
{
	int ace_counter = 1;

	for (int i = 0; i < HANDSIZE; i++) if (h.cards[i] != 0 && getCardValue(h.cards[i]) == 1) ++ace_counter;
	uint8_t* sum = new uint8_t[ace_counter];

	for (int i = 0; i < ace_counter; i++)
	{
		sum[i] = i * 10;
		for (int j = 0; j < HANDSIZE; j++)
		{
			if (h.cards[j] != 0)
				sum[i] += getCardValue(h.cards[j]);
		}
	}

	int closest_distance = -1;
	int chosen = 0;
	for (int i = 0; i < ace_counter; i++)
	{
		if (sum[i] <= 21)
		{
			int distance = 21 - sum[i];
			if (closest_distance > distance || closest_distance == -1)
			{
				closest_distance = distance;
				chosen = i;
			}
		}
	}
	if (sum[chosen] > 21) return 0;
	int toReturn = sum[chosen];
	delete [] sum;
	return toReturn;
}
uint8_t Blackjack::getGameStateAtHindex(uint8_t hindex, uint16_t gameState) const
{
	return (((0b11 << (hindex * 2)) & gameState) >> (hindex * 2));
}
uint8_t Blackjack::getPlayerBJValue(int hindex) const
{
	return this->getHandValue(this->playerHands[hindex]);
}
uint8_t Blackjack::getDealerBJValue() const
{
	return this->getHandValue(this->dealerHand);
}
uint8_t Blackjack::playerHit(int hindex)
{
	if (!isPlaying) return INVALID;
	addCard(playerHands[hindex], pickFromDeck());
	return CONTINUE;
}
uint8_t Blackjack::playerDoubleDown(int hindex)
{
	if (!isPlaying) return INVALID;
	if (getPlayerCard(hindex, 2) != 0) return INVALID;
	if (playerMoney < playerHands[hindex].bet) return INVALID;
	addCard(playerHands[hindex], pickFromDeck());
	playerMoney -= playerHands[hindex].bet;
	return CONTINUE;
}
uint8_t Blackjack::playerStands(int hindex)
{
	if (!isPlaying) return INVALID;
	return CONTINUE;
}
uint8_t Blackjack::playerSplit(int hindex)
{
	if (!isPlaying) return INVALID;

	if (!canSplit(hindex)) return INVALID;

	int howManyHands;
	for (howManyHands = 0; howManyHands < MAXHANDS; ++howManyHands)
		if (playerHands[howManyHands].cards[0] == 0) break;

	Hand newHand;
	resetHand(newHand);
	newHand.cards[0] = playerHands[hindex].cards[1];
	newHand.bet = playerHands[hindex].bet;
	playerMoney -= playerHands[hindex].bet;
	playerHands[hindex].cards[1] = 0;

	playerHands[howManyHands] = newHand;

	return CONTINUE;
}
inline void Blackjack::playerWin(int hindex)
{
	if (!isPlaying) return;
	this->playerMoney += this->playerHands[hindex].bet * 2;
}
inline void Blackjack::playerPush(int hindex)
{
	if (!isPlaying) return;
	this->playerMoney += this->playerHands[hindex].bet;
}
inline void Blackjack::playerLost(int hindex)
{
	if (!isPlaying) return;
	//Nothing happens as he already paid
}
inline void Blackjack::clearGameAndShuffle()
{
	srand(time(NULL));
	// GET ALL CARDS
	for (int i = 0; i < DECKSIZE; i++)
	{
		taken[i] = false;
	}
	// REMOVE ALL CARDS FROM PLAYER
	for (int i = 0; i < MAXHANDS; i++)
	{
		for (int j = 0; j < HANDSIZE; j++)
		{
			playerHands[i].cards[j] = 0;
			playerHands[i].bet = 0;
		}
	}
	//REMOVE CARDS FROM DEALER
	for (int i = 0; i < HANDSIZE; i++)
	{
		dealerHand.cards[i] = 0;
	}
}
uint16_t Blackjack::finishGame()
{
	if (!isPlaying) return (uint8_t)INVALID;

	uint16_t gameResult = 0;


	//Dealer's turn
	bool tooManyCards = false;
	addCard(dealerHand, pickFromDeck());
	uint8_t dealerTotal;

	while (true)
	{
		dealerTotal = getDealerBJValue();
		if (dealerTotal >= DEALER_STAND_THRESHOLD || dealerTotal == 0 || tooManyCards)
		{
			break;
		}
		else {
			//dealer hits
			if (!addCard(dealerHand, pickFromDeck()))
				tooManyCards = true;
			continue;
		}
	}


	//for all hands
	for (int hindex = 0; hindex < MAXHANDS; hindex++)
	{
		if (isHandEmpty(playerHands[hindex])) break;

		uint8_t playerTotal = getPlayerBJValue(hindex);
		if (playerTotal == 21)
		{
			playerWin(hindex);
			gameResult |= WIN << hindex * 2;
			continue;
		}
		if (playerTotal == 0)
		{
			playerLost(hindex);
			gameResult |= LOST << hindex * 2;
			continue;
		}

		//dealer stands
		//Verify if player won
		if (dealerTotal < playerTotal)
		{
			playerWin(hindex);
			gameResult |= WIN << hindex * 2;
		}
		else if (dealerTotal > playerTotal)
		{
			playerLost(hindex);
			gameResult |= LOST << hindex * 2;
		}
		else
		{
			playerPush(hindex);
			gameResult |= PUSH << hindex * 2;
		}
	}
	this->isPlaying = false;
	return gameResult;
}
bool Blackjack::hasPlayerBusted(int hindex)
{
	return (getPlayerBJValue(hindex) == 0 && isHandEmpty(hindex) == false);
}
inline void Blackjack::resetHand(Hand& h)
{
	for (int i = 0; i < HANDSIZE; i++)
		h.cards[i] = 0;
}
bool Blackjack::isHandEmpty(Hand h)const
{
	if (h.cards[0] == 0) return true;
	return false;
}
bool Blackjack::isHandEmpty(int hindex)const
{
	if (playerHands[hindex].cards[0] == 0) return true;
	return false;
}
bool Blackjack::canSplit(int hindex)const
{
	if ((getCardValue(this->playerHands[hindex].cards[0]) != getCardValue(this->playerHands[hindex].cards[1])) || getCardValue(playerHands[hindex].cards[2]) != 0) return false;
	//std::cout << "I can split ?" << std::endl;
	if (!isPlaying) return false;
	// IF YOU HAVE SPLITTED TOO MUCH
	int howManySplits;
	for (howManySplits = 0; howManySplits < MAXHANDS; ++howManySplits)
		if (playerHands[howManySplits].cards[0] == 0) break;
	if (howManySplits >= 7) return false;

	if (playerMoney < playerHands[hindex].bet) return false;

	return true;
}
uint8_t Blackjack::pickFromDeck()
{
	if (!isPlaying) return 0;
	//return 14;
	int picked = 0;
	bool alreadyTaken = false;
	do {
		picked = rand() % 52;
		if (taken[picked] == false)
		{
			alreadyTaken = false;
			taken[picked] = true;
			break;
		}
		if (taken[picked] == true)
		{
			alreadyTaken = true;
		}
	} while (alreadyTaken);
	uint8_t card = (uint8_t)picked + 1;
	//std::cout << +card << std::endl;
	return card;


}

std::ostream& Blackjack::display(std::ostream& os) const
{

	os << "Dealer : ";
	// this->getDealer().display(os)
	for (int i = 0; i < HANDSIZE; i++)
	{
		if (dealerHand.cards[i] != 0)
			displayCard(os, dealerHand.cards[i]) << " ";
		//os << +getCardValue(dealerHand.cards[i]) << " ";
	}
	os << "\tworth: " << +getDealerBJValue() << "\nPlayer :\n";
	//this->getPlayer().display(os)
	for (int hindex = 0; hindex < MAXHANDS; hindex++)
	{
		os << "Hand #" << hindex << ": ";
		for (int cindex = 0; cindex < HANDSIZE; cindex++)
		{
			if (this->playerHands[hindex].cards[cindex] != 0)
				displayCard(os, this->playerHands[hindex].cards[cindex]) << " ";
			//os << +getCardValue(this->playerHands[hindex].cards[cindex]) << " ";
			else
			{
				break;
			}
		}
		os << "\tworth: " << +getPlayerBJValue(hindex);
		if (getPlayerCard(hindex + 1, 0))
			os << std::endl;
		else break;
	}
	os << "\nYou have: " << playerMoney << "$" << std::endl;
	return os;
}
std::ostream& Blackjack::displayCard(std::ostream& os, uint8_t card) const
{
	if (card > 52 || card <= 0) return os;
	uint8_t digit = (card % 13);
	uint8_t shape = (card / 13);
	if (digit == 0) os << 'K';
	else if (digit == 12) os << 'Q';
	else if (digit == 11) os << 'J';
	else if (digit == 1)  os << 'A';
	else os << +digit;
	if (shape == 0) os << "Ca";
	else if (shape == 1) os << "Co";
	else if (shape == 2) os << "Pi";
	else if (shape == 3) os << "Tr";
	return os;
}

