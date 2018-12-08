#pragma once
#include <Windows.h>
#include <stdio.h>
#include "Main.h"
#include "Receiver.h"

static bool isListening = TRUE;
static LPOVERLAPPED ov;

DWORD WINAPI pollForEvents(LPVOID portHandle);
void stopEventHandlerThrd();

typedef struct ReadThreadParams {
	HANDLE hComm;
	HANDLE stopThreadEvent;
	//HDC hdc;
	HWND* hwnd;
	LPDWORD numBytesRead;

	ReadThreadParams(HANDLE hCom, HANDLE stopThreadEvent, LPDWORD numBytesRead, HWND* hwnd) : hComm(hCom), stopThreadEvent(stopThreadEvent), numBytesRead(numBytesRead), hwnd(hwnd) {}
}READTHREADPARAMS, *PREADTHREADPARAMS;