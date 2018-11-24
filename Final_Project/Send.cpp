#include "Send.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam)
{
	int response;
	HDC hdc;

	switch (Message)
	{
	case WM_COMMAND: // Menu items
		switch (LOWORD(wParam))
		{
			// Session layer, initializing the port, start connection, and exiting the program
		case IDM_CON1: // Port COM1
			if (!connection)
			{
				///* Connecting to the port*/
				//if ((hComm = CreateFile(lpszCommName, GENERIC_READ | GENERIC_WRITE, 0,
				//	NULL, OPEN_EXISTING, 0, NULL))
				//	== INVALID_HANDLE_VALUE)
				//{
				//	MessageBox(NULL, TEXT("Error opening COM port:"), TEXT(""), MB_OK);
				//	return FALSE;
				//}
				//cc.dwSize = sizeof(COMMCONFIG);
				//cc.wVersion = 0x100;
				//GetCommConfig(hComm, &cc, &cc.dwSize);

				//if (!CommConfigDialog(lpszCommName, hwnd, &cc))
				//	break;

				//SetCommState(hComm, &cc.dcb);
				//connection = true;
				//// Starting a thread
				//thread1 = CreateThread(NULL, 0, ReadThread, hwnd, 0, &threadId);
			}
			else
				MessageBox(NULL, TEXT("Already connected"), TEXT("Connection"), MB_OK);
			break;
		case IDM_EXIT:
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_CHAR: // Process keystroke (part of the Physical Layer, transmitting characters)
	{
		switch (wParam)
		{
		default:
			hdc = GetDC(hwnd);			 // get device context
			sprintf_s(str, "%c", LPCWSTR(wParam));
			WriteFile(hComm, str, 1, 0, NULL);
			ReleaseDC(hwnd, hdc); // Release device context
		}
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	case WM_DESTROY:	 // Closing the program
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}