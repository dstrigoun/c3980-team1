#pragma once

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "Menu.h"

static char Name[] = "GTID";
static DWORD MAX_RANDOM_WAIT_TIME_MS = 500;
static DWORD IDLE_TIMEOUT_TIME_S = 30;
static DWORD CHECK_IDLE_TIMEOUT_MS = 5000;
LPCSTR lpszCommName = "com1";
char str[80] = "";

int randomNumberGenerator(int min, int max);

void triggerRandomWait();

DWORD WINAPI checkIdleTimeout(LPVOID n);

void terminateProgram();

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow);

void sendCharacter(HWND hwnd/*, WPARAM wParam*/);