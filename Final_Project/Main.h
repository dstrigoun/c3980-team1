#pragma once
#include <windows.h>
#include <stdio.h>

int randomNumberGenerator(int min, int max);

void triggerRandomWait();

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow);
