#pragma once
#include <Windows.h>
#include <stdio.h>
#include "Main.h"

static bool isListening = false;

DWORD WINAPI pollForEvents(LPVOID portHandle);
void stopEventHandlerThrd();