#pragma once
#include <Windows.h>
#include <stdio.h>
#include "Main.h"
#include "Receiver.h"

static DWORD RECEIVE_TIMEOUT_TIME_S = 5;
static bool isListening = false;
static time_t lastFrameReceived = time(0);

DWORD WINAPI pollForEvents(LPVOID portHandle);
void stopEventHandlerThrd();