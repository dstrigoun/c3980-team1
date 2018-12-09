#pragma once
#include <Windows.h>
#include "FrameHandler.h"
#include "ReadThreadParams.h"

static HANDLE receiveTO;
DWORD WINAPI checkReceiveTimeout(LPVOID n);
void readFromPort(PREADTHREADPARAMS readTP);