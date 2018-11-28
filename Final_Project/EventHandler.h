#pragma once
#include <Windows.h>
#include <stdio.h>
#include "Main.h"
#include "Receiver.h"

static bool isListening = false;

DWORD WINAPI pollForEvents(LPVOID portHandle);
void stopEventHandlerThrd();