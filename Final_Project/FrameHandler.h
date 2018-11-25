#pragma once
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "Main.h"

static char nextFrameToReceive = (char)17;
static char nextFrameToSend = (char)17;
static bool isClearToSend = true;

void receiveFrame(const char* frame);
void sendFrame(char* frame, const char* data, int ctrl);

void readDataFrame(const char* frame);
void readCtrlFrame(const char* frame);

void generateDataFrame(char* dataFrame, const char* data);
void generateCtrlFrame(char* ctrlFrame, int ctrl);