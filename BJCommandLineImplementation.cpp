#include "blackjack.h"
using namespace std;

int main()
{

	Blackjack h4 = Blackjack();
	bool playing = true;
	while(playing)
	{
		cout << "You have " << h4.getPlayerBalance() << ", how much would you like to bet to play? >";
		unsigned int bet = 0;
		cin >> bet;
		if(!h4.beginGame(bet))
		{
			cout << "You ain't got all that money, try again" << std::endl;
			return 0;
		}
		cout << "- Beginning of the game -"<<endl;
		h4.display(cout)<<endl;
		
		bool finished = false;
		
		while(!finished)
		{
			cout << "Would you like to hit or stand ? [H/s]>";
			uint8_t answer='H';
			cin >> answer;
			if(answer == 's' || answer=='S')
			{
				uint8_t standanswer = h4.playerStands();
				if(standanswer == WIN)
				{
					cout << "Bro you won! #*-\\*|\\*-#" << endl;
				}
				else if(standanswer == LOST)
				{
					cout << "Bro you lost :'-(" << endl;
				}
				else if(standanswer == PUSH)
				{
					cout << "It's a damn push" << endl;
				}
				finished = true;
			}
			else if(answer=='H'||answer=='h')
			{
				uint8_t hitanswer = h4.playerHit();
				if(hitanswer == WIN)
				{
					cout << "Bro you won!" << endl;
					finished = true;
				}
				else if(hitanswer == LOST)
				{
					cout << "Bro you busted" << endl;
					finished = true;
				}
				else if(hitanswer == CONTINUE)
				{
					cout << "Nothing interesting" << endl;
				}
					
			}else{
				cout << "Bro you ain\'t listenin\' my firend !" << std::endl;
				return 0;
			}
			h4.display(cout);
		}
		
		cout << "Wanna play again my friend[Y/n]? >";
		uint8_t answer = 0;
		cin >> answer;
		playing = (answer == 'y' || answer == 'Y');
	}

	return 0;
}



