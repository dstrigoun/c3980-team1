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
--	INTERFACE:		void sendFrameToPort(char* frame, size_t frameLen)
--						char* frame - frame to be sent
--						size_t frameLen - length of frame
--
--	RETURNS:		void
--
--	NOTES:
--	Pass this thread function to Sender thread to send out a frame
--------------------------------------------------------------------------------------*/
void sendFrameToPort(char* frame, size_t frameLen) {
	VariableManager& vm = VariableManager::getInstance();
	DWORD numBytesWritten;

	std::stringstream message;
	message << "Frame to send: " << (LPSTR)frame;
	message << "\tFrame length: " << (int)frameLen;
	debugMessage(message.str());

	if (!WriteFile(vm.get_portHandle(), frame, frameLen, &numBytesWritten, NULL)) {
		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		//debugMessage((LPSTR)lpMsgBuf);
		debugMessage("Write File failed");
	}
	
}