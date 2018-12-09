#include "EventHandler.h"

DWORD receiveTOThreadId;
HANDLE hReceiveTimeoutThrd;

/*-------------------------------------------------------------------------------------
--	FUNCTION:	pollForEvents
--
--	DATE:			November 26, 2018
--
--	REVISIONS:		November 26, 2018 (Initial version)
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		DWORD WINAPI pollForEvents(LPVOID portHandle)
--						LPVOID portHandle: the handle to monitor for incoming frames
--
--	RETURNS:		returns 0 when infinite loop to poll for received characters exits
--
--	NOTES:
--	Thread calls this function and initiates the infinite loop to poll for any
--	characters received via the connected port.
--	Any data received is passed to receiveFrame to be hanl.
--------------------------------------------------------------------------------------*/
DWORD WINAPI pollForEvents(LPVOID n)
{
	VariableManager &vm = VariableManager::getInstance();
	PREADTHREADPARAMS readTP;
	readTP = PREADTHREADPARAMS(n);
	COMMTIMEOUTS timeouts = { 0,0,10,0,0 };
	SetCommTimeouts(vm.get_portHandle(), &timeouts);
	DWORD waitResult;

	//lastFrameReceived = time(0);

	//if (isListening) {
	//	// Start receiveTO thread since the program is listening
	//	hReceiveTimeoutThrd = CreateThread(NULL, 0, checkReceiveTimeout, 0, 0, &receiveTOThreadId);
	//	while (isListening)
	//	{
	//		if (WaitCommEvent(portHandle, &dwEvent, NULL))
	//		{
	//			if (dwEvent & EV_RXCHAR)
	//			{
	//				ReadFromPort(portHandle);
	//				lastFrameReceived = time(0);
	//			}
	//		}
	//		else
	//		{
	//			MessageBox(NULL, "Error Reading from port", "", MB_OK);
	//			break;
	//		}
	//	}
	//}
	//CloseHandle(hReceiveTimeoutThrd);

	while (1) {
		waitResult = WaitForSingleObject(readTP->stopThreadEvent, 10);
		switch (waitResult)
		{
		case WAIT_TIMEOUT:
			readFromPort(readTP);
			break;
		case WAIT_OBJECT_0:
			ExitThread(0);
			break;
		default:
			break;
		}
	}
	return 0;
}
