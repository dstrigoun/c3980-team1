#pragma once
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <boost/crc.hpp>
#include "Main.h"

static const char SYN = 22;
static const char DC1 = 17;
static const char DC2 = 18;
static const char ACK = 6;
static const char NAK = 21;
static const char ENQ = 5;
static const char EOT = 4;

static char nextFrameToReceive = DC1;
static char nextFrameToSend = DC1;
static bool isClearToSend = true;

void receiveFrame(const char* frame, HWND* hwnd);
void sendFrame(char* frame, const char* data, char ctrl);

void readDataFrame(const char* frame);
void readCtrlFrame(const char* frame);

void generateDataFrame(char* dataFrame, const char* data);
void generateCtrlFrame(char* ctrlFrame, char ctrl);

boost::uint16_t buildCRC(const char* data);
bool checkCRC(const char* data, boost::uint16_t receivedCRC);
