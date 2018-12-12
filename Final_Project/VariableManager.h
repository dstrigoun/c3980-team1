#pragma once

#include <Windows.h>
#include <string>
#include <fstream>
/*
curr state
enq flag
port handle
hwnd


unfinished transission
currently open file stream
last eot received

wp?
*/

class VariableManager {
public:
	static VariableManager& getInstance() {
		static VariableManager instance;
		return instance;
	}
	VariableManager(VariableManager const&) = delete;
	void operator=(VariableManager const&) = delete;

	void set_ENQ_FLAG(bool flag) { this->ENQ_FLAG = flag; }
	bool get_ENQ_FLAG() { return this->ENQ_FLAG; }

	void set_hwnd(HWND hwnd) { this->hwnd = hwnd; }
	HWND get_hwnd() { return this->hwnd; }

	void set_portHandle(HANDLE portHandle) { this->portHandle = portHandle; }
	HANDLE get_portHandle() { return this->portHandle; }

	void set_curState(std::string curState) { this->curState = curState; }
	std::string get_curState() { return this->curState; }

	void reset_numFramesSent() { this->numFramesSent = 0; }
	void increment_numFramesSent() { this->numFramesSent++; }
	int get_numFramesSent() { return this->numFramesSent; }

	void reset_numFramesReSent() { this->numFramesReSent = 0; }
	void increment_numFramesReSent() { this->numFramesReSent++; }
	int get_numFramesReSent() { return this->numFramesReSent; }

	void set_LAST_EOT(time_t current) { this->LAST_EOT_RECEIVED = current; }
	DWORD get_LAST_EOT() { return this->LAST_EOT_RECEIVED; }

	void set_LAST_DATA(time_t current) { this->LAST_DATA_FRAME_RECEIVED = current; }
	DWORD get_LAST_DATA() { return this->LAST_DATA_FRAME_RECEIVED; }

	void set_EOT_frame(char* frame) { this->EOT_frame = frame; }
	char* get_EOT_frame() { return this->EOT_frame; }

	void set_ENQ_frame(char* frame) { this->ENQ_frame = frame; }
	char* get_ENQ_frame() { return this->ENQ_frame; }

	void set_ACK_0_frame(char* frame) { this->ACK_frame_0 = frame; }
	char* get_ACK_0_frame() { return this->ACK_frame_0; }

	void set_ACK_1_frame(char* frame) { this->ACK_frame_1 = frame; }
	char* get_ACK_1_frame() { return this->ACK_frame_1; }

	void set_NAK_frame(char* frame) { this->NAK_frame = frame; }
	char* get_NAK_frame() { return this->NAK_frame; }
	
	void set_currUploadFile(std::ifstream* currUploadFile) { this->currUploadFile = currUploadFile; }
	std::ifstream* get_currUploadFile() { return this->currUploadFile; }

	void set_countDataFrameBytesRead(int i) {
		countDataFrameBytesRead = i;
	}
	int get_countDataFrameBytesRead() { return countDataFrameBytesRead; }

	void set_lastFrameSent(LPCSTR lastFrame) {
		this->lastFrameSent = lastFrame;
	}
	LPCSTR get_lastFrameSent(){ return this->lastFrameSent; }

	bool isMaxFramesSent() {
		return this->numFramesSent >= MAX_FRAMES_SENT;
	}
	bool isMaxResends() {
		return this->numFramesReSent >= MAX_RESENDS;
	}

	int get_numACKReceived() { return this->numACKReceived; }
	void set_numACKReceived(int num) { this->numACKReceived; }

private:
	VariableManager() {}

	bool ENQ_FLAG = false;
	HWND hwnd;
	HANDLE portHandle;
	std::string curState;
	int numFramesSent;
	int numFramesReSent;
	DWORD LAST_EOT_RECEIVED;
	DWORD LAST_DATA_FRAME_RECEIVED;
	LPCSTR lastFrameSent;
	const int MAX_RESENDS = 3;
	const int MAX_FRAMES_SENT = 10;
	int numACKReceived;

	std::ifstream* currUploadFile; //later make a queue of filestreams for other multiple file uploads
	int countDataFrameBytesRead;

	char* EOT_frame;
	char* ENQ_frame;
	char* ACK_frame_0;
	char* ACK_frame_1;
	char* NAK_frame;
};