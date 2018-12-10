#pragma once
#include <windows.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
//#include <boost/crc.hpp>
#include "Main.h"
#include "WriteParams.h"
#include "ReadThreadParams.h"

static const char SYN = 22;
static const char DC1 = 17;
static const char DC2 = 18;
static const char DC4 = 20;
static const char ACK = 6;
static const char NAK = 21;
static const char ENQ = 5;
static const char EOT = 4;
static const char SMILEY = 2;

static char nextFrameToReceive = DC1;
static char nextFrameToSend = DC1;
static bool isClearToSend = true;

void receiveFrame(const char* frame, PREADTHREADPARAMS rtp);
void generateFrame(const char* data, char ctrl, PWriteParams wp);


void readDataFrame(const char* frame, DWORD numBytesSent, bool firstPartOfFrame);
void readCtrlFrame(const char* frame, PREADTHREADPARAMS rtp);

void generateDataFrame(char* dataFrame, const char* data);
void generateCtrlFrame(char* ctrlFrame, char ctrl);

//boost::uint16_t buildCRC(const char* data);
//bool checkCRC(const char* data, boost::uint16_t receivedCRC);
