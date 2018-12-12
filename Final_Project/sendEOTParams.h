#pragma once
#include <Windows.h>
#include"WriteParams.h"
typedef struct sendEOTParams {
	HANDLE stopThreadEvent;
	PWriteParams wp;
	sendEOTParams(HANDLE stopThreadEvent, PWriteParams wp) : stopThreadEvent(stopThreadEvent) {
		this->wp = wp;
	}
}sendEOTParams, *PsendEOTParams;