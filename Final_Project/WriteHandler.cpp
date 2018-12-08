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
--	INTERFACE:		DWORD WINAPI snedFrame(LPVOID writeParams)
--						LPVOID writeParams - contains the port and frame
--
--	RETURNS:		returns 0 when finished attempt to send
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
	
	sendFrameToPort(wp->portHandle,wp->frame, wp->frameLen);

	return 0;
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	sendEOTs
--
--	DATE:			December 03, 2018
--
--	REVISIONS:		December 03, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		DWORD WINAPI sendEOTs(LPVOID writeParams)
--						LPVOID writeParams - contains the port and frame
--
--	RETURNS:		returns 0 when curState is no longer "IDLE" 
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
		sendFrameToPort(wp->portHandle, wp->frame, wp->frameLen);

		std::ofstream file;
		file.open("log.txt", std::fstream::app);
		file << time(0) << ": \tSent EOT\n";
		file.close();

		Sleep(5000);
	} while (curState == "IDLE");
	
	return 0;
}