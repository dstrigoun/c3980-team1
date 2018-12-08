#pragma once


#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include "Menu.h"
#include "FrameHandler.h"
#include "EventHandler.h"
#include "WriteHandler.h"

static char Name[] = "GTID";
static DWORD MAX_RANDOM_WAIT_TIME_MS = 500;
static DWORD IDLE_TIMEOUT_TIME_S = 30;
static DWORD CHECK_IDLE_TIMEOUT_MS = 5000;
static std::string curState = "IDLE";
static time_t LAST_EOT_RECEIVED = time(0);
static bool ENQ_FLAG = false;
static bool unfinishedTransmission = false;

int randomNumberGenerator(int min, int max);

void triggerRandomWait();

DWORD WINAPI checkIdleTimeout(LPVOID n);

void terminateProgram();

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow);

void sendCharacter(HWND hwnd);

void updateLastEOTReceived(time_t receivedTime);

