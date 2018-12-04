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
DWORD WINAPI pollForEvents(LPVOID portHandle)
{
	DWORD dwEvent;
	BOOL fWaitingOnRead = FALSE;
	DWORD dwRes;
	char chRead[1024];
	DWORD dwRead = NULL;

	OVERLAPPED ovRead = { 0 };
	ovRead.hEvent = CreateEvent(0, TRUE, FALSE, 0);
	if (ovRead.hEvent == NULL) 
	{
		//error in creating event
	}

	while (isListening)
	{
		if (!WaitCommEvent(portHandle, &dwEvent, &ovRead))
		{
			dwRes = WaitForSingleObject(ovRead.hEvent, INFINITE);
			switch (dwRes)
			{
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(portHandle, &ovRead, &dwRead, FALSE))
				{
					// Error in communications;
				}
				else
				{
					if (!ReadFile(portHandle, chRead, 1024, &dwRead, &ovRead))
					{
						receiveFrame(chRead);
					}
				}
				break;

			case WAIT_TIMEOUT:
				// This is a good time to do some background work.
				break;

			default:
				// Error in the WaitForSingleObject; abort.
				// This indicates a problem with the OVERLAPPED structure's
				// event handle.
				break;
			}
		}
	}
	return 0;
}