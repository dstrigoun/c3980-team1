#pragma once
#include <Windows.h>

typedef struct ReadThreadParams {
	HANDLE hComm;
	HANDLE stopThreadEvent;
	HWND* hwnd;
	LPDWORD numBytesRead;

	ReadThreadParams(HANDLE hCom, HANDLE stopThreadEvent, LPDWORD numBytesRead, HWND* hwnd) : hComm(hCom), stopThreadEvent(stopThreadEvent), numBytesRead(numBytesRead), hwnd(hwnd) {}
}READTHREADPARAMS, *PREADTHREADPARAMS;