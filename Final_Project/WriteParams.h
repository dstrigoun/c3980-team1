#pragma once
#include <Windows.h>
typedef struct WriteParams {
	char* frame;
	size_t frameLen;
	WriteParams(char* frame, size_t frameLen) : frame(frame), frameLen(frameLen) {}
}WriteParams, *PWriteParams;