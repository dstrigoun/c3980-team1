#pragma once
#include <windows.h>
#include <stdio.h>
#include <string.h>

char nextFrameToReceive = (char)17;
char nextFrameToSend = (char)17;

void receiveFrame(const char* frame);
void sendFrame(const char* frame);

void readDataFrame(const char* frame);
void readCtrlFrame(const char* frame);

const char* generateDataFrame(char* data);
const char* generateCtrlFrame(int ctrl, int dc);