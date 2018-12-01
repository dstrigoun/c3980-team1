#pragma once
#include <Windows.h>
#include "FrameHandler.h"

typedef struct WriteParams {
	HANDLE portHandle;
	char* frame;
	WriteParams(HANDLE portHandle, char* frame) : portHandle(portHandle), frame(frame) {}
}WriteParams, *PWriteParams;

void sendFrameToPort(HANDLE portHandle, char* frame);