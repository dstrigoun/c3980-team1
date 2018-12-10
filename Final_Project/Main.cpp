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
--					void sendCharacter(HWND hwnd)
--
--	DATE:			November 19, 2018
--
--	REVISIONS:		November 19, 2018
--					November 23, 2018 - added sendCharacter(HWND hwnd)
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim, Dasha Strigoun, Alexander Song
--
--	NOTES:
--	This program will create the window and menu that the user can interact with.
--
--	User can upload by clicking the menu 'Upload'
--
--------------------------------------------------------------------------------------*/

#include "Menu.h"
#include "Main.h"

#define STRICT_TYPED_ITEMIDS
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <atlbase.h>
#include <AtlConv.h>
#include <queue>
using namespace std;

//time_t LAST_EOT_RECEIVED;
DWORD idleTimeoutThreadId;
DWORD eventHandlerThreadId;
DWORD senderThreadId;

DWORD numBytesRead;

HANDLE hIdleTimeoutThrd;
HANDLE eventHandlerThrd;
HANDLE senderThrd;

HANDLE stopThreadEvent = CreateEventA(NULL, false, false, "stopEventThread");
COMMCONFIG	cc;
LPCSTR lpszCommName = "com1";
char str[80] = "";
char CurrentSendingCharArrKieran[1024];

PREADTHREADPARAMS rtp;


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

	VariableManager& vm = VariableManager::getInstance();
	vm.set_curState("IDLE");

	hWnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
		600, 400, NULL, NULL, hInst, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	vm.set_hwnd(hWnd);

	srand((unsigned int)time(0));
	//initial random wait to put programs off sync to reduce collision
	triggerRandomWait();

	//open com port
	HANDLE tempPortHandle;
	if ((tempPortHandle = CreateFile(lpszCommName, GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, NULL, NULL))
		== INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
		std::ofstream file;
		file.open("log.txt", std::fstream::app);
		file << time(0) << ": \tError opening COM port.\n";
		file.close();
		//PostQuitMessage(0); // end program since opening port failed
	}
	vm.set_portHandle(tempPortHandle);
	COMMTIMEOUTS timeouts = { 0,0,10,0,0 };
	SetCommTimeouts(vm.get_portHandle(), &timeouts);

	//wp.portHandle = portHandle;
	//wp.frame = CurrentSendingCharArrKieran;
	//wp.frameLen = 1024;
	cc.dwSize = sizeof(COMMCONFIG);
	cc.wVersion = 0x100;

	SetCommMask(vm.get_portHandle(), EV_RXCHAR);

	//start thread with checkIdleTimeout
	hIdleTimeoutThrd = CreateThread(NULL, 0, checkIdleTimeout, 0, 0, &idleTimeoutThreadId);
	PREADTHREADPARAMS rtp = new ReadThreadParams (stopThreadEvent, &numBytesRead);
	eventHandlerThrd = CreateThread(NULL, 0, pollForEvents, (LPVOID)rtp, 0, &eventHandlerThreadId);

	char testEOTFrame[3];
	generateCtrlFrame(testEOTFrame, EOT);
	size_t frameLen = 3;
	PWriteParams writeParams = new WriteParams(testEOTFrame, frameLen);
	
	senderThrd = CreateThread(NULL, 0, sendEOTs, (LPVOID)writeParams, 0, &senderThreadId);

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	
	return (int)Msg.wParam;
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
	VariableManager& vm = VariableManager::getInstance();
	switch (Message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_UPLOAD:
			std::ofstream file;
			file.open("log.txt", std::fstream::app);
			file << time(0) << ": \tClicked upload\n";
			file.close();

			ifstream* kieransTempButNotReallyTempUploadFile = new ifstream;
			*kieransTempButNotReallyTempUploadFile = openFile(&hwnd);
			vm.set_currUploadFile(kieransTempButNotReallyTempUploadFile); //ho;pefully this memery is never releazsed weh we are usnig it

			wp->frame = CurrentSendingCharArrKieran;
			
			file.open("log.txt", std::fstream::app);
			file << time(0) << ": \tSENDING ENQ, BEFORE GENERATE FRAME\n";
			generateFrame(NULL, ENQ, wp);
			file << time(0) << ": \tSENDING ENQ, AFTERGENERATE FRAME\n";
		

			file << time(0) << ": \tRight before setting ENQ_FLAG to true.\n";
			vm.set_ENQ_FLAG(true);
			file << time(0) << ": \tRight after setting ENQ_FLAG to true." << vm.get_ENQ_FLAG() << "\n";
			
			file.close();
			break;
		}
		break;

	case WM_DESTROY:	// Terminate program
		SetEvent(stopThreadEvent);
		CloseHandle(hIdleTimeoutThrd);
		CloseHandle(eventHandlerThrd);
		CloseHandle(senderThrd);
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
	int randomNum = (int)((double)rand() / (RAND_MAX + 1) * (max - min) + min);
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
		if (currentTime - LAST_EOT_RECEIVED > IDLE_TIMEOUT_TIME_S) 
		{
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
	CloseHandle(eventHandlerThrd);
	CloseHandle(senderThrd);
	PostQuitMessage(0);
	exit(1);
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	sendCharacter
--
--	DATE:			November 23, 2018
--
--	REVISIONS:		November 23, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Alexander Song
--
--	INTERFACE:		void sendCharacter(HWND hwnd)
--
--	RETURNS:		n/a
--
--	NOTES:
--	Called to send a character to the port
--------------------------------------------------------------------------------------*/
void sendCharacter(HWND hwnd) {
	VariableManager& vm = VariableManager::getInstance();
	//HDC hdc = GetDC(hwnd); // get device context
	sprintf_s(str, "%c", LPCWSTR('a'));
	WriteFile(vm.get_portHandle(), str, 1, 0, NULL);
	//ReleaseDC(hwnd, hdc); // Release device context
}

void updateLastEOTReceived(time_t receivedTime) {
	LAST_EOT_RECEIVED = receivedTime;
}