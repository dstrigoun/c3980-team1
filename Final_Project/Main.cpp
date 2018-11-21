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
--					void triggerRandomWait()
--					int randomNumberGenerator(int min, int max)
--					DWORD WINAPI checkIdleTimeout(LPVOID n)
--					void terminateProgram()
--
--	DATE:			November 19, 2018
--
--	REVISIONS:		November 19, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim, Dasha Strigoun
--
--	NOTES:
--	This program will create the window and menu that the user can interact with.
--
--	User can upload by clicking the menu 'Upload'
--
--------------------------------------------------------------------------------------*/

#include "Main.h"

time_t LAST_EOT_RECEIVED;
DWORD idleTimeoutThreadId;

HANDLE hIdleTimeoutThrd;
HANDLE stopThreadEvent = CreateEventA(NULL, false, false, "stopEventThread");

#pragma warning (disable: 4096)


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
	LPCSTR	lpszCommName = "com1";

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

	srand(time(0));
	//initial random wait to put programs off sync to reduce collision
	triggerRandomWait();

	//set LAST_EOT_RECEIVED to current time
	LAST_EOT_RECEIVED = time(0);

	//start thread with checkIdleTimeout
	hIdleTimeoutThrd = CreateThread(NULL, 0, checkIdleTimeout, 0, 0, &idleTimeoutThreadId);

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
			// handle file upload here
			break;
		}
		break;

	case WM_DESTROY:	// Terminate program
		SetEvent(stopThreadEvent);
		CloseHandle(hIdleTimeoutThrd);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	triggerRandomWait
--
--	DATE:			November 19, 2018
--
--	REVISIONS:		November 19, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		void triggerRandomWait() 
--
--	RETURNS:		void
--
--	NOTES:
--	Call this to halt a program and wait random miliseconds 
--	between 0 and MAX_RANDOM_WAIT_TIME_MS before resuming the program
--------------------------------------------------------------------------------------*/
void triggerRandomWait() 
{
	DWORD timeToWait_ms = randomNumberGenerator(0, MAX_RANDOM_WAIT_TIME_MS);
	Sleep(timeToWait_ms);
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	randomNumberGenerator
--
--	DATE:			November 19, 2018
--
--	REVISIONS:		November 19, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		int randomNumberGenerator(int min, int max)
--						int min: the minimum number that can be generated
--						int max: the maximum number that can be generated
--
--	RETURNS:		returns the random number between min-max
--
--	NOTES:
--	Call this to generate a random number within a specified range
--------------------------------------------------------------------------------------*/
int randomNumberGenerator(int min, int max)
{
	int randomNum = (double)rand() / (RAND_MAX + 1) * (max - min) + min;
	return randomNum;
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	checkIdleTimeout
--
--	DATE:			November 21, 2018
--
--	REVISIONS:		November 21, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Dasha Strigoun
--
--	INTERFACE:		DWORD WINAPI checkIdleTimeout(LPVOID n)
--						LPVOID n: void pointer to thread param
--
--	RETURNS:		returns 0
--
--	NOTES:
--	Called by the thread created to periodically check the difference between
--  the current time and the time when the last EOT was received
--------------------------------------------------------------------------------------*/
DWORD WINAPI checkIdleTimeout(LPVOID n)
{
	while (1) {
		time_t currentTime = time(0);

		char cur[16] = "";
		sprintf_s(cur, "%d", currentTime);
		OutputDebugString(cur);
		OutputDebugString("\n");

		if (currentTime - LAST_EOT_RECEIVED > IDLE_TIMEOUT_TIME_S) {
			terminateProgram();
		}

		Sleep(CHECK_IDLE_TIMEOUT_MS);
	}

	return 0;
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	terminateProgram
--
--	DATE:			November 21, 2018
--
--	REVISIONS:		November 21, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Dasha Strigoun
--
--	INTERFACE:		void terminateProgram()
--
--	RETURNS:		n/a
--
--	NOTES:
--	Call this to close all handles and exit the program
--------------------------------------------------------------------------------------*/
void terminateProgram() 
{
	MessageBox(NULL, "Lost connection.", "", MB_OK);

	SetEvent(stopThreadEvent);
	CloseHandle(hIdleTimeoutThrd);
	PostQuitMessage(0);
	exit(1);
}