#include "Sender.h"

/*-------------------------------------------------------------------------------------
--	FUNCTION:	WriteToPort
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		DWORD WINAPI WriteToPort(LPVOID hComm, LPVOID frameToSend)
--						LPVOID hComm - port to be used for writing
--						LPVOID frameToSend - frame to send
--
--	RETURNS:		void
--
--	NOTES:
--	Pass this thread function to Sender thread to send out a frame
--------------------------------------------------------------------------------------*/
//DWORD WINAPI WriteToPort(LPVOID portHandle)
//{
//	DWORD dwWrite = NULL;
//	DWORD dwBytesWritten = 0;
//
//	//PWriteParams wp;
//	//wp = PWriteParams(writeParams);
//	//char* test = wp->data;
//	//char* test = (char*)writeParams;
//	char data2[2] = "t";
//	//generateCtrlFrame(data2, 4);
//
//	do
//	{
//		//OutputDebugString(data2);
//		//WriteFile(hComm, data2, 2, 0, NULL);
//
//		char str[80] = "";
//		sprintf_s(str, "%c", LPCWSTR('a'));
//		WriteFile(portHandle, str, 1, 0, NULL);
//		Sleep(1000);
//	} while (true);
//
//	return 0;
//}
