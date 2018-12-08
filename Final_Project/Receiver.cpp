#include "Receiver.h"

/*-------------------------------------------------------------------------------------
--	FUNCTION:	readFromPort
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--					December 03, 2018 - refactored for better layered architechture
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		void ReadFromPort(LPVOID portHandle)
--
--	RETURNS:		void
--
--	NOTES:
--	Pass this thread function to Receiver thread to handle a received char event
--	and handle the frame received.
--------------------------------------------------------------------------------------*/
void readFromPort(HANDLE portHandle)
{

	DWORD dwEvent;
	DWORD dwRes;
	char chRead[1024];
	DWORD dwRead = NULL;
	OVERLAPPED ovRead = { 0 };
	ovRead.hEvent = CreateEvent(0, TRUE, FALSE, 0);
	if (ovRead.hEvent == NULL)
	{
		//error in creating event
	}

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
					//receiveFrame(chRead); COMMENTED OUT BY KIERAN 07/12/2018 cause he was messing with receive frame
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
