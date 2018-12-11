#pragma once

#include <Windows.h>
#include <stdio.h>
#include "Main.h"
#include "Receiver.h"
#include "ReadThreadParams.h"

class Receiver;

DWORD WINAPI pollForEvents(LPVOID portHandle);