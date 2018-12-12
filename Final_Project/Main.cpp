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
#include "sendEOTParams.h"

#define STRICT_TYPED_ITEMIDS
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <atlbase.h>
#include <AtlConv.h>
#include <queue>

using namespace std;

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
	vm.set_countDataFrameBytesRead(0);
	vm.set_stopThreadEvent(stopThreadEvent);

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
		debugMessage("Error opening COM port");
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
	debugMessage("Starting Connection");
	//start thread with checkIdleTimeout
	vm.set_LAST_EOT(time(0));
	hIdleTimeoutThrd = CreateThread(NULL, 0, checkIdleTimeout, 0, 0, &idleTimeoutThreadId);
	PREADTHREADPARAMS rtp = new ReadThreadParams (stopThreadEvent, &numBytesRead);
	eventHandlerThrd = CreateThread(NULL, 0, pollForEvents, (LPVOID)rtp, 0, &eventHandlerThreadId);

	create_CTRL_frames();

	size_t frameLen = 3;
	WriteParams wp(vm.get_EOT_frame(), frameLen);
	PsendEOTParams sep = new sendEOTParams(stopThreadEvent, &wp);
	
	senderThrd = CreateThread(NULL, 0, sendEOTs, (LPVOID)sep, 0, &senderThreadId);

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
			debugMessage("Clicked upload");
			if (vm.get_unfinishedTransmission()) {
				MessageBox(hwnd, "file currenrtly uploading, please try again later", "sorry", MB_OK);
			}
			else {
				ifstream* kieransTempButNotReallyTempUploadFile = new ifstream;
				*kieransTempButNotReallyTempUploadFile = openFile(&hwnd);
				vm.set_currUploadFile(kieransTempButNotReallyTempUploadFile); //ho;pefully this memery is never releazsed weh we are usnig it

				wp->frame = CurrentSendingCharArrKieran;
				vm.set_unfinishedTransmission(true);
				vm.set_ENQ_FLAG(true);

				generateAndSendFrame(ENQ, wp);
				vm.reset_numFramesSent();
				vm.reset_numFramesReSent();

				PREADTHREADPARAMS rtp = new ReadThreadParams(stopThreadEvent, &numBytesRead);

			}


			break;
		}
		break;

	case WM_DESTROY:	// Terminate program
		debugMessage("Connected Terminated");
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
--	FUNCTION:	goToIdle
--
--	DATE:			December 8th, 2018
--
--	REVISIONS:		December 8th, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Dasha Strigoun
--
--	INTERFACE:		void goToIdle()
--
--	RETURNS:		void
--
--	NOTES:
--	Call this to start IDLE functions
--------------------------------------------------------------------------------------*/
void goToIdle()
{
	VariableManager& vm = VariableManager::getInstance();

	std::string previous_state = (vm.get_curState() == "SEND") ? "SEND" : "RECEIVE";
	vm.set_curState("IDLE");

	debugMessage("Current State: " + vm.get_curState());
	vm.set_ENQ_FLAG(false);

	vm.set_LAST_EOT(time(0));
	hIdleTimeoutThrd = CreateThread(NULL, 0, checkIdleTimeout, 0, 0, &idleTimeoutThreadId);

	debugMessage("IDLE timeout created, starting to send EOTs");

	std::stringstream message;
	message << "EOT Frame to start thread: " << (LPSTR)vm.get_EOT_frame() << std::endl;
	debugMessage(message.str());

	size_t frameLen = 3;
	PWriteParams wps = new WriteParams(vm.get_EOT_frame(), frameLen);
	PsendEOTParams sep = new sendEOTParams(stopThreadEvent, wps);

	senderThrd = CreateThread(NULL, 0, sendEOTs, (LPVOID)sep, 0, &senderThreadId);

	if (previous_state == "SEND")
	{
		triggerRandomWait();
	}
	

	// check to see if there's data
	if (vm.get_unfinishedTransmission()) {
		debugMessage("Buffer not empty yet, sending ENQ to bid for line");
		wp->frame = vm.get_ENQ_frame();
		wp->frameLen = 3;
		sendFrame(wp);
		vm.set_ENQ_FLAG(true);
	}
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	debugMessage
--
--	DATE:			December 8th, 2018
--
--	REVISIONS:		December 8th, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Dasha Strigoun
--
--	INTERFACE:		void debugMessage(std::string message)
--						std::string message: message to append to log file
--
--	RETURNS:		void
--
--	NOTES:
--	Call this to log a message to log.txt that gets created in the program directory.
--------------------------------------------------------------------------------------*/
void debugMessage(std::string message)
{
	std::ofstream file;
	file.open("log.txt", std::fstream::app);
	file << time(0) << ": \t" + message + "\n";
	file.close();
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
	VariableManager& vm = VariableManager::getInstance();
	while (vm.get_curState() == "IDLE") {
		time_t currentTime = time(0);
		if (currentTime - vm.get_LAST_EOT() > IDLE_TIMEOUT_TIME_S) 
		{
			terminateProgram();
		}
		int difference = currentTime - vm.get_LAST_EOT();
		std::stringstream message;
		message << "Last EOT was " << difference << " seconds ago";
		debugMessage(message.str());
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
	debugMessage("Lost Connection");
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
--	RETURNS:		void
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

void create_CTRL_frames() {
	VariableManager& vm = VariableManager::getInstance();
	char tempEOTFrame[3] = {};
	generateCtrlFrame(tempEOTFrame, EOT);
	vm.set_EOT_frame(tempEOTFrame);

	char tempENQFrame[3] = {};
	generateCtrlFrame(tempENQFrame, ENQ);
	vm.set_ENQ_frame(tempENQFrame);
}
