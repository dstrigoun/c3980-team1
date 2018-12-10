#include "Sender.h"

/*-------------------------------------------------------------------------------------
--	FUNCTION:	sendFrameToPort
--
--	DATE:			November 28, 2018
--
--	REVISIONS:		November 28, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		void sendFrameToPort(HANDLE portHandle, char* data)
--						HANDLE portHandle - port used to send frame
--						char* data - frame to be sent
--
--	RETURNS:		void
--
--	NOTES:
--	Pass this thread function to Sender thread to send out a frame
--------------------------------------------------------------------------------------*/
void sendFrameToPort(char* frame, size_t frameLen) {
	VariableManager &vm = VariableManager::getInstance();
	DWORD numBytesWritten;
	if (!WriteFile(vm.get_portHandle(), frame, frameLen, &numBytesWritten, NULL)) {
		debugMessage("Write File failed");
	}
	
}