#include "FrameHandler.h"
#include "ReadThreadParams.h"
#include "fileChooser.h"

DWORD timeoutThreadId;
HANDLE hTimeoutThrd;
HANDLE displayThrd;
DWORD displayThreadId;

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
--	INTERFACE:		void receiveFrame(const char* frame, PREADTHREADPARAMS rtp)
--						const char* frame - frame received
--						PREADTHREADPARAMS rtp - struct with stopThreadEvent and number of
--							Bytes to be read
--
--	RETURNS:		void
--
--	NOTES:
--	Call this generic function to receive a frame that was sent. Type will be determined
--		here, control or data.
--------------------------------------------------------------------------------------*/
void receiveFrame(const char* frame, PREADTHREADPARAMS rtp) {
	VariableManager &vm = VariableManager::getInstance();
	if (vm.get_countDataFrameBytesRead() > 0
		&& !(frame[1] == DC1 || frame[1] == DC2)) {
		vm.set_countDataFrameBytesRead((int)(vm.get_countDataFrameBytesRead() + *(rtp->numBytesRead)));
		readDataFrame(frame, *(rtp->numBytesRead), false);
		if (vm.get_countDataFrameBytesRead() == 1024) {
			vm.set_countDataFrameBytesRead(0);
			if (vm.get_isDuplicate()) {
				generateAndSendFrame(ACK, wp);
				vm.set_isDuplicate(false);
			}
			else {
				vm.set_nextFrameToReceive((vm.get_nextFrameToReceive() == DC1) ? DC2 : DC1);
				generateAndSendFrame(ACK, wp);
			}
		}
	}
	// check type of frame
	if (frame[0] == SYN) {
		if (vm.get_curState()!="IDLE" && (frame[1] == DC1 || frame[1] == DC2)) {
			//data frame
			vm.set_countDataFrameBytesRead((int)(vm.get_countDataFrameBytesRead() + *(rtp->numBytesRead)));
			readDataFrame(frame, *(rtp->numBytesRead), true);
			if (vm.get_countDataFrameBytesRead() == 1024) {
				vm.set_countDataFrameBytesRead(0);
				if (vm.get_isDuplicate()) {
					generateAndSendFrame(ACK, wp);
					vm.set_isDuplicate(false);
				}
				else {
					vm.set_nextFrameToReceive((frame[1] == DC1) ? DC2 : DC1);
					generateAndSendFrame(ACK, wp);
				}
			}
		}
		else {
			//ctrl frame
			readCtrlFrame(frame, rtp);
		}
	}
	else {
	}
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	generateAndSendFrame
--
--	DATE:			November 24, 2018
--
--	REVISIONS:		November 24, 2018
--					November 30, 2018 - changed ctrl to be char
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim, Dasha Strigoun
--
--	INTERFACE:		void generateAndSendFrame(char ctrl, PWriteParams wp)
--						const char* data - data to send
--						char ctrl - control character to send
--						PWriteParams wp - struct with frame to be written
--
--	RETURNS:		void
--
--	NOTES:
--	Call this generic send function to generate a frame to be sent, control or data
--------------------------------------------------------------------------------------*/
void generateAndSendFrame(char ctrl, PWriteParams wp) {

	VariableManager &vm = VariableManager::getInstance();
	
	char localCtlFrame[3] = {};
	char localDataFrame[1024] = {};
	
	if (ctrl != NULL) {
		wp->frameLen = 3;
		wp->frame = localCtlFrame;
		generateCtrlFrame(localCtlFrame, ctrl);
	}
	else {
		wp->frameLen = 1024;
		wp->frame = localDataFrame;
		generateDataFrame(localDataFrame);
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
--	INTERFACE:		void readDataFrame(const char* frame, DWORD numBytesRead, bool firstPartOfFrame) 
--						const char* frame - data frame to read
--						DWORD numBytesRead - number of Bytes to be read
--
--	RETURNS:		void
--
--	NOTES:
--	Call this to read a data frame and handle the data retrieved from the frame
--------------------------------------------------------------------------------------*/
void readDataFrame(const char* frame, DWORD numBytesRead, bool firstPartOfFrame) {
	VariableManager &vm = VariableManager::getInstance();
	
	if (frame[1] == DC1 || frame[1] == DC2) {
		if (frame[1] != vm.get_nextFrameToReceive()) { //tempeoaraliy made this always fail
			//duplicate frame 
			debugMessage("Received Duplicate");
			vm.set_isDuplicate(true);
		}
		else {
			vm.set_isDuplicate(false);
		}
		
		std::uint8_t crc = CRC::Calculate(frame + 2, 1022, CRC::CRC_8());
		

		char data[1021] = {};

		if (firstPartOfFrame) {
			for (int i = 0; i < 1021; i++) {
				data[i] = frame[2 + i];
			}
		}
		else {
			for (int i = 0; i < 1021; i++) {
				data[i] = frame[i];
			}
		}
		int data_size = 0;
		if (firstPartOfFrame) {
			data_size = numBytesRead - 2;
		}
		else {
			data_size = numBytesRead;
		}

		std::ofstream file;
		if (!vm.get_isDuplicate()) {
			file.open("receive_data.txt", std::fstream::app);
			for (int i = 0; i < data_size; i++) {
				file << data[i];
				if (data[i] == -1) {
					debugMessage("Reached EOF in data");
					vm.set_unfinishedTransmission(false);
					data_size = i;
					break;
				}
			}
			file.close();
		}
		vm.set_LAST_DATA(time(0));
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
--					December 11th, 2018 - increment ACK and NAK received counts
--												and added displayThrd
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim, Alexander Song, Dasha Strigoun
--
--	INTERFACE:		void readCtrlFrame(const char* frame, PREADTHREADPARAMS rtp)
--						const char* frame - control frame to read
--						PREADTHREADPARMS rtp - read thread being used
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
	debugMessage("Received CTRL in State: " + vm.get_curState());
	std::string tempENQ = (vm.get_ENQ_FLAG()) ? "TRUE" : "FALSE";
	debugMessage("ENQ_FLAG: " + tempENQ);


	char *temp = new char[4];
	strncpy_s(temp, 4, frame, 3);

	std::stringstream message;
	message << "Received Frame: " << (LPSTR)temp << std::endl;
	debugMessage(message.str());
	
	delete [] temp;

	// handle behaviour based on control char received
	if (vm.get_curState() == "IDLE") {
		displayThrd = CreateThread(NULL, 0, displayStats, vm.get_hwnd(), 0, &displayThreadId);

		if (ctrlChar == EOT) {
			vm.set_LAST_EOT(time(0));
		}

		else if (ctrlChar == ENQ && !(vm.get_ENQ_FLAG())) {

			char ctrlFrame[3]; // if generateFrame ever becomes async, then we have to worry about exiting the scope where this is defined before we acutally send it
			generateAndSendFrame( ACK, wp);

			SetEvent(*(vm.get_stopEOTThreadEvent()));
			vm.set_curState("RECEIVE");
			// start receive timeout thread
			vm.set_LAST_DATA(time(0));
			hTimeoutThrd = CreateThread(NULL, 0, receiveTimeout, 0, 0, &timeoutThreadId);
		}
		else if (ctrlChar == ACK && (vm.get_ENQ_FLAG())) {
			SetEvent(vm.get_stopTransmitTimeoutThreadEvent());
			vm.set_curState("SEND");
			vm.set_unfinishedTransmission(true);
			vm.set_numACKReceived(vm.get_numACKReceived() + 1);

			debugMessage("Sending payload");
			//send the first data frame
			generateAndSendFrame(NULL, wp);
			
		}
	}
	else if (vm.get_curState() == "SEND")
	{
		switch (ctrlChar)
		{
		case EOT:
			if (vm.get_hasSentEOT()) {
				goToIdle();
			}
		case ACK:
			vm.set_numACKReceived(vm.get_numACKReceived() + 1); // increment ACK count
			if (vm.get_hitEOF()) {
				vm.set_unfinishedTransmission(false);
				wp->frame = vm.get_EOT_frame();
				wp->frameLen = 3;
				sendFrame(wp);
				vm.set_hasSentEOT(true);
				goToIdle();
				vm.get_currUploadFile()->close();
				vm.set_currUploadFile(nullptr);
				vm.set_hitEOF(false);
				break;
			}

			//update DC1/DC2
			vm.set_nextFrameToSend((vm.get_nextFrameToSend() == DC1) ? DC2 : DC1);
			//trigger send frame
			
			if (!vm.isMaxFramesSent()) {
				generateAndSendFrame(NULL, wp);
			}
			else {
				debugMessage("Reached Max Number of Frames Sent");
				wp->frame = vm.get_EOT_frame();
				wp->frameLen = 3;
				sendFrame(wp);
				vm.set_hasSentEOT(true);
				goToIdle();
			}
			break;
		case NAK:
			vm.set_numNAKReceived(vm.get_numNAKReceived() + 1); // increment NAK count
			//trigger resend frame
			resendDataFrame();
			break;
		}
	}
	else if (vm.get_curState() == "RECEIVE") {
		switch (ctrlChar)
		{
		case ACK:
			// increment ACK count
			vm.set_numACKReceived(vm.get_numACKReceived() + 1);
			break;
		}

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
void generateDataFrame(char* dataFrame) {
	// use local data frame to append data

	VariableManager &vm = VariableManager::getInstance();

	char localData[1024] = {};
	*localData = *dataFrame;
	char* data = (char*)getPayload();

	localData[0] = SYN;
	localData[1] = vm.get_nextFrameToSend();

	// copy data into local data frame
	for (int i = 0; i < 1021; i++) {
		localData[2 + i] = data[i];
	}
	
	// copy local data frame into data frame
	for (int i = 0; i < 1024; i++) {
		dataFrame[i] = localData[i];
		vm.get_lastFrameSent()[i] = localData[i];
	}

	//append the CRC bit
	std::uint8_t crc = CRC::Calculate(dataFrame + 2, 1021, CRC::CRC_8());
	dataFrame[1023] = crc;
	vm.get_lastFrameSent()[1023] = crc;
	debugMessage("Generated CRC bit is: " + crc);
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

	VariableManager &vm = VariableManager::getInstance();

	ctrlFrame[0] = SYN;
	ctrlFrame[1] = ctrl;
	ctrlFrame[2] = vm.get_nextFrameToSend();

	std::stringstream message;
	message << "Generate CTRL frame: " << (LPSTR)ctrlFrame << std::endl;
	debugMessage(message.str());
}

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
	ExitThread(0);
	return 0;
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	transmissionTimeout
--
--	DATE:			December 12, 2018
--
--	REVISIONS:		December 12, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Jason Kim
--
--	INTERFACE:		DWORD WINAPI transmissionTimeout(LPVOID n)
--						LPVOID n: void pointer to thread param
--
--	RETURNS:		returns 0
--
--	NOTES:
--	Called by the thread created to periodically check the difference between
--  the current time and the time when last transmission was sent
--------------------------------------------------------------------------------------*/
DWORD WINAPI transmissionTimeout(LPVOID n)
{
	VariableManager& vm = VariableManager::getInstance();
	time_t startTime = time(0);
	time_t currentTime = time(0);

	DWORD waitResult;

	while (vm.get_curState() != "RECEIVE") {
		waitResult = WaitForSingleObject(*(vm.get_stopTransmitTimeoutThreadEvent()), 10);
		switch (waitResult)
		{
		case WAIT_TIMEOUT:
			currentTime = time(0);
			if (currentTime - startTime > RETRANSMISSION_TIMEOUT_TIME_S)
			{
				goToIdle();
				ExitThread(0);
			}
			break;
		case WAIT_OBJECT_0:
			ExitThread(0);
			break;
		default:
			break;
		}
	}
	ExitThread(0);
	return 0;
}

/*-------------------------------------------------------------------------------------
--	FUNCTION:	displayStats
--
--	DATE:			December 11, 2018
--
--	REVISIONS:		December 11, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Alexander Song
--
--	INTERFACE:		DWORD WINAPI displayStats(LPVOID hwnd)
--						LPVOID hwnd: void pointer to the handle of the window
--
--	RETURNS:		returns 0
--
--	NOTES:
--	Called by the thread created to periodically check and update the number of packets
--    sent, bit error rate, number of ACKs received and number of NAKs received.
--------------------------------------------------------------------------------------*/
DWORD WINAPI displayStats(LPVOID hwnd)
{
	VariableManager& vm = VariableManager::getInstance();
	const int char_height{ 15 };
	const int char_width{ 8 };
	HDC hdc;

	while (1)
	{
		char ackStats[1024];
		char nakStats[1024];
		char packetStats[1024];
		char berStats[1024];

		sprintf_s(packetStats, "Packets sent: %d", vm.get_numPacketsSent());
		sprintf_s(berStats, "Bit Error Rate: %0.4f", vm.get_BER());
		sprintf_s(ackStats, "ACKs received: %d", vm.get_numACKReceived());
		sprintf_s(nakStats, "NAKs received: %d", vm.get_numNAKReceived());

		hdc = GetDC((HWND)hwnd);
		TextOut(hdc, 0, 0, packetStats, strlen(packetStats));
		TextOut(hdc, 0, char_height, berStats, strlen(berStats));
		TextOut(hdc, 0, 2 * char_height, ackStats, strlen(ackStats));
		TextOut(hdc, 0, 3 * char_height, nakStats, strlen(nakStats));
		ReleaseDC((HWND)hwnd, hdc);

		Sleep(500);
	}
	return 0;


}