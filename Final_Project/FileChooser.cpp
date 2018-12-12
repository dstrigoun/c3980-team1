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
--	INTERFACE:		HRESULT openFile()
--
--	RETURNS:		HRESULT object
--                  This will contain any error codes if the function fails
--
--	NOTES:          TODO
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

char* getPayload() {

	VariableManager &vm = VariableManager::getInstance();
	if (vm.get_currUploadFile() == nullptr) {
		return nullptr;
	}
	ifstream* currFile = vm.get_currUploadFile();

	std::ofstream file;
	file.open("log.txt", std::fstream::app);
	file.close();
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