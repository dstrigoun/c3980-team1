/*-------------------------------------------------------------------------------------
--	SOURCE FILE:    FileChooser.cpp - An application that create/show window and handles basic
--										menu functions and window events, as well as getting data
--									  from the selected file
--
--	PROGRAM:		GTID
--
--	FUNCTIONS:
--					std::ifstream openFile(HWND *hWnd)
--					char* getPayload()
--
--
--	DATE:			November 21, 2018
--
--	REVISIONS:		November 22, 2018 - added getPayload()
--					November 23, 2018 - finished getPayload()
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Kieran Lee
--
--	NOTES:
--	in charge of choosing the file and getting data from it
--
--------------------------------------------------------------------------------------*/

#include "FileChooser.h"
#include "FrameHandler.h"

#define INDEX_TEXTDOC 3

using namespace std;
//File extensions users are allowed to choose
const COMDLG_FILTERSPEC c_rgSaveTypes[] =
{
	{L"Text Document (*.txt)",       L"*.txt"},
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
--	INTERFACE:		std::ifstream openFile(HWND *hWnd)
--
--	RETURNS:		std::ifstream file
--                  File stream of the file that the user has chosen to upload;
--
--	NOTES:          Users can only choose .txt files
--
--------------------------------------------------------------------------------------*/
std::ifstream openFile(HWND *hWnd)
{
	USES_CONVERSION;

	std::vector<std::string> lines;
	std::ifstream hFile;


	// CoCreate the File Open Dialog object.
	IFileDialog *pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pfd));
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
										hFile.open(W2A(pszFilePath));

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
	}
	return hFile;
}


/*-------------------------------------------------------------------------------------
--	FUNCTION:	    getPayload
--
--	DATE:			November 22, 2018
--
--	REVISIONS:		November 23, 2018
--
--	DESIGNER:		Dasha Strigoun, Kieran Lee, Alexander Song, Jason Kim
--
--	PROGRAMMER:		Kieran Lee
--
--	INTERFACE:		char* getPayload()
--
--	RETURNS:		char*
--                  character array of length 1021 that contains data from the file to send
--
--	NOTES:          If there are not 1021 characters left in the file stream, then the remaining will be padded with DC4
--
--------------------------------------------------------------------------------------*/
char* getPayload() {

	VariableManager &vm = VariableManager::getInstance();
	if (vm.get_currUploadFile() == nullptr) {
		return nullptr;
	}
	ifstream* currFile = vm.get_currUploadFile();

	char* payload = new char[NUM_PAYLOAD_BYTES + 1];
	for (int i = 0; i < NUM_PAYLOAD_BYTES; i++) {
		payload[i] = DC4;
	}
	for (int i = 0; i < NUM_PAYLOAD_BYTES; i++) {
		payload[i] = currFile->get();
		
		if (currFile->eof()) {
			debugMessage("HIT EOF");
			vm.set_hitEOF(true);
			break;
		}
		if (currFile->fail()) {
			OutputDebugString("failbit true");
			break;
		}
		if (i == NUM_PAYLOAD_BYTES - 1) {
			OutputDebugString("i made it to end of payload");
			payload[i + 1] = '\0';
		}
	}
	return payload;
}