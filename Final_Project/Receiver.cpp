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
void readFromPort(PREADTHREADPARAMS readTP) {
	char readStr[1024];
	if (!ReadFile(readTP->hComm, readStr, sizeof(readStr), readTP->numBytesRead, NULL)) {
		OutputDebugStringA("FAILED : read from serial");
	}
	else {
		if (*(readTP->numBytesRead) > 0) {
			receiveFrame(readStr, readTP);
		}
	}
}