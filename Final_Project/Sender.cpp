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
void sendFrameToPort(HANDLE portHandle, char* frame, size_t frameLen) {
	
	DWORD numBytesWritten;
	if (!WriteFile(portHandle, frame, frameLen, &numBytesWritten, NULL)) {
		exit(1);
	}
	//OVERLAPPED osWrite = { 0 };
	//DWORD dwWritten;
	//DWORD dwRes;

	//// Create this write operation's OVERLAPPED structure's hEvent.
	//osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//if (osWrite.hEvent == NULL) 
	//{
	//	//error in creating event
	//}
	//// Issue write.
	//if (!WriteFile(portHandle, frame, frameLen, &dwWritten, &osWrite))
	//{
	//	if (GetLastError() != ERROR_IO_PENDING) 
	//	{
	//		// WriteFile failed, but isn't delayed. Report error and abort.
	//	}
	//	else
	//	{
	//		// Write is pending.
	//		dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
	//	}
	//		
	//	switch (dwRes)
	//	{
	//		// OVERLAPPED structure's event has been signalled. 
	//	case WAIT_OBJECT_0:
	//		if (!GetOverlappedResult(portHandle, &osWrite, &dwWritten, FALSE))
	//		{
	//			//error
	//		}
	//		else
	//		{
	//			// Write operation completed successfully.
	//		}
	//		break;
	//	default:
	//		// An error has occurred in WaitForSingleObject.
	//		// This usually indicates a problem with the
	//	   // OVERLAPPED structure's event handle.
	//		break;
	//	}
	//}
	//else 
	//{
	//	// WriteFile completed immediately.
	//}
 //  CloseHandle(osWrite.hEvent);
}