#include "EventHandler.h"

/*-------------------------------------------------------------------------------------
--	FUNCTION:	StopThread
--
--	DATE:			October 2, 2018
--
--	REVISIONS:		October 2, 2018 (Initial version)
--
--	DESIGNER:		Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		void StopThread()
--
--	RETURNS:		void
--
--	NOTES:
--	Call this function to set isReading to false to trigger the running thread to exit
--	and close the thread handle after.
--------------------------------------------------------------------------------------*/
void stopEventHandlerThrd()
{
	isListening = false;
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	ReadFromPort
--
--	DATE:			October 2, 2018
--
--	REVISIONS:		October 2, 2018 (Initial version)
--
--	DESIGNER:		Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		DWORD WINAPI ReadFromPort(LPVOID hComm)
--						LPVOID hComm: the handle to monitor for incoming characters
--
--	RETURNS:		returns 0 when infinite loop to poll for received characters exits
--
--	NOTES:
--	Thread calls this function and initiates the infinite loop to poll for any
--	characters received via the connected port.
--	Any characters received is passed as a parameter with the displayCharactersRead
--	function call.
--------------------------------------------------------------------------------------*/
DWORD WINAPI pollForEvents(LPVOID portHandle)
{
	DWORD dwRead = NULL;
	char chRead[1024];
	DWORD dwEvent;
	isListening = true;
	SetCommMask(portHandle, EV_RXCHAR | EV_CTS);

	while (isListening)
	{
		if (WaitCommEvent(portHandle, &dwEvent, NULL))
		{
			if (dwEvent & EV_RXCHAR)
			{
				do
				{
					ReadFile(portHandle, chRead, 1024, &dwRead, NULL);
					if (chRead != NULL)
					{
						receiveFrame(chRead);
					}
				} while (dwRead == 1024);
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