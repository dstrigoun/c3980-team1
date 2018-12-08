#include "EventHandler.h"

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
		//char readStr[1024];
		waitResult = WaitForSingleObject(readTP->stopThreadEvent, 10);
		switch (waitResult)
		{
		case WAIT_TIMEOUT:
			readFromPort(readTP);
			/*if (!ReadFile(readTP->hComm, readStr, sizeof(readStr), readTP->numBytesRead, NULL)) {
				OutputDebugStringA("FAILED : read from serial");
			}
			else {
				if (*(readTP->numBytesRead) > 0) {
					receiveFrame(readStr, readTP->hwnd);
				}
			}*/
			break;
		case WAIT_OBJECT_0:
			ExitThread(0);
			break;
		default:
			break;
		}
	}
	return 0;
}