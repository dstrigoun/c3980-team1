#pragma once
#include <Windows.h>
#include <stdio.h>
#include "Main.h"
#include "Receiver.h"
#include "ReadThreadParams.h"

//static DWORD RECEIVE_TIMEOUT_TIME_S = 5;
//static bool isListening = false;
//static time_t lastFrameReceived = time(0);
class Receiver;

DWORD WINAPI pollForEvents(LPVOID portHandle);