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

		//check lastByte CRC if data is corrupt
		if (checkCRC(data, receivedCRC)) {
			//CRC passed
		}

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
	//strcat_s(dataFrame, 4, (char*)buildCRC(data));

	char cur[64] = {};
	sprintf_s(cur, "%x", buildCRC(data));
	OutputDebugString(cur);
	OutputDebugString("\n");
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
	boost::crc_32_type result;

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
bool checkCRC(const char* data, const char* receivedCRC) {
	boost::crc_32_type result;

	result.process_bytes(data, 1021);

	return (char*)result.checksum() == receivedCRC;
}