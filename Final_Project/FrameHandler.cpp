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

// ctrl frame generator
const char* generateCtrlFrame(int ctrl, int dc) {
	char ctrlFrame3[4]="";
	ctrlFrame3[0] = 22;
	ctrlFrame3[1] = ctrl;
	ctrlFrame3[2] = dc;
	char ctrlFrame2[50] = "";
	sprintf_s(ctrlFrame2, "%d%d%d", 22, ctrl, dc);
	return ctrlFrame3;
	
}

//isctrl
// ctrl frame reader
void readCtrlFrame(const char* ctrlFrameToRead) {

	char test = ctrlFrameToRead[0];
	char test1 = ctrlFrameToRead[1];
	char test2 = ctrlFrameToRead[2];
	int synChar = (int)ctrlFrameToRead[0];
	int ctrlChar = (int)ctrlFrameToRead[1];
	int dcChar = (int)ctrlFrameToRead[2];

	char cur[3]="";
	sprintf_s(cur, "%d", test);
	//return ctrlFrame;
	//strcpy_s(output,3,ctrlFrame);
	OutputDebugString(cur);
}



// DATA FRAME MAKEUP
// SYN DC1/2 DATA CRC

// data frame generator

// data frame reader