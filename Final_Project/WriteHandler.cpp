#include "WriteHandler.h"

/*-------------------------------------------------------------------------------------
--	FUNCTION:	sendFrame
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
DWORD WINAPI sendFrame(LPVOID writeParams)
{
	DWORD dwWrite = NULL;
	DWORD dwBytesWritten = 0;

	PWriteParams wp;
	wp = PWriteParams(writeParams);
	
	sendFrameToPort(wp->portHandle,wp->frame);

	return 0;
}

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
DWORD WINAPI sendEOTs(LPVOID writeParams)
{
	DWORD dwWrite = NULL;
	DWORD dwBytesWritten = 0;

	PWriteParams wp;
	wp = PWriteParams(writeParams);

	do {
		sendFrameToPort(wp->portHandle, wp->frame);
		Sleep(1000);
	} while (curState == "IDLE");
	
	return 0;
}