#pragma once
#include "CDialogEventHandler.h"

class CDialogEventHandler : public IFileDialogEvents,
	public IFileDialogControlEvents
{
public:
	#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


	#define INDEX_TEXTDOC 3

	// Controls
	#define CONTROL_GROUP           2000
	#define CONTROL_RADIOBUTTONLIST 2
	#define CONTROL_RADIOBUTTON1    1
	#define CONTROL_RADIOBUTTON2    2       // It is OK for this to have the same ID as CONTROL_RADIOBUTTONLIST,
										// because it is a child control under CONTROL_RADIOBUTTONLIST

	// IDs for the Task Dialog Buttons
	#define IDC_BASICFILEOPEN                       100
	#define IDC_ADDITEMSTOCUSTOMPLACES              101
	#define IDC_ADDCUSTOMCONTROLS                   102
	#define IDC_SETDEFAULTVALUESFORPROPERTIES       103
	#define IDC_WRITEPROPERTIESUSINGHANDLERS        104
	#define IDC_WRITEPROPERTIESWITHOUTUSINGHANDLERS 105


	// IUnknown methods
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		static const QITAB qit[] = {
			QITABENT(CDialogEventHandler, IFileDialogEvents),
			QITABENT(CDialogEventHandler, IFileDialogControlEvents),
			{ 0 },
		};
		return QISearch(this, qit, riid, ppv);
	}

	IFACEMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	IFACEMETHODIMP_(ULONG) Release()
	{
		long cRef = InterlockedDecrement(&_cRef);
		if (!cRef)
			delete this;
		return cRef;
	}

	// IFileDialogEvents methods
	IFACEMETHODIMP OnFileOk(IFileDialog *) { return S_OK; };
	IFACEMETHODIMP OnFolderChange(IFileDialog *) { return S_OK; };
	IFACEMETHODIMP OnFolderChanging(IFileDialog *, IShellItem *) { return S_OK; };
	IFACEMETHODIMP OnHelp(IFileDialog *) { return S_OK; };
	IFACEMETHODIMP OnSelectionChange(IFileDialog *) { return S_OK; };
	IFACEMETHODIMP OnShareViolation(IFileDialog *, IShellItem *, FDE_SHAREVIOLATION_RESPONSE *) { return S_OK; };
//	IFACEMETHODIMP OnTypeChange(IFileDialog *pfd);
	IFACEMETHODIMP OnOverwrite(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *) { return S_OK; };

	// IFileDialogControlEvents methods
	//IFACEMETHODIMP OnItemSelected(IFileDialogCustomize *pfdc, DWORD dwIDCtl, DWORD dwIDItem);
	IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize *, DWORD) { return S_OK; };
	IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize *, DWORD, BOOL) { return S_OK; };
	IFACEMETHODIMP OnControlActivating(IFileDialogCustomize *, DWORD) { return S_OK; };

	
	// IFileDialogEvents methods
// This method gets called when the file-type is changed (combo-box selection changes).
// For sample sake, let's react to this event by changing the properties show.
	HRESULT OnTypeChange(IFileDialog *pfd)
	{
		IFileSaveDialog *pfsd;
		HRESULT hr = pfd->QueryInterface(&pfsd);
		if (SUCCEEDED(hr))
		{
			UINT uIndex;
			hr = pfsd->GetFileTypeIndex(&uIndex);   // index of current file-type
			if (SUCCEEDED(hr))
			{
				IPropertyDescriptionList *pdl = NULL;

				switch (uIndex)
				{
				case INDEX_TEXTDOC:
					// When .txt is selected, let's ask for some other arbitrary property, say Author.
					hr = PSGetPropertyDescriptionListFromString(L"prop:System.Author", IID_PPV_ARGS(&pdl));
					if (SUCCEEDED(hr))
					{
						// TRUE as second param == show default properties as well, but show Author property first in list.
						hr = pfsd->SetCollectedProperties(pdl, TRUE);
						pdl->Release();
					}
					break;
				}
			}
			pfsd->Release();
		}
		return hr;
	}

	// IFileDialogControlEvents
	// This method gets called when an dialog control item selection happens (radio-button selection. etc).
	// For sample sake, let's react to this event by changing the dialog title.
	HRESULT OnItemSelected(IFileDialogCustomize *pfdc, DWORD dwIDCtl, DWORD dwIDItem)
	{
		IFileDialog *pfd = NULL;
		HRESULT hr = pfdc->QueryInterface(&pfd);
		if (SUCCEEDED(hr))
		{
			if (dwIDCtl == CONTROL_RADIOBUTTONLIST)
			{
				switch (dwIDItem)
				{
				case CONTROL_RADIOBUTTON1:
					hr = pfd->SetTitle(L"Longhorn Dialog");
					break;

				case CONTROL_RADIOBUTTON2:
					hr = pfd->SetTitle(L"Vista Dialog");
					break;
				}
			}
			pfd->Release();
		}
		return hr;
	}


	CDialogEventHandler() : _cRef(1) { };
private:
	~CDialogEventHandler() { };
	long _cRef;
};

// Instance creation helper
HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void **ppv)
{
	*ppv = NULL;
	CDialogEventHandler *pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
	HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = pDialogEventHandler->QueryInterface(riid, ppv);
		pDialogEventHandler->Release();
	}
	return hr;
}

