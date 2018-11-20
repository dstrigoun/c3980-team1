/*-------------------------------------------------------------------------------------
--	SOURCE FILE: Main.cpp - An application that create/show window and handles basic
--							menu functions and window events
--
--	PROGRAM:		GTID
--
--	FUNCTIONS:
--					int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
--											LPSTR lspszCmdParam, int nCmdShow)
--					LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam,
--											LPARAM lParam)
--
--
--	DATE:			November 19, 2018
--
--	REVISIONS:		November 19, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	NOTES:
--	This program will create the window and menu that the user can interact with.
--
--	User can upload by clicking the menu 'Upload'
--
--------------------------------------------------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include "Menu.h"

char Name[] = "GTID";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#pragma warning (disable: 4096)

LPCSTR	lpszCommName = "com1";
bool isConnected = false;

/*-------------------------------------------------------------------------------------
--	FUNCTION:	WinMain
--
--	DATE:			November 19, 2018
--
--	REVISIONS:		November 19, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
--									LPSTR lspszCmdParam, int nCmdShow)
--
--	RETURNS:		int
--
--	NOTES:
--	Call this function to create the main window and monitor events received from user
--------------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow)
{
	MSG Msg;
	WNDCLASSEX Wcl;
	HWND hWnd;

	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = CS_HREDRAW | CS_VREDRAW;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	Wcl.hIconSm = NULL;
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);

	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	Wcl.lpszClassName = Name;

	Wcl.lpszMenuName = "MYMENU";
	Wcl.cbClsExtra = 0;
	Wcl.cbWndExtra = 0;

	if (!RegisterClassEx(&Wcl))
		return 0;

	hWnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
		600, 400, NULL, NULL, hInst, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	WndProc
--
--	DATE:			November 19, 2018
--
--	REVISIONS:		November 19, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam,
--											LPARAM lParam)
--
--	RETURNS:		LRESULT
--
--	NOTES:
--	Call this to handle window events.
--------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_UPLOAD:
		
			break;
		}
		break;

	case WM_DESTROY:	// Terminate program
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}