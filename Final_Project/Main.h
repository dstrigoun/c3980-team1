#pragma once


#define STRICT_TYPED_ITEMIDS
#include <fstream>
#include <iostream>
#include <vector>
#include <atlbase.h>
#include <AtlConv.h>

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <mutex>
#include <atomic>
#include <vector>
#include <sstream>
#include "Menu.h"
#include "FrameHandler.h"
#include "EventHandler.h"
#include "WriteHandler.h"
#include "WriteParams.h"
#include "VariableManager.h"
#include "FileChooser.h"
#include "Sender.h"

static char Name[] = "GTID";
static DWORD MAX_RANDOM_WAIT_TIME_MS = 3000;
static DWORD IDLE_TIMEOUT_TIME_S = 30;
static DWORD RECEIVE_TIMEOUT_TIME_S = 8;
static DWORD RETRANSMISSION_TIMEOUT_TIME_S = 2;
static DWORD CHECK_IDLE_TIMEOUT_MS = 5000;
static HANDLE senderThrd;

static PWriteParams wp = new WriteParams(NULL, NULL);

int randomNumberGenerator(int min, int max);

void goToIdle();

void debugMessage(std::string message);

void triggerRandomWait();

DWORD WINAPI checkIdleTimeout(LPVOID n);

void terminateProgram();

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow);

void sendCharacter(HWND hwnd);

void create_CTRL_frames();

DWORD WINAPI displayStats(LPVOID hwnd);

void initialDisplay();