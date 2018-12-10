#pragma once
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <new>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <atlbase.h>
#include <AtlConv.h>
#include <vector>
#include <string>

#include "VariableManager.h"

using namespace std;
const int NUM_PAYLOAD_BYTES = 1021;

ifstream openFile(HWND *hWnd);
char* getPayload();

