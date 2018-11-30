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
	isListening = false;
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
	isListening = true;
	SetCommMask(portHandle, EV_RXCHAR | EV_CTS);

	while (isListening)
	{
		if (WaitCommEvent(portHandle, &dwEvent, NULL))
		{
			if (dwEvent & EV_RXCHAR)
			{
				ReadFromPort(portHandle);
			}
		}
		else
		{
			MessageBox(NULL, "Error Reading from port", "", MB_OK);
			break;
		}
	}

	return 0;
}