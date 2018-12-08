#pragma once
#include <Windows.h>
#include "FrameHandler.h"

typedef struct WriteParams {
	HANDLE portHandle;
	char* frame;
	size_t frameLen;
	WriteParams(HANDLE portHandle, char* frame, size_t frameLen) : portHandle(portHandle), frame(frame), frameLen(frameLen) {}
}WriteParams, *PWriteParams;

void sendFrameToPort(HANDLE portHandle, char* frame, size_t frameLen);