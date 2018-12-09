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

static int MAX_RESENDS = 5;
static int MAX_FRAMES_SENT = 50;


void initWriteHandler(ifstream* file);

void sendCtrlFrame(LPVOID writeParams);

void sendDataFrame(LPVOID writeParams);

void resendDataFrame(LPVOID writeParams);

void sendFrame(LPVOID writeParams);

DWORD WINAPI sendEOTs(LPVOID writeParams);