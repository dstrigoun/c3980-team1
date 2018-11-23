#pragma once
#include <windows.h>
#include <stdio.h>
#include <string.h>

const char* generateCtrlFrame(int ctrl, int dc);
void readCtrlFrame(const char* ctrlFrameToRead);