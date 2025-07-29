#pragma once
#include "timer.h"    

extern Timer timer;

void nfc_setup();
void nfc_loop();
void readDataBlock();
void resetWildCard();
void handleWildCard();
bool isWildCard();
void exec();
bool myHandleWildCard();