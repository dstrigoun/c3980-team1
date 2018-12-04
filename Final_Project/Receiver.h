#pragma once
#include <Windows.h>
#include "FrameHandler.h"

DWORD WINAPI checkReceiveTimeout(LPVOID n);
void ReadFromPort(LPVOID portHandle);