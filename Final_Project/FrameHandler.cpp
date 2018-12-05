#include "FrameHandler.h"

/*-------------------------------------------------------------------------------------
--	FUNCTION:	receiveFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		void receiveFrame(const char* frame)
--						const char* frame - frame received
--
--	RETURNS:		void
--
--	NOTES:
--	Call this generic send method and send a frame based on parameters provided.
--------------------------------------------------------------------------------------*/
void receiveFrame(const char* frame) {

	// check type of frame
	if (frame[0] == SYN) {
		if (frame[1] == DC1 || frame[1] == DC2) {
			//data frame
			readDataFrame(frame);
		}
		else {
			//ctrl frame
			readCtrlFrame(frame);
		}
	}
	else {
			MessageBox(NULL, "Frame Corrupt, 1st Byte not SYN", "", MB_OK);
	}
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	sendFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--						November 30, 2018 - changed ctrl to be char
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim, Dasha Strigoun
--
--	INTERFACE:		void sendFrame(char* frame, const char* data, char ctrl)
--						char* frame - the frame to populate
--						const char* data - data to send
--						char ctrl - control character to send
--
--	RETURNS:		void
--
--	NOTES:
--	Call this generic send method and send a frame based on parameters provided.
--------------------------------------------------------------------------------------*/
void sendFrame(char* frame, const char* data, char ctrl) {
	(ctrl != NULL) ? generateCtrlFrame(frame, ctrl)
		: generateDataFrame(frame, data);
	//start sender thread here with the above created frame
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	readDataFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		void readDataFrame(const char* frame) 
--						const char* frame - data frame to read
--
--	RETURNS:		void
--
--	NOTES:
--	Call this to read a data frame and handle the data retrieved from the frame
--------------------------------------------------------------------------------------*/
void readDataFrame(const char* frame) {
	if (frame[1] != nextFrameToReceive) {
		//duplicate frame 
	}
	else {
		//alternate nextFrameToReceive between DC1 and DC2 for duplicate checks
		nextFrameToReceive = (frame[1] == DC1) ? DC2 : DC1;

		//check lastByte CRC if data is corrupt

		OutputDebugString("in readDataFrame");

		char data[9] = {};
		for (int i = 0; i < 9; i++) {
			data[i] = frame[2 + i];
		}

		HANDLE hFile = CreateFile("test.txt", GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_APPEND_DATA, 0);

		if (hFile)
		{
			OutputDebugString("created hFile successfully\n");
		}

		LPDWORD written = 0;
		WriteFile(hFile, data, 9, written, 0);

		CloseHandle(hFile);

		//strncpy_s(data, frame + 2, 9);

		//return the data portion to be appended to file
	}
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	readCtrlFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--					November 26, 2018 - receive EOT and ENQ and update state
--					November 28, 2018 - receive ACK when ENQ was sent and update state
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim, Alexander Song, Dasha Strigoun
--
--	INTERFACE:		void readCtrlFrame(const char* frame)
--						const char* frame - control frame to read
--
--	RETURNS:		void
--
--	NOTES:
--	Call this to read a control frame and handle behaviour based on each control char
--------------------------------------------------------------------------------------*/
void readCtrlFrame(const char* frame) {
	int ctrlChar = (int)frame[1];
	int dcChar = (int)frame[2];
	char cur[16] = "";
	sprintf_s(cur, "%d", ctrlChar);
	OutputDebugString(cur);
	OutputDebugString("\n");
	// handle behaviour based on control char received
	if (curState == "IDLE") {
		if (ctrlChar == EOT) {
			LAST_EOT_RECEIVED = time(0);
			char cur2[16] = "";
			sprintf_s(cur2, "%d", LAST_EOT_RECEIVED);
			OutputDebugString(cur2);
			OutputDebugString("\n");
		}
		else if (ctrlChar == ENQ && !ENQ_FLAG) {
			char ctrlFrame[1024];
			sendFrame(ctrlFrame, nullptr, ACK);
			curState = "RECEIVE";
		}
		else if (ctrlChar == ACK && ENQ_FLAG) {
			curState = "SEND";
		}
	}
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	generateDataFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		void generateDataFrame(char* dataFrame, const char* data) 
--						char* dataFrame - the data frame
--						const char* data - data to send
--
--	RETURNS:		void
--
--	NOTES:
--	Call this to generate a data frame
--	DATA FRAME MAKEUP
--	SYN | DC1/2 | DATA | CRC
--------------------------------------------------------------------------------------*/
void generateDataFrame(char* dataFrame, const char* data) {
	char localData[12] = {};
	*localData = *dataFrame;

	localData[0] = SYN;
	localData[1] = nextFrameToSend;

	for (int i = 0; i < 9; i++) {
		localData[2 + i] = data[i];
	}

	//*dataFrame = *localData;
	for (int i = 0; i < 12; i++) {
		dataFrame[i] = localData[i];
	}

	// strcat CRC to dataframe
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	generateCtrlFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--						November 30, 2018 - changed ctrl to be char
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim, Dasha Strigoun
--
--	INTERFACE:		void generateCtrlFrame(char* ctrlFrame, char ctrl)
--						char* ctrlFrame - the control frame
--						char ctrl - control character to send
--
--	RETURNS:		void
--
--	NOTES:
--	Call this to generate a control frame
--	CTRL FRAME MAKEUP
--	SYN | CTRL | DC1/2
--------------------------------------------------------------------------------------*/
void generateCtrlFrame(char* ctrlFrame, char ctrl) {
	ctrlFrame[0] = SYN;
	ctrlFrame[1] = ctrl;
	ctrlFrame[2] = nextFrameToSend;
}
