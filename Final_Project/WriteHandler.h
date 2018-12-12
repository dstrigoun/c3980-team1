#pragma once

#include <windows.h>
#include <fstream>
#include <iostream>
#include "Main.h"
#include "Sender.h"
#include "FileChooser.h"
#include "VariableManager.h"

using namespace std;

void sendCtrlFrame(LPVOID writeParams);

void sendDataFrame(LPVOID writeParams);

void resendDataFrame();

void sendFrame(LPVOID writeParams);

DWORD WINAPI sendEOTs(LPVOID writeParams);