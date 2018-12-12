#include "WriteHandler.h"
#include "sendEOTParams.h"

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

	sendFrameToPort(wp->frame, wp->frameLen);

	debugMessage("DataFrame Sent Successfully");
	vm.increment_numFramesSent();
	debugMessage("Number of Data Frames Sent: " + to_string(vm.get_numFramesSent()));

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
		wp->frame = vm.get_EOT_frame();
		wp->frameLen = 3;
		sendFrame(wp);
		vm.set_hasSentEOT(true);
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
DWORD WINAPI sendEOTs(LPVOID n)
{
	debugMessage("Creating EOT thread");

	VariableManager &vm = VariableManager::getInstance();

	PsendEOTParams sep;
	sep = (PsendEOTParams)n;

	//WriteParams wppp = sep->wp;

	DWORD waitResult;
	sendFrameToPort(sep->wp->frame, sep->wp->frameLen);

	do {
		waitResult = WaitForSingleObject(*(vm.get_stopEOTThreadEvent()), 500);	
		
		switch (waitResult) {
		case WAIT_TIMEOUT:
			debugMessage("case timeout");

			sendFrameToPort(sep->wp->frame, sep->wp->frameLen);

			break;
		case WAIT_OBJECT_0:
			debugMessage("Ending the eot thread");
			ExitThread(0);
			break;
		default:
			;
		}
	} while (1);
	
}