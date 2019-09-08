#include <stdio.h>
#include <windows.h>
#include <Shlwapi.h>

int main(int argc, char *argv[])
{
	WCHAR path[MAX_PATH + 1];
	WCHAR gtkpath[MAX_PATH + 1];
	WCHAR exepath[MAX_PATH + 1];

	HMODULE hModule = GetModuleHandleW(NULL);
	GetModuleFileNameW(hModule, path, MAX_PATH);
	PathRemoveFileSpecW(path);

	snwprintf(gtkpath, MAX_PATH + 1, L"%s%s", path, L"\\gtk\\bin");
	SetDllDirectoryW(gtkpath);
	
	snwprintf(exepath, MAX_PATH + 1, L"%s%s", path, L"\\sethdkt-gtk.exe");

	SHELLEXECUTEINFOW ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
	ShExecInfo.fMask = SEE_MASK_DEFAULT;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = exepath;        
	ShExecInfo.lpParameters = NULL;   
	ShExecInfo.lpDirectory = path;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL; 
	ShellExecuteExW(&ShExecInfo);
	return 0;
}
