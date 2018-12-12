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
void sendFrame(LPVOID writeParams)
{
	PWriteParams wp;
	wp = PWriteParams(writeParams);
	
	(wp->frameLen == 3) ? sendCtrlFrame(writeParams) : sendDataFrame(writeParams);

}

void sendCtrlFrame(LPVOID writeParams) 
{
	VariableManager &vm = VariableManager::getInstance();

	PWriteParams wp;
	wp = PWriteParams(writeParams);
	sendFrameToPort( wp->frame, wp->frameLen);

	debugMessage("CtrlFrame Sent Successfully");
}

void sendDataFrame(LPVOID writeParams)
{
	VariableManager &vm = VariableManager::getInstance();
	PWriteParams wp;
	wp = PWriteParams(writeParams);

	if (!vm.isMaxFramesSent()) {
		sendFrameToPort( wp->frame, wp->frameLen);
		vm.increment_numFramesSent();

		debugMessage("DataFrame Sent Successfully");
		debugMessage("Number of Data Frames Sent: " + to_string(vm.get_numFramesSent()));
	}
	else {
		debugMessage("Reached Max Number of Frames Sent");
		goToIdle();
	}
}

void resendDataFrame()
{

	VariableManager &vm = VariableManager::getInstance();

	if (!vm.isMaxResends()) {
		sendFrameToPort( (char*)vm.get_lastFrameSent(), 1024);
		vm.increment_numFramesReSent();
		debugMessage("Number of resends: " + to_string(vm.get_numFramesReSent()));
	}
	else {
		debugMessage("Reached Max Number of Resends");
		goToIdle();
	}
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

	PWriteParams write_params;
	write_params = PWriteParams(writeParams);

	do {
		sendFrameToPort(write_params->frame, write_params->frameLen);

		debugMessage("Send EOT");

		Sleep(5000);
	} while (vm.get_curState() == "IDLE");
	
	return 0;
}