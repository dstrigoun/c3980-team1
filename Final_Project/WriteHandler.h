#include <windows.h>
#include "Main.h"
#include "Sender.h"

DWORD WINAPI sendFrame(LPVOID writeParams);

DWORD WINAPI sendEOTs(LPVOID writeParams);