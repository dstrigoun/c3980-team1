#include "FrameHandler.h"
#include "ReadThreadParams.h"
#include "fileChooser.h"

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
void receiveFrame(const char* frame, PREADTHREADPARAMS rtp) {

	// check type of frame
	if (frame[0] == SYN) {
		//MessageBox(*(rtp->hwnd), "SYN\n", "SYN\n", MB_OK);


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
		OutputDebugString("Frame Corrupt, 1st Byte not SYN\n");
		//MessageBox(*rtp->hwnd, "Frame Corrupt, 1st Byte not SYN\n", "Frame Corrupt, 1st Byte not SYN\n", MB_OK);

		std::ofstream file;
		file.open("log.txt", std::fstream::app);
		file << time(0) << ": \tFrame Corrupt, 1st Byte not SYN\n";
		file.close();
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
void generateFrame(const char* data, char ctrl, PWriteParams wp) {
	char localCtlFrame[3] = {};
	char localDataFrame[1024] = {};
	(ctrl != NULL) ? generateCtrlFrame(localCtlFrame, ctrl)
		: generateDataFrame(localDataFrame, data);
	
	(ctrl != NULL) ? wp->frameLen = 3 : wp->frameLen = 1024;
	
	if (ctrl != NULL) {
		for (int i = 0; i < wp->frameLen; i++) {
			wp->frame[i] = localCtlFrame[i];
		}
	}
	else {
		for (int i = 0; i < wp->frameLen; i++) {
			wp->frame[i] = localDataFrame[i];
		}
	}
	
	
	sendFrame(wp);
	
	//start sender thread here with the above created frame
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
	std::ofstream log_file;

	if (false /*frame[1] != nextFrameToReceive*/) { //tempeoaraliy made this always fail
		//duplicate frame 
	}
	else {
		//alternate nextFrameToReceive between DC1 and DC2 for duplicate checks
		nextFrameToReceive = (frame[1] == DC1) ? DC2 : DC1;

		//check for dummy CRC bit
		if (frame[1023] == 1) {
			OutputDebugString("Dummy CRC bit works\n");
			std::ofstream log_file;
			log_file.open("log.txt", std::fstream::app);
			log_file << time(0) << ":\tCRC bit is correct.\n";
			log_file.close();
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

		OutputDebugString("in readDataFrame");

		char data[1021] = {};
		for (int i = 0; i < 1021; i++) {
			data[i] = frame[2 + i];
		}

		// Check for EOF (-1) in the data
		//int data_size = sizeof(data) / sizeof(*data);
		int data_size = 1021;

		for (int i = 0; i < data_size; ++i) {
			log_file.open("log.txt", std::fstream::app);
			log_file << data[i];
			log_file.close();

			if (data[i] == -1) {
				OutputDebugString("Found EOF in data\n");

				std::ofstream log_file;
				log_file.open("log.txt", std::fstream::app);
				log_file << time(0) << ":\n\tReached EOF in data.\n";
				log_file.close();

				unfinishedTransmission = false;
				data_size = i;
				break;
			}
		}

		/*std::ofstream file;
		file.open("test.txt", std::fstream::app);
		for (int i = 0; i < data_size; i++) {
			file << data[i];
		}
		file.close();*/
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


	std::ofstream afile;
	afile.open("log.txt", std::fstream::app);
	afile << time(0) << ": \tCurrent state: " << vm.get_curState() << "\n";
	afile << time(0) << ": \tENQ_FLAG: " << vm.get_ENQ_FLAG() << "\n";
	afile.close();

	// handle behaviour based on control char received
	if (vm.get_curState() == "IDLE") {
		if (ctrlChar == EOT) {
			LAST_EOT_RECEIVED = time(0);
			char cur2[16] = "";
			sprintf_s(cur2, "%d", LAST_EOT_RECEIVED);
			updateLastEOTReceived(time(0));
			OutputDebugString(cur2);
			OutputDebugString("\n");

			std::ofstream file;
			file.open("log.txt", std::fstream::app);
			file << time(0) << ": \tReceived EOT\n";
			file.close();
		}
		else if (ctrlChar == ENQ && !(vm.get_ENQ_FLAG())) {
			
			std::ofstream file;
			file.open("log.txt", std::fstream::app);
			file << time(0) << ": \tReceived ENQ & SENDING ACK, BEFORE GENERATE FRAME\n";
			file.close();
			generateFrame(nullptr, ACK, wp);
			file.open("log.txt", std::fstream::app);
			file << time(0) << ": \tReceived ENQ & SENDING ACK, AFTERGENERATE FRAME\n";
			file.close();
			vm.set_curState("RECEIVE");

			file.open("log.txt", std::fstream::app);
			file << time(0) << ": \tReceived ENQ, go to RECEIVE\n";
			file.close();
		}
		else if (ctrlChar == ACK && (vm.get_ENQ_FLAG())) {
			vm.set_curState("SEND");
			unfinishedTransmission = true;
			//MessageBox(*rtp->hwnd, "Send State", "Send State", MB_OK);


			std::ofstream file;
			file.open("log.txt", std::fstream::app);
			file << time(0) << ": \tENQ was approved, go to SEND\n";
			file.close();

			//send the first data frame
			
			char* payload = getPayload();

			file.open("log.txt", std::fstream::app);
			file << time(0) << ": \tGot payload\n";
			file.close();

			generateFrame(payload, NULL, wp);
		}
	}
	else if (vm.get_curState() == "SEND") {
		if (ctrlChar == ACK) {
			char* payload = getPayload();
			generateFrame(payload, NULL, wp);
		}
	}
	else if (vm.get_curState() == "RECEIVE") {
		if (ctrlChar == DC1 || ctrlChar == DC2) {
			bool lastDataFrame = false;
			std::ofstream file;
			file.open("log.txt", std::fstream::app);
			file << time(0) << ": \t Data content: ";
			for (int i = 2; i < 1022; i++) {
				if (frame[i] == EOF) {
					lastDataFrame = true;
					break;
				}
				file << frame[i];
				
			}
			file << std::endl;
			file.close();

			generateFrame(nullptr, ACK, wp);

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

	std::ofstream log_file;
	log_file.open("log.txt", std::fstream::app);
	log_file << time(0) << ":\tGenerated CRC bit is: " << dummyCRC << "\n";
	log_file.close();

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
