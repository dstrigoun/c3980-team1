#pragma once
#include <Windows.h>
#include "FrameHandler.h"
#include "ReadThreadParams.h"

static HANDLE receiveTO;
void readFromPort(PREADTHREADPARAMS readTP);
