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
		// frame corrupt, first byte not SYN
	}
}

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
--	INTERFACE:		void sendFrame(char* frame, const char* data, int ctrl)
--						char* frame - the frame to populate
--						const char* data - data to send
--						int ctrl - control character to send
--
--	RETURNS:		void
--
--	NOTES:
--	Call this generic send method and send a frame based on parameters provided.
--------------------------------------------------------------------------------------*/
void sendFrame(char* frame, const char* data, int ctrl) {
	(ctrl == NULL) ? generateCtrlFrame(frame, ctrl)
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

		char data[1021];
		char receivedCRC[2];
		strncpy_s(data, frame + 2, 1020);
		strncpy_s(receivedCRC, frame + 2 + 1020, 1);

		if (receivedCRC[0] == '1') {
			OutputDebugString("wow the dummy bit is good");
		}

		//-----------------------------------------------------
		//char cur[1024] = {};
		//sprintf_s(cur, "original CRC: %x", receivedCRC);
		//OutputDebugString(cur);
		//OutputDebugString("\n");

		////check lastByte CRC if data is corrupt
		//if (checkCRC(data, (boost::uint16_t)receivedCRC)) {
		//	OutputDebugString("CRC passed\n");
		//}
		//OutputDebugString("CRC failed\n");
		//-----------------------------------------------------

		//return the data portion to be appended to file
	}
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	readCtrlFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
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
	// handle behaviour based on control char received
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
	dataFrame[0] = SYN;
	dataFrame[1] = nextFrameToSend;
	strcat_s(dataFrame, 1021, data);

	if ((sizeof(data) / sizeof(data[0])) < 1021) {
		OutputDebugString("data is less than 1021");
		for (int i = 0; i < (1021 - (sizeof(data) / sizeof(data[0]))); i++) {
			//TODO: this doesn't actually work
			char nullChar[2] = { 0 };
			strcat_s(dataFrame, 1021, nullChar);
		}
	}

	char dummyCRC[2] = { 1 };
	strcat_s(dataFrame, 1021, (LPCSTR)dummyCRC);

	//------------------------------------------------------
	//boost::uint16_t var = buildCRC(data);

	//char msgbuf[1024];
	//sprintf_s(msgbuf, "%x", (unsigned)var);
	//OutputDebugString(msgbuf);
	//OutputDebugString("\n");

	//strcat_s(dataFrame, 1021, msgbuf);
	//-------------------------------------------------------
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	generateCtrlFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		void generateCtrlFrame(char* ctrlFrame, int ctrl)
--						char* ctrlFrame - the control frame
--						int ctrl - control character to send
--
--	RETURNS:		void
--
--	NOTES:
--	Call this to generate a control frame
--	CTRL FRAME MAKEUP
--	SYN | CTRL | DC1/2
--------------------------------------------------------------------------------------*/
void generateCtrlFrame(char* ctrlFrame, int ctrl) {
	ctrlFrame[0] = SYN;
	ctrlFrame[1] = ctrl;
	ctrlFrame[2] = nextFrameToSend;
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	buildCRC
--
--	DATE:			November 26, 2018
--
--	REVISIONS:		November 26, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Dasha Strigoun
--
--	INTERFACE:		char* buildCRC(const char* data)
--						const char* data - data to send
--
--	RETURNS:		boost::uint16_t - generated CRC based on data
--
--	NOTES:
--  Call this to build CRC for set of data
--------------------------------------------------------------------------------------*/
boost::uint16_t buildCRC(const char* data) {
	boost::crc_basic<8> result(0x1021, 0xFFFF, 0, false, false);

	result.process_bytes(data, 1021);

	return result.checksum();
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	checkCRC
--
--	DATE:			November 26, 2018
--
--	REVISIONS:		November 26, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Dasha Strigoun
--
--	INTERFACE:		bool checkCRC(const char* data, const char* receivedCRC)
--						const char* data - data to send
--						const char* receivedCRC - CRC checksum at the end of frame
--
--	RETURNS:		bool - whether receivedCRC and calculated CRC match
--
--	NOTES:
--  Call this to check the CRC in the last byte of the data frame
--------------------------------------------------------------------------------------*/
bool checkCRC(const char* data, boost::uint16_t receivedCRC) {
	boost::crc_basic<8> result(0x1021, 0xFFFF, 0, false, false);

	result.process_bytes(data, 1021);

	char expected[64] = {};
	sprintf_s(expected, "expected CRC: %u", (unsigned)receivedCRC);
	OutputDebugString(expected);
	OutputDebugString("\n");

	char msgbuf[1024];
	sprintf_s(msgbuf, "calculated CRC: %u", (unsigned)result.checksum());
	OutputDebugString(msgbuf);
	OutputDebugString("\n");

	return result.checksum() == receivedCRC;
}