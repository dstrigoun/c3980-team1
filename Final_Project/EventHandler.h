#pragma once
#include <Windows.h>
#include <stdio.h>
#include "Main.h"
#include "Receiver.h"
#include "ReadThreadParams.h"

class Receiver;

DWORD WINAPI pollForEvents(LPVOID portHandle);

//typedef struct ReadThreadParams {
//	HANDLE hComm;
//	HANDLE stopThreadEvent;
//	HWND* hwnd;
//	LPDWORD numBytesRead;
//
//	ReadThreadParams(HANDLE hCom, HANDLE stopThreadEvent, LPDWORD numBytesRead, HWND* hwnd) : hComm(hCom), stopThreadEvent(stopThreadEvent), numBytesRead(numBytesRead), hwnd(hwnd) {}
//}READTHREADPARAMS, *PREADTHREADPARAMS;