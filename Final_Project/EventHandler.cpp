#include "EventHandler.h"

/*-------------------------------------------------------------------------------------
--	FUNCTION:	stopEventHandlerThrd
--
--	DATE:			November 26, 2018
--
--	REVISIONS:		November 26, 2018 (Initial version)
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		void StopThread()
--
--	RETURNS:		void
--
--	NOTES:
--	Call this function to set isListening to false to trigger the running thread to exit
--	and close the thread handle after.
--------------------------------------------------------------------------------------*/
void stopEventHandlerThrd()
{
	isListening = FALSE;
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	pollForEvents
--
--	DATE:			November 26, 2018
--
--	REVISIONS:		November 26, 2018 (Initial version)
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		DWORD WINAPI pollForEvents(LPVOID portHandle)
--						LPVOID portHandle: the handle to monitor for incoming frames
--
--	RETURNS:		returns 0 when infinite loop to poll for received characters exits
--
--	NOTES:
--	Thread calls this function and initiates the infinite loop to poll for any
--	characters received via the connected port.
--	Any data received is passed to receiveFrame to be hanl.
--------------------------------------------------------------------------------------*/
DWORD WINAPI pollForEvents(LPVOID n)
{
	PREADTHREADPARAMS readTP;
	readTP = PREADTHREADPARAMS(n);
	COMMTIMEOUTS timeouts = { 0,0,10,0,0 };
	SetCommTimeouts(readTP->hComm, &timeouts);
	DWORD waitResult;

	while (1) {
		char readStr[1024];
		waitResult = WaitForSingleObject(readTP->stopThreadEvent, 10);
		switch (waitResult)
		{
		case WAIT_TIMEOUT:
			if (!ReadFile(readTP->hComm, readStr, sizeof(readStr), readTP->numBytesRead, NULL)) {
				OutputDebugStringA("FAILED : read from serial");
			}
			else {
				//char printChar[] = { tempChar, '\0' };
				//TextOut(readTP->hdc, 0, 0, (LPCWSTR)printChar, strlen(printChar));
				if (*(readTP->numBytesRead) > 0) {
					receiveFrame(readStr, readTP->hwnd);
				}
			}
			break;
		case WAIT_OBJECT_0:
			ExitThread(0);
			break;
		default:
			break;
		}
	}
	//DWORD dwEvent;
	//BOOL fWaitingOnRead = FALSE;
	//DWORD dwRes;
	//char chRead[1024];
	//DWORD dwRead = NULL;

	//OVERLAPPED ovRead = { 0 };
	//ovRead.hEvent = CreateEvent(0, TRUE, FALSE, 0);
	//if (ovRead.hEvent == NULL) 
	//{
	//	//error in creating event
	//}

	//while (isListening)
	//{
	//	if (!WaitCommEvent(portHandle, &dwEvent, &ovRead))
	//	{
	//		dwRes = WaitForSingleObject(ovRead.hEvent, INFINITE);
	//		switch (dwRes)
	//		{
	//		case WAIT_OBJECT_0:
	//			if (!GetOverlappedResult(portHandle, &ovRead, &dwRead, FALSE))
	//			{
	//				// Error in communications;
	//			}
	//			else
	//			{
	//				if (!ReadFile(portHandle, chRead, 1024, &dwRead, &ovRead))
	//				{
	//				receiveFrame(chRead);
	//				}
	//			}
	//			break;

	//		case WAIT_TIMEOUT:
	//			// This is a good time to do some background work.
	//			break;

	//		default:
	//			// Error in the WaitForSingleObject; abort.
	//			// This indicates a problem with the OVERLAPPED structure's
	//			// event handle.
	//			break;
	//		}
	//	}
	//}
	return 0;
}