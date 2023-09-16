#ifndef INCLUDES_BJ_H
#define INCLUDES_BJ_H

#include <iostream>
#include <cstdlib>
#include <ctime>

#define HANDSIZE 8 // has to be lower than 25
#define BEGINMONEY 400 // default amount of money the player starts with
#define DEALER_STAND_THRESHOLD 17 // The dealer has to stand if his total is 17 or more, otherwise he hits
#define MAXHANDS 8
#define DECKSIZE 52
#define NUMBER_OF_HAPPYEMOJIS 3
#define NUMBER_OF_SADEMOJIS 3
#define NUMBER_OF_PUSHEMOJIS 1 // Order of pixmap array: happy, sad, push
#define WIN 0b01
#define LOST 0b10
#define PUSH 0b11
#define NOGAME 0b00
#define CONTINUE 'c'
#define INVALID 'x'
#define BACKGROUND_COLOR QString("#338833")
#define FPGA_TIMEOUT 500 // en ms
#define PHONEME_A_CONDITION (echconv[0]>115&&echconv[1]>100&&echconv[2]<10&&echconv[3]<10)
#define PHONEME_E_CONDITION (echconv[0]>115&&echconv[1]>100&&echconv[3]>39)
#define PHONEME_I_CONDITION (echconv[0]<100&&echconv[1]>100&&echconv[2]<77)
#define PHONEME_U_CONDITION (echconv[0]<100&&echconv[1]>100&&echconv[2]>155)
#define DUREE_PHONEME_MIN 1000 // en ms
#define DUREE_PHONEME_MAX 2000 // en ms

// REGISTRES
unsigned const int nreg_lect_can0 = 3;      // fpga -> PC  canal 0 lus FPGA -> PC
unsigned const int nreg_lect_can1 = 4;      // fpga -> PC  canal 1 lus FPGA -> PC
unsigned const int nreg_lect_can2 = 5;      // fpga -> PC  canal 2 lus FPGA -> PC
unsigned const int nreg_lect_can3 = 6;      // fpga -> PC  canal 3 lus FPGA -> PC

#include "blackjack.h"

#endif
