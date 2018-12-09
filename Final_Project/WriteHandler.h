#pragma once
#include <windows.h>
#include "Main.h"
#include "Sender.h"
#include "VariableManager.h"

DWORD WINAPI sendFrame(LPVOID writeParams);

DWORD WINAPI sendEOTs(LPVOID writeParams);