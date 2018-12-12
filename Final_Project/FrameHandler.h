#pragma once
#include <windows.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "Main.h"
#include "WriteParams.h"
#include "ReadThreadParams.h"
#include "WriteHandler.h"

static const char SYN = 22;
static const char DC1 = 17;
static const char DC2 = 18;
static const char DC4 = 20;
static const char ACK = 6;
static const char NAK = 21;
static const char ENQ = 5;
static const char EOT = 4;
static const char SMILEY = 2;

void receiveFrame(const char* frame, PREADTHREADPARAMS rtp);
void generateAndSendFrame(char ctrl, PWriteParams wp);


void readDataFrame(const char* frame, DWORD numBytesSent, bool firstPartOfFrame);
void readCtrlFrame(const char* frame, PREADTHREADPARAMS rtp);

void generateDataFrame(char* dataFrame);
void generateCtrlFrame(char* ctrlFrame, char ctrl);

DWORD WINAPI receiveTimeout(LPVOID n);
DWORD WINAPI transmissionTimeout(LPVOID n);

DWORD WINAPI displayStats(LPVOID hwnd);
