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
void generateCtrlFrame(char* output, int ctrl, int dc) {
	char ctrlFrame[3];
	ctrlFrame[0] = 22;
	ctrlFrame[1] = ctrl;
	ctrlFrame[2] = dc;
	strcpy(output,ctrlFrame);
	OutputDebugString(output);
}

// ctrl frame reader

// DATA FRAME MAKEUP
// SYN DC1/2 DATA CRC

// data frame generator

// data frame reader