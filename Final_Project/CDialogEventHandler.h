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

HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void **ppv);


IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);
IFACEMETHODIMP_(ULONG) AddRef();
IFACEMETHODIMP_(ULONG) Release();
//IFACEMETHODIMP OnFileOk(IFileDialog *);
//IFACEMETHODIMP OnFolderChange(IFileDialog *);
//IFACEMETHODIMP OnFolderChanging(IFileDialog *, IShellItem *);
//IFACEMETHODIMP OnHelp(IFileDialog *);
//IFACEMETHODIMP OnSelectionChange(IFileDialog *);
//IFACEMETHODIMP OnShareViolation(IFileDialog *, IShellItem *, FDE_SHAREVIOLATION_RESPONSE *);
//IFACEMETHODIMP OnTypeChange(IFileDialog *pfd);
//IFACEMETHODIMP OnOverwrite(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *);
//IFACEMETHODIMP OnItemSelected(IFileDialogCustomize *pfdc, DWORD dwIDCtl, DWORD dwIDItem);
//IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize *, DWORD);
//IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize *, DWORD, BOOL);
//IFACEMETHODIMP OnControlActivating(IFileDialogCustomize *, DWORD);
