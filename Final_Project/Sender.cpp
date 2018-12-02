#include "Sender.h"

/*-------------------------------------------------------------------------------------
--	FUNCTION:	WriteToPort
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		DWORD WINAPI WriteToPort(LPVOID hComm, LPVOID frameToSend)
--						LPVOID hComm - port to be used for writing
--						LPVOID frameToSend - frame to send
--
--	RETURNS:		void
--
--	NOTES:
--	Pass this thread function to Sender thread to send out a frame
--------------------------------------------------------------------------------------*/
DWORD WINAPI WriteToPort(LPVOID hComm, LPVOID frameToSend)
{
	DWORD dwWrite = NULL;
	dwBytesWritten = 0;

	DWORD dwEvent;
	// commMask should be combined with Receiver and set in Main
	SetCommMask(hComm, EV_CTS);

	while (curState == "SEND")
	{
		if (WaitCommEvent(hComm, &dwEvent, NULL))
		{
			if(dwEvent & EV_CTS)
			{
				do
				{
					WriteFile(hComm, frameToSend, 1024, &dwBytesWritten, NULL);
				} while (isClearToSend);
			}
		}
	}

	return 0;
}
