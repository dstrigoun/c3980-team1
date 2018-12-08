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
static int numFramesSent;
static int numFramesReSent;

void initWriteHandler(ifstream* file, HANDLE* port);

void sendDataFrame();

void resendDataFrame();

DWORD WINAPI sendFrame(LPVOID writeParams);

DWORD WINAPI sendEOTs(LPVOID writeParams);