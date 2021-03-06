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
	VariableManager &vm = VariableManager::getInstance();
	char readStr[1024];

	if (!ReadFile(vm.get_portHandle(), readStr, sizeof(readStr), readTP->numBytesRead, NULL)) {
		debugMessage("Read file failed");
	}
	else {
		if (*(readTP->numBytesRead) > 0) {
			std::ofstream file;
			file.open("log.txt", std::fstream::app);
			file << time(0) << ": \tNumBytesRead: " << *(readTP->numBytesRead) << "\n";
			file.close();
			receiveFrame(readStr, readTP);
		}
	}
}