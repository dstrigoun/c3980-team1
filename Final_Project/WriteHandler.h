#pragma once

#include <windows.h>
#include <fstream>
#include <iostream>
#include "Main.h"
#include "Sender.h"
#include "FileChooser.h"

using namespace std;

static ifstream* PcurrUploadFile;
static LPCSTR lastFrameSent;
static HANDLE* Pport;


void initWriteHandler(ifstream* file);

void sendCtrlFrame(LPVOID writeParams);

void sendDataFrame(LPVOID writeParams);

void resendDataFrame(LPVOID writeParams);

void sendFrame(LPVOID writeParams);

DWORD WINAPI sendEOTs(LPVOID writeParams);