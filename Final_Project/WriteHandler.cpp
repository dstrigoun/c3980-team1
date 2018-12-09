#include "WriteHandler.h"

void initWriteHandler(ifstream* file, HANDLE* port)  
{
	PcurrUploadFile = file;
	Pport = port;
	numFramesSent = 0;
	numFramesReSent = 0;
};

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
	PWriteParams wp;
	wp = PWriteParams(writeParams);
	
	sendFrameToPort(wp->portHandle,wp->frame, wp->frameLen);

	return 0;
}

void sendDataFrame()
{
	sendFrameToPort(&Pport, (char*) getPayload(PcurrUploadFile)[0],1024);
	numFramesSent++;

	debugMessage("DataFrame Sent Successfully");
	debugMessage("Number of Data Frames Sent: " + numFramesSent);
}

void resendDataFrame() 
{
	sendFrameToPort(&Pport, (char*) lastFrameSent, 1024);
	numFramesReSent++;

	debugMessage("Received NAK for DataFrame");
	debugMessage("Number of resends: " + numFramesReSent);

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

	VariableManager &vm = VariableManager::getInstance();
	DWORD dwWrite = NULL;
	DWORD dwBytesWritten = 0;

	PWriteParams wp;
	wp = PWriteParams(writeParams);

	do {
		sendFrameToPort(wp->portHandle, wp->frame, wp->frameLen);

		debugMessage("Send EOT");

		Sleep(5000);
	} while (vm.get_curState() == "IDLE");
	
	return 0;
}