#pragma once
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "Menu.h"

static char Name[] = "GTID";
static DWORD MAX_RANDOM_WAIT_TIME_MS = 500;

int randomNumberGenerator(int min, int max);

void triggerRandomWait();

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow);
