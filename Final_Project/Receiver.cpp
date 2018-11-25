#include "Receiver.h"
#include "Main.h"
#include "FrameHandler.h"

DWORD WINAPI ReadFromPort(LPVOID hComm)
{
	DWORD dwRead = NULL;
	char chRead[1024];
	DWORD dwEvent;
	SetCommMask(hComm, EV_RXCHAR);

	while (curState == "RECEIVE")
	{
		if (WaitCommEvent(hComm, &dwEvent, NULL))
		{
			if (dwEvent & EV_RXCHAR)
			{
				do
				{
					
					ReadFile(hComm, chRead, 1024, &dwRead, NULL);
					if (chRead != NULL)
					{
						receiveFrame(chRead);
					}
				} while (dwRead == 1024);

			}

		}
		else
		{
			MessageBox(NULL, "Error Reading from port", "", MB_OK);
			break;
		}

	}
	return 0;
}
