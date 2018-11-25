#include "FrameHandler.h"
// check if control or data frame

//ascii dec codes
// SYN  22
// DC1	17
// DC2	18
// ACK	6
// NAK	21
// ENQ	5
// EOT	4

// CTRL FRAME MAKEUP
// SYN CTRL DC1/2

// DATA FRAME MAKEUP
// SYN DC1/2 DATA CRC

void receiveFrame(const char* frame) {

	// check type of frame
	if (frame[0] == (char)22) {
		if (frame[1] == (char)17 || frame[1] == (char)18) {
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

void sendFrame(const char* frame) {
	//start sender thread here?
}

// data frame reader
void readDataFrame(const char* frame) {
	if (frame[1] != nextFrameToReceive) {
		//duplicate frame 
	}
	else {
		//alternate nextFrameToReceive between DC1 and DC2 for duplicate checks
		nextFrameToReceive = (frame[1] == (char)17) ? (char)18 : (char)17;

		//check lastByte CRC if data is corrupt

		char data[1021];
		strncpy(data, frame + 2, 1020);
		//return the data portion to be appended to file
	}
}

// ctrl frame reader
void readCtrlFrame(const char* frame) {
	char test1 = frame[1];
	char test2 = frame[2];
	int ctrlChar = (int)frame[1];
	int dcChar = (int)frame[2];
}

// data frame generator
const char* generateDataFrame(char* data) {
	char dataFrame[1024];
	dataFrame[0] = 22;
	dataFrame[1] = nextFrameToSend;
	strcat(dataFrame, data);
	// strcat CRC to dataframe
	// send dataFrame
}

// ctrl frame generator
const char* generateCtrlFrame(int ctrl, int dc) {
	char ctrlFrame3[4] = "";
	ctrlFrame3[0] = 22;
	ctrlFrame3[1] = ctrl;
	ctrlFrame3[2] = dc;
	char ctrlFrame2[50] = "";
	sprintf_s(ctrlFrame2, "%d%d%d", 22, ctrl, dc);
	return ctrlFrame3;
}