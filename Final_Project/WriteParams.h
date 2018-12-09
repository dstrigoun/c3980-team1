#pragma once
#include <Windows.h>
typedef struct WriteParams {
	HANDLE portHandle;
	char* frame;
	size_t frameLen;
	WriteParams(HANDLE portHandle, char* frame, size_t frameLen) : portHandle(portHandle), frame(frame), frameLen(frameLen) {}
}WriteParams, *PWriteParams;