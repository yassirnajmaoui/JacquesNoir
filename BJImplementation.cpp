#include "BJGUI.h"
int main( int argc, char ** argv )
{
	QApplication app(argc, argv);
	
	Blackjack bjgame;

	BJGUI gui;
	gui.show();
	
	return app.exec();
}
