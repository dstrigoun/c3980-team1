#pragma once

#include <Windows.h>
#include <string>
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
private:
	VariableManager() {}

	bool ENQ_FLAG = false;
	HWND hwnd;
	HANDLE portHandle;
	std::string curState;

};