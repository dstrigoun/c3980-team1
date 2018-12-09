#pragma once


#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <mutex>
#include <atomic>
#include <vector>
#include "Menu.h"
#include "FrameHandler.h"
#include "EventHandler.h"
#include "WriteHandler.h"
#include "WriteParams.h"
#include "VariableManager.h"
#include "FileChooser.h"
#include "Sender.h"

static char Name[] = "GTID";
static DWORD MAX_RANDOM_WAIT_TIME_MS = 500;
static DWORD IDLE_TIMEOUT_TIME_S = 30;
static DWORD CHECK_IDLE_TIMEOUT_MS = 5000;
static time_t LAST_EOT_RECEIVED = time(0);

static bool unfinishedTransmission = false;
static PWriteParams wp = new WriteParams(NULL, NULL, NULL);

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

void updateLastEOTReceived(time_t receivedTime);

