#include "Receiver.h"

/*-------------------------------------------------------------------------------------
--	FUNCTION:	ReadFromPort
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		DWORD WINAPI ReadFromPort(LPVOID hComm)
--						LPVOID hComm - port to be used for writing
--
--	RETURNS:		void
--
--	NOTES:
--	Pass this thread function to Receiver thread to handle a received char event
--	and handle the frame received.
--------------------------------------------------------------------------------------*/
DWORD WINAPI ReadFromPort(LPVOID hComm)
{
	DWORD dwRead = NULL;
	char chRead[1024];

	DWORD dwEvent;
	// commMask should be combined with Sender and set in Main
	SetCommMask(hComm, EV_RXCHAR);

	while (curState == "RECEIVE")
	{
		if (WaitCommEvent(hComm, &dwEvent, NULL))
		{
			if (dwEvent & EV_RXCHAR)
			{
				do
				{
					
					ReadFile(hComm, chRead, 1024, &dwRead, NULL);
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
