TEMPLATE     = app
TARGET       = BlackJackGUI
CONFIG      += warn_on qt debug windows console
HEADERS     += BJGUI.h blackjack.h includes.h 
SOURCES     += BJImplementation.cpp BJGUI.cpp blackjack.cpp


QT += widgets
