#pragma once
#include <windows.h>
#include <string.h>

char* generateCtrlFrame(int ctrl, int dc);
void readCtrlFrame(char* ctrlFrameToRead);