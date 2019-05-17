#include "BJGUI.h"


BJGUI::BJGUI(QWidget *parent) : QMainWindow(parent), currentGamehindex(0), statutport(true)
{
	this->setWindowTitle("Jacques Noir - Le jeu");
	allCardsPixmap = new QPixmap[DECKSIZE + 1];
	for (int i = 0; i < DECKSIZE + 1; i++)
	{
		allCardsPixmap[i] = QPixmap(QString("./assets/") + QString::number(i) + QString(".png"));
	}
	emojisPixmap = new QPixmap[NUMBER_OF_HAPPYEMOJIS + NUMBER_OF_SADEMOJIS + NUMBER_OF_PUSHEMOJIS];
	//Setting up emojis
	for (int i = 0; i < NUMBER_OF_HAPPYEMOJIS + NUMBER_OF_SADEMOJIS + NUMBER_OF_PUSHEMOJIS; i++)
	{
		if (i < NUMBER_OF_HAPPYEMOJIS)
			emojisPixmap[i] = QPixmap(QString("assets/happyemoji") + QString::number(i) + QString(".png"));
		else if (i < NUMBER_OF_SADEMOJIS + NUMBER_OF_HAPPYEMOJIS && i >= NUMBER_OF_HAPPYEMOJIS)
			emojisPixmap[i] = QPixmap(QString("assets/sademoji") + QString::number(i%NUMBER_OF_SADEMOJIS) + QString(".png"));
		else if (i > NUMBER_OF_SADEMOJIS + NUMBER_OF_HAPPYEMOJIS - 1)
			emojisPixmap[i] = QPixmap(QString("assets/pushface") + QString::number(i%NUMBER_OF_PUSHEMOJIS) + QString(".png"));
	}
	emojiIterator = 0;
	//fpgaTimer = QTimer(this);
	fpgaTimer.setInterval(FPGA_TIMEOUT);
	connect(&fpgaTimer, SIGNAL(timeout()), this, SLOT(readFPGA()));
	fpgaTimer.start();

	//bjgame = Blackjack();

	menuBar = new QMenuBar(this);
	fichierOpt = new QMenu("Jeu", menuBar);
	newGameOpt = new QAction("Nouvelle Partie", fichierOpt);
	newGameOpt->setShortcut(QKeySequence(Qt::Key_N));
	quitOpt = new QAction("Quitter", fichierOpt);
	quitOpt->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	fichierOpt->addAction(newGameOpt);
	fichierOpt->addAction(quitOpt);
	menuBar->addMenu(fichierOpt);
	this->setMenuBar(menuBar);


	qCentralW = new QWidget(this);
	qcentralgrid = new QGridLayout(qCentralW);

	QWidget* topPane = new QWidget(this);
	QGridLayout* topPaneLayout = new QGridLayout(topPane);
	// ALL SHIT TO PANE LAYOUT TOP
	topCards = new QLabel**[MAXHANDS];
	for (int i = 0; i < MAXHANDS; ++i)
	{
		topCards[i] = new QLabel*[HANDSIZE];
		for (int j = 0; j < HANDSIZE; j++)
		{
			topCards[i][j] = new QLabel(topPane);
			topCards[i][j]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			setCard(true, i, j, 0);
			topPaneLayout->addWidget(topCards[i][j], i, j);
		}
	}

	QWidget* middlePane = new QWidget(this);
	QGridLayout* middlePaneLayout = new QGridLayout(middlePane);
	// ALL SHIT TO PANE LAYOUT MIDDLE
	infoPane = new QLabel(middlePane);
	infoPane->setStyleSheet("color:white;");
	console = new QTextEdit(middlePane);
	console->setReadOnly(true);
	console->setStyleSheet("color:white;background-color:#115511;");
	hitBtn = new QPushButton("Hit", middlePane);
	//hitBtn->setShortcut(QKeySequence(Qt::Key_H));
	hitBtn->setStyleSheet("color:white;");
	standBtn = new QPushButton("Stand", middlePane);
	//standBtn->setShortcut(QKeySequence(Qt::Key_S));
	standBtn->setStyleSheet("color:white;");
	splitBtn = new QPushButton("Split", middlePane);
	//splitBtn->setShortcut(QKeySequence(Qt::Key_P));
	splitBtn->setStyleSheet("color:white;");
	doubleDBtn = new QPushButton("Double Down", middlePane);
	//doubleDBtn->setShortcut(QKeySequence(Qt::Key_D));
	doubleDBtn->setStyleSheet("color:white;");
	middlePaneLayout->addWidget(infoPane, 0, 0);
	middlePaneLayout->addWidget(console, 0, 1, 1, 3);
	middlePaneLayout->addWidget(hitBtn, 1, 0);
	middlePaneLayout->addWidget(doubleDBtn, 1, 1);
	middlePaneLayout->addWidget(standBtn, 1, 2);
	middlePaneLayout->addWidget(splitBtn, 1, 3);


	QWidget* bottomPane = new QWidget(this);
	QGridLayout* bottomPaneLayout = new QGridLayout(bottomPane);
	// ALL SHIT TO PANE LAYOUT BOTTOM
	bottomCards = new QLabel**[MAXHANDS];
	for (int i = 0; i < MAXHANDS; ++i)
	{
		bottomCards[i] = new QLabel*[HANDSIZE];
		for (int j = 0; j < HANDSIZE; j++)
		{
			bottomCards[i][j] = new QLabel(bottomPane);
			bottomCards[i][j]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			setCard(false, i, j, 0);
			bottomPaneLayout->addWidget(bottomCards[i][j], i, j);
		}
	}

	qcentralgrid->addWidget(topPane, 0, 0);
	qcentralgrid->addWidget(middlePane, 1, 0);
	qcentralgrid->addWidget(bottomPane, 2, 0);


	qCentralW->setStyleSheet(QString("background-color:") + BACKGROUND_COLOR + QString(";"));
	this->setCentralWidget(qCentralW);


	connect(hitBtn, SIGNAL(clicked()), this, SLOT(hitBtnClicked()));
	connect(doubleDBtn, SIGNAL(clicked()), this, SLOT(doubleDBtnClicked()));
	connect(standBtn, SIGNAL(clicked()), this, SLOT(standBtnClicked()));
	connect(splitBtn, SIGNAL(clicked()), this, SLOT(splitBtnClicked()));
	connect(newGameOpt, SIGNAL(triggered()), this, SLOT(startGame()));
	connect(quitOpt, SIGNAL(triggered()), this, SLOT(close()));

	renderGame();
	this->setFixedWidth(this->size().width() + 5);
}


void BJGUI::setCard(bool topOrBottom, int hindex, int cindex, int card)
{
	if (topOrBottom)
	{
		topCards[hindex][cindex]->setPixmap(allCardsPixmap[card]);
	}
	else
	{
		bottomCards[hindex][cindex]->setPixmap(allCardsPixmap[card]);
	}
}

void BJGUI::setVisibleHands(bool topOrBottom, int lastHand)
{
	if (topOrBottom)
	{
		for (int i = 0; i <= lastHand; i++)
		{
			for (int j = 0; j < HANDSIZE; j++)
			{
				topCards[i][j]->show();
			}
		}
		for (int i = lastHand + 1; i < MAXHANDS; i++)
		{
			for (int j = 0; j < HANDSIZE; j++)
			{
				topCards[i][j]->hide();
			}
		}
	}
	else
	{
		for (int i = 0; i <= lastHand; i++)
		{
			for (int j = 0; j < HANDSIZE; j++)
			{
				bottomCards[i][j]->show();
				if (i - currentGamehindex == 0 && bjgame.getPlayerCard(i, j) != 0)
				{
					bottomCards[i][j]->setStyleSheet("QLabel { border: 1px solid black; padding: 2px 2px 2px 2px; border-radius: 4px}");
				}
				else
				{
					bottomCards[i][j]->setStyleSheet(QString("QLabel {}"));
				}

			}
		}
		for (int i = lastHand + 1; i < MAXHANDS; i++)
		{
			for (int j = 0; j < HANDSIZE; j++)
			{
				bottomCards[i][j]->hide();
			}
		}
	}
}

void BJGUI::messageConsole(QString str)
{
	console->setPlainText(console->toPlainText() + str);
}

void BJGUI::clearConsole()
{
	console->setPlainText("");
}

void BJGUI::hitBtnClicked()
{
	if (bjgame.isGamePlaying())
	{
		bjgame.playerHit(currentGamehindex);

		if (bjgame.hasPlayerBusted(currentGamehindex))
		{
			messageConsole(QString("You busted in hand #") + QString::number(currentGamehindex));
			currentGamehindex += 1;
			if (bjgame.isHandEmpty(currentGamehindex))
			{
				currentGamehindex -= 1;
				endGame();
				return;
			}
		}
		renderGame();
	}
}

void BJGUI::doubleDBtnClicked()
{
	if (bjgame.isGamePlaying())
	{
		uint8_t doubleDResult = bjgame.playerDoubleDown(currentGamehindex);

		if (doubleDResult == INVALID)
		{
			messageConsole(QString("You couldn't Double down because you lacked money or you didn't have exactly two cards.\n"));
			return;
		}
		currentGamehindex += 1;
		if (bjgame.isHandEmpty(currentGamehindex))
		{
			currentGamehindex -= 1;
			endGame();
			return;
		}
		renderGame();
	}
}

void BJGUI::standBtnClicked()
{
	if (bjgame.isGamePlaying())
	{
		bjgame.playerStands(currentGamehindex);
		currentGamehindex++;

		if (bjgame.isHandEmpty(currentGamehindex))
		{
			currentGamehindex -= 1;
			endGame();
			return;
		}
		renderGame();
	}
}

void BJGUI::splitBtnClicked()
{
	if (bjgame.isGamePlaying())
	{
		uint8_t splittingResult = bjgame.playerSplit(currentGamehindex);
		if (splittingResult == INVALID)
		{
			console->setPlainText(console->toPlainText() + QString("\nYou couldn\'t split because you did not have a pair or you are not actually playing a game"));
		}
		renderGame();
	}
}

void BJGUI::startGame()
{
	bool ok;
	int bet = QInputDialog::getInt(this, ("Starting a game"),
		("You have " + QString::number(bjgame.getPlayerBalance()) + "$\nHow much do you want to bet to start the game ? :"), bjgame.getPlayerBalance() / 2, 0, bjgame.getPlayerBalance(), 1, &ok);

	if (!ok) return;
	bool beginningGame = bjgame.beginGame(bet);
	if (!beginningGame)
	{
		return;
	}
	clearConsole();
	currentGamehindex = 0;
	renderGame();
}

void BJGUI::endGame()
{
	QMessageBox msgBox(this);
	int moneyLostOrWon = bjgame.getPlayerBalance();
	for (int hindex = 0; hindex < MAXHANDS; hindex++)
	{
		if (bjgame.isHandEmpty(hindex)) break;
		moneyLostOrWon += bjgame.getHandBet(hindex);
	}
	uint16_t gameResult = bjgame.finishGame();
	moneyLostOrWon = bjgame.getPlayerBalance() - moneyLostOrWon;
	renderGame(true);

	QString msg("\n");

	if (!bjgame.hasPlayerBusted(currentGamehindex))
		msg += QString("Dealer\'s hand is worth ") + QString::number(bjgame.getDealerBJValue()) + QString("\n");

	for (int hindex = 0; hindex < MAXHANDS; hindex++)
	{
		uint8_t handResult = bjgame.getGameStateAtHindex(hindex, gameResult);
		if (handResult == WIN)
		{
			msg += "You won with hand #" + QString::number(hindex) + "\n";
		}
		else if (handResult == LOST)
		{
			msg += "You lost with hand #" + QString::number(hindex) + "\n";
		}
		else if (handResult == PUSH)
		{
			msg += "Hand #" + QString::number(hindex) + " was a push\n";
		}
		else if (handResult == NOGAME)
		{
			//msg += "\n";
			break;
		}
	}

	msg += "You can start a new game.";

	if (moneyLostOrWon < 0)
	{
		msgBox.setText("You lost money!");
		msgBox.setInformativeText("You lost " + QString::number(0 - moneyLostOrWon) + "$");
		msgBox.setIconPixmap(emojisPixmap[NUMBER_OF_HAPPYEMOJIS + (emojiIterator%NUMBER_OF_SADEMOJIS)]);
	}
	else if (moneyLostOrWon > 0)
	{
		msgBox.setText("You won money!");
		msgBox.setInformativeText("You won " + QString::number(moneyLostOrWon) + "$");
		msgBox.setIconPixmap(emojisPixmap[(emojiIterator%NUMBER_OF_HAPPYEMOJIS)]);
	}
	else
	{
		msgBox.setText("You didn't lose nor win anything!");
		msgBox.setInformativeText("So we can say that you won " + QString::number(moneyLostOrWon) + "$! Congratulations");
		msgBox.setIconPixmap(emojisPixmap[NUMBER_OF_HAPPYEMOJIS + NUMBER_OF_SADEMOJIS + (emojiIterator%NUMBER_OF_PUSHEMOJIS)]);
	}
	emojiIterator++;
	console->setPlainText(console->toPlainText() + msg);

	msgBox.setStandardButtons(QMessageBox::Close | QMessageBox::Retry);
	msgBox.setDefaultButton(QMessageBox::Retry);
	int ret = msgBox.exec();

	switch (ret) {
	case QMessageBox::Close:
		// Will close the dialog, so no big deal there
		break;
	case QMessageBox::Retry:
		startGame();
		break;
	default:
		// should never be reached
		break;
	}

}

void BJGUI::renderGame(bool force)
{
	if (!(bjgame.isGamePlaying()) && force == false)
	{
		this->setVisibleHands(false, 0);
		this->setVisibleHands(true, 0);
		return;
	}

	for (int i = 0; i < HANDSIZE; i++)
	{
		setCard(true, 0, i, bjgame.getDealerCard(i));
	}

	int visHands = 0;
	for (int i = 0; i < MAXHANDS; i++)
	{
		if (bjgame.isHandEmpty(i))
		{
			visHands = i - 1;
			break;
		}
		for (int j = 0; j < HANDSIZE; j++)
		{
			int cardid = bjgame.getPlayerCard(i, j);
			setCard(false, i, j, cardid);
		}
	}
	this->setVisibleHands(false, visHands);
	this->setVisibleHands(true, 0);

	infoPane->setText(QString("You posess ") + QString::number(bjgame.getPlayerBalance()) + QString("$\n") + QString("Current hand (hand #") + QString::number(currentGamehindex) + QString(") is worth: ") + QString::number(bjgame.getPlayerBJValue(currentGamehindex)));
}
/*
void BJGUI::readFPGA()
{

	if (statutport) statutport = port.lireRegistre(nreg_lect_can0, echconv[0]);       // lecture canal 0
	else { qDebug() << "Erreur du port nreg_lect_can0" << endl; return; }
	if (statutport) statutport = port.lireRegistre(nreg_lect_can1, echconv[1]);       // lecture canal 1
	else { qDebug() << "Erreur du port nreg_lect_can1" << endl; return; }
	if (statutport) statutport = port.lireRegistre(nreg_lect_can2, echconv[2]);       // lecture canal 2
	else { qDebug() << "Erreur du port nreg_lect_can2" << endl; return; }
	if (statutport) statutport = port.lireRegistre(nreg_lect_can3, echconv[3]);       // lecture canal 3
	else { qDebug() << "Erreur du port nreg_lect_can3" << endl; return; }
	if (!statutport) {
		qDebug() << "Erreur du port dans la boucle" << endl;
	}

	for (int indice_canal_a_afficher = 0; indice_canal_a_afficher < 4; indice_canal_a_afficher++)
	{
		if ((int)echconv[indice_canal_a_afficher] == 0) qDebug() << ",     0x00";
		else qDebug() << ",    " << (int)echconv[indice_canal_a_afficher];
	}

	bool phonemeChange = false;
	if (currentPhoneme != 'a' && PHONEME_A_CONDITION)
	{
		currentPhoneme = 'a';
		phonemeTimer = FPGA_TIMEOUT;
		phonemeChange = true;
	}
	else if (currentPhoneme != 'e' && PHONEME_E_CONDITION)
	{
		currentPhoneme = 'e';
		phonemeTimer = FPGA_TIMEOUT;
		phonemeChange = true;
	}
	else if (currentPhoneme != 'i' && PHONEME_I_CONDITION)
	{
		currentPhoneme = 'i';
		phonemeTimer = FPGA_TIMEOUT;
		phonemeChange = true;
	}
	else if (currentPhoneme != 'u' && PHONEME_U_CONDITION)
	{
		currentPhoneme = 'u';
		phonemeTimer = FPGA_TIMEOUT;
		phonemeChange = true;
	}

	if (!phonemeChange)
	{
		if (PHONEME_A_CONDITION || PHONEME_E_CONDITION || PHONEME_I_CONDITION || PHONEME_U_CONDITION)
		{
			phonemeTimer += FPGA_TIMEOUT;
			if (phonemeTimer > DUREE_PHONEME_MIN && phonemeTimer < DUREE_PHONEME_MAX)
			{
				if (currentPhoneme == 'a') hitBtnClicked();
				else if (currentPhoneme == 'e') standBtnClicked();
				else if (currentPhoneme == 'i') splitBtnClicked();
				else if (currentPhoneme == 'u') doubleDBtnClicked();
			}
		}
	}

}*/

BJGUI::~BJGUI()
{
	//delete bjgame;
	delete[] allCardsPixmap;
	delete[] emojisPixmap;
}
