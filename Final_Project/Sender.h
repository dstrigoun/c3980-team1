#pragma once
#include <Windows.h>
#include "FrameHandler.h"
//
//DWORD WINAPI WriteToPort(LPVOID writeParams);
//
typedef struct WriteParams {
	HANDLE hComm;
	char* data;
	WriteParams(HANDLE comm, char* data) : hComm(comm), data(data) {}
}WriteParams, *PWriteParams;