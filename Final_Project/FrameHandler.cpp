#include "FrameHandler.h"
#include "ReadThreadParams.h"

DWORD timeoutThreadId;
HANDLE hTimeoutThrd;

/*-------------------------------------------------------------------------------------
--	FUNCTION:	receiveFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--						December 8th, 2018 - add debug statements to log file
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim, Dasha Strigoun
--
--	INTERFACE:		void receiveFrame(const char* frame)
--						const char* frame - frame received
--
--	RETURNS:		void
--
--	NOTES:
--	
--------------------------------------------------------------------------------------*/
void receiveFrame(const char* frame, PREADTHREADPARAMS rtp) 
{
	// check type of frame
	if (frame[0] == SYN) {
		if (frame[1] == DC1 || frame[1] == DC2) {
			//data frame
			readDataFrame(frame);
		}
		else {
			//ctrl frame
			readCtrlFrame(frame, rtp);
		}
	}
	else {
		debugMessage("Frame Corrupt, 1st Byte not SYN");
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
void generateFrame(char* frame, const char* data, char ctrl, PWriteParams wp) {
	char localFrame[3] = {};
	(ctrl != NULL) ? generateCtrlFrame(localFrame, ctrl)
		: generateDataFrame(frame, data);

	(ctrl != NULL) ? wp->frameLen = 3 : wp->frameLen = 1024;
	//copy in frame info to wp char azrr
	for (int i = 0; i < wp->frameLen; i++) {
		wp->frame[i] = localFrame[i];
	}
	sendFrame(wp);
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	readDataFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--						November 27, 2018 - CRC code
--						November 29, 2018 - use dummy CRC byte instead
--						December 5th, 2018 - append data to local file
--						December 8th, 2018 - add debug messages to log file
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim, Dasha Strigoun
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

		//check for dummy CRC bit
		if (frame[1023] == 1) {
			debugMessage("CRC bit is correct");
		}

		// CRC code that does not work
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

		char data[1021] = {};
		for (int i = 0; i < 1021; i++) {
			data[i] = frame[2 + i];
		}

		// Check for EOF (-1) in the data
		int data_size = sizeof(data) / sizeof(*data);

		for (int i = 0; i < data_size; ++i) {
			if (data[i] == -1) {
				debugMessage("Reached EOF in data");

				unfinishedTransmission = false;
				data_size = i;
			}
		}

		std::ofstream file;
		file.open("test.txt", std::fstream::app);
		for (int i = 0; i < data_size; i++) {
			file << data[i];
		}
		file.close();
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
--					December 8th, 2018 - write debug messages to log file
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
void readCtrlFrame(const char* frame, PREADTHREADPARAMS rtp) {
	VariableManager& vm = VariableManager::getInstance();

	char ctrlChar = frame[1];
	char dcChar = frame[2];
	char CurrentSendingCharArrKieran[1024] = {};
	wp->frame = CurrentSendingCharArrKieran;
	wp->portHandle = vm.get_portHandle();

	debugMessage("Current State: " + vm.get_curState());
	std::string tempENQ = (vm.get_ENQ_FLAG()) ? "TRUE" : "FALSE";
	debugMessage("ENQ_FLAG: " + tempENQ);

	// handle behaviour based on control char received
	if (vm.get_curState() == "IDLE") {
		if (ctrlChar == EOT) {
			vm.set_LAST_EOT(time(0));
			debugMessage("Received EOT");
		}
		else if (ctrlChar == ENQ && !(vm.get_ENQ_FLAG())) {
			debugMessage("Received ENQ & sending ACK");

			char ctrlFrame[3]; // if generateFrame ever becomes async, then we have to worry about exiting the scope where this is defined before we acutally send it
			generateFrame(ctrlFrame, nullptr, ACK, wp);

			vm.set_curState("RECEIVE");
			debugMessage("curState is now RECEIVE");

			// start receive timeout thread
			vm.set_LAST_DATA(time(0));
			hTimeoutThrd = CreateThread(NULL, 0, receiveTimeout, 0, 0, &timeoutThreadId);
		}
		else if (ctrlChar == ACK && (vm.get_ENQ_FLAG())) {
			vm.set_curState("SEND");
			unfinishedTransmission = true;

			debugMessage("ENQ was approved, go to SEND state");
		}
	}
	else if (vm.get_curState() == "SEND")
	{
		if (ctrlChar == EOT)
		{
			goToIdle();
		}
	}
	else if (vm.get_curState() == "RECEIVE")
	{
		if (ctrlChar == EOT) 
		{
			goToIdle();
		}
	}
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	generateDataFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--						November 27, 2018 - CRC code
--						November 29, 2018 - use dummy CRC byte instead
--						December 8th, 2018 - generate data frame and switch DC1/DC2
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim, Dasha Strigoun
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
	// use local data frame to append data
	char localData[1024] = {};
	*localData = *dataFrame;

	localData[0] = SYN;
	localData[1] = nextFrameToSend;

	// copy data into local data frame
	for (int i = 0; i < 1021; i++) {
		localData[2 + i] = data[i];
	}
	
	// copy local data frame into data frame
	for (int i = 0; i < 1024; i++) {
		dataFrame[i] = localData[i];
	}

	//append the dummy CRC bit
	char dummyCRC = 1;
	dataFrame[1023] = dummyCRC;

	debugMessage("Generated CRC bit is: " + dummyCRC);

	// CRC code that does not work
	//------------------------------------------------------
	//boost::uint16_t var = buildCRC(data);

	//char msgbuf[1024];
	//sprintf_s(msgbuf, "%x", (unsigned)var);
	//OutputDebugString(msgbuf);
	//OutputDebugString("\n");

	//strcat_s(dataFrame, 1021, msgbuf);
	//-------------------------------------------------------


	nextFrameToSend = (nextFrameToSend == DC1) ? DC2 : DC1;
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
//boost::uint16_t buildCRC(const char* data) {
//	boost::crc_basic<8> result(0x1021, 0xFFFF, 0, false, false);
//
//	result.process_bytes(data, 1021);
//
//	return result.checksum();
//}

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
//bool checkCRC(const char* data, boost::uint16_t receivedCRC) {
//	boost::crc_basic<8> result(0x1021, 0xFFFF, 0, false, false);
//
//	result.process_bytes(data, 1021);
//
//	char expected[64] = {};
//	sprintf_s(expected, "expected CRC: %u", (unsigned)receivedCRC);
//	OutputDebugString(expected);
//	OutputDebugString("\n");
//
//	char msgbuf[1024];
//	sprintf_s(msgbuf, "calculated CRC: %u", (unsigned)result.checksum());
//	OutputDebugString(msgbuf);
//	OutputDebugString("\n");
//
//	return result.checksum() == receivedCRC;
//}


/*-------------------------------------------------------------------------------------
--	FUNCTION:	receiveTimeout
--
--	DATE:			December 8, 2018
--
--	REVISIONS:		December 8, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Dasha Strigoun
--
--	INTERFACE:		DWORD WINAPI receiveTimeout(LPVOID n)
--						LPVOID n: void pointer to thread param
--
--	RETURNS:		returns 0
--
--	NOTES:
--	Called by the thread created to periodically check the difference between
--  the current time and the time when the last data frame was received
--------------------------------------------------------------------------------------*/
DWORD WINAPI receiveTimeout(LPVOID n)
{
	VariableManager& vm = VariableManager::getInstance();
	while (vm.get_curState() == "RECEIVE") {
		time_t currentTime = time(0);
		if (currentTime - vm.get_LAST_DATA() > RECEIVE_TIMEOUT_TIME_S)
		{
			goToIdle();
		}
		int difference = currentTime - vm.get_LAST_DATA();
		std::stringstream message;
		message << "Last data frame was " << difference << " seconds ago";
		debugMessage(message.str());
		Sleep(CHECK_IDLE_TIMEOUT_MS);
	}
	return 0;
}
