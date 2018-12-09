#pragma once
#include <Windows.h>

typedef struct ReadThreadParams {
	HANDLE stopThreadEvent;
	LPDWORD numBytesRead;

	ReadThreadParams(HANDLE stopThreadEvent, LPDWORD numBytesRead) : stopThreadEvent(stopThreadEvent), numBytesRead(numBytesRead){}
}READTHREADPARAMS, *PREADTHREADPARAMS;