#pragma once
#include <Windows.h>
#include "FrameHandler.h"
#include "WriteParams.h"



void sendFrameToPort(HANDLE portHandle, char* frame, size_t frameLen);