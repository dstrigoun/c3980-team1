#include "FileChooser.h"
#include "CDialogEventHandler.h"

#define INDEX_TEXTDOC 3

const COMDLG_FILTERSPEC c_rgSaveTypes[] =
{
	{L"Text Document (*.txt)",       L"*.txt"},
	//{L"All Documents (*.*)",         L"*.*"}
};


/*-------------------------------------------------------------------------------------
--	FUNCTION:	    openFile
--
--	DATE:			November 21, 2018
--
--	REVISIONS:		November 21, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Kieran Lee
--
--	INTERFACE:		HRESULT openFile()
--
--	RETURNS:		HRESULT object
--                  This will contain any error codes if the function fails
--
--	NOTES:          TODO
--
--------------------------------------------------------------------------------------*/
HRESULT openFile(HWND *hWnd)
{
	USES_CONVERSION;

	// CoCreate the File Open Dialog object.
	IFileDialog *pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		// Create an event handling object, and hook it up to the dialog.
		IFileDialogEvents *pfde = NULL;
		hr = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
		if (SUCCEEDED(hr))
		{
			// Hook up the event handler.
			DWORD dwCookie;
			hr = pfd->Advise(pfde, &dwCookie);
			if (SUCCEEDED(hr))
			{
				// Set the options on the dialog.
				DWORD dwFlags;

				// Before setting, always get the options first in order 
				// not to override existing options.
				hr = pfd->GetOptions(&dwFlags);
				if (SUCCEEDED(hr))
				{
					// In this case, get shell items only for file system items.
					hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
					if (SUCCEEDED(hr))
					{
						// Set the file types to display only. 
						// Notice that this is a 1-based array.
						hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
						if (SUCCEEDED(hr))
						{
							// Set the selected file type index to Word Docs for this example.
							hr = pfd->SetFileTypeIndex(INDEX_TEXTDOC);
							if (SUCCEEDED(hr))
							{
								// Set the default extension to be ".doc" file.
								hr = pfd->SetDefaultExtension(L"txt");
								if (SUCCEEDED(hr))
								{
									// Show the dialog
									hr = pfd->Show(NULL);
									if (SUCCEEDED(hr))
									{
										// Obtain the result once the user clicks 
										// the 'Open' button.
										// The result is an IShellItem object.
										IShellItem *psiResult;
										hr = pfd->GetResult(&psiResult);
										if (SUCCEEDED(hr))
										{
											// We are just going to print out the 
											// name of the file for sample sake.
											PWSTR pszFilePath = NULL;
											hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH,
												&pszFilePath);

											if (SUCCEEDED(hr))
											{
												TaskDialog(NULL,
													NULL,
													L"CommonFileDialogApp",
													pszFilePath,
													NULL,
													TDCBF_OK_BUTTON,
													TD_INFORMATION_ICON,
													NULL);

												std::ifstream hFile(W2A(pszFilePath));

												std::vector<std::string> lines;
												std::string line;
												while (std::getline(hFile, line))
													lines.push_back(line);

												for (std::string s : lines) {
													LPCSTR ss = s.c_str();
													OutputDebugString(ss);
													MessageBox(*hWnd, ss, "file path?", MB_OK);

												}
												CoTaskMemFree(pszFilePath);
											}
											psiResult->Release();
										}
									}
								}
							}
						}
					}
				}
				// Unhook the event handler.
				pfd->Unadvise(dwCookie);
			}
			pfde->Release();
		}
		pfd->Release();
	}
	return hr;
}

