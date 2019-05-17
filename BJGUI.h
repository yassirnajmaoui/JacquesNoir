#ifndef BJGUI_H
#define BJGUI_H

#include "includes.h"
class BJGUI : public QMainWindow
{
Q_OBJECT
public:
	explicit BJGUI(QWidget *parent = 0);
	virtual ~BJGUI();
	void setCard(bool topOrBottom, int hindex, int cindex, int card);
	void renderGame(bool force = false);
	void setVisibleHands(bool topOrBottom, int lastHand);
	void messageConsole(QString str);
	void clearConsole();
public slots:
	void hitBtnClicked();
	void standBtnClicked();
	void splitBtnClicked();
	void doubleDBtnClicked();
	void startGame();
	void endGame();
	//void readFPGA();
private:

	QGridLayout* qcentralgrid;
	QWidget* qCentralW;
	QLabel* infoPane;
	QTextEdit* console;
	QPushButton* hitBtn;
	QPushButton* standBtn;
	QPushButton* splitBtn;
	QPushButton* doubleDBtn;

	QLabel*** topCards;
	QLabel*** bottomCards;

	QMenuBar* menuBar;
	QMenu* fichierOpt;
	QAction* newGameOpt;
	QAction* quitOpt;

	QPixmap* allCardsPixmap;
	QPixmap* emojisPixmap;
	int emojiIterator;

	Blackjack bjgame;
	int currentGamehindex;

	//CommunicationFPGA port;
	QTimer fpgaTimer;
	bool statutport;
	int echconv[4];
	char currentPhoneme;
	int phonemeTimer;
};

#endif // BJGUI_H
