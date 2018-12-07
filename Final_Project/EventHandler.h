#pragma once
#include <Windows.h>
#include <stdio.h>
#include "Main.h"
#include "Receiver.h"

static bool isListening = TRUE;
static LPOVERLAPPED ov;

DWORD WINAPI pollForEvents(LPVOID portHandle);
void stopEventHandlerThrd();