#pragma once
#include <Windows.h>
#include "FrameHandler.h"

static HANDLE receiveTO;
void readFromPort(HANDLE portHandle);
