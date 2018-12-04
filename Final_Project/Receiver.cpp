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
void ReadFromPort(LPVOID portHandle)
{
	DWORD dwRead = NULL;
	char chRead[1024];
	do
	{
		ReadFile(portHandle, chRead, 1024, &dwRead, NULL);
		if (chRead != NULL)
		{
			receiveFrame(chRead);
		}
	} while (dwRead == 1024);
}

DWORD WINAPI checkReceiveTimeout(LPVOID n) {
	bool isReceiving = true;
	while (isReceiving) {
		time_t currentTime = time(0);
		if (currentTime - lastFrameReceived >= RECEIVE_TIMEOUT_TIME_S) {
			char ctrlFrame[4] = {};
			sendFrame(ctrlFrame, NULL, EOT);
			curState = "IDLE";
			isReceiving = false;
			isListening = false;
			OutputDebugString("ReceiveTO");
		}

		//Sleep(CHECK_IDLE_TIMEOUT_MS);
	}

	return 0;
}