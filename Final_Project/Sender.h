#pragma once
#include <Windows.h>
#include "FrameHandler.h"
#include "WriteParams.h"

void sendFrameToPort(char* frame, size_t frameLen);
