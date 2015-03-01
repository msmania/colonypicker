//
// main.cpp
//

#include <Windows.h>
#include <stdio.h>

#include <string>
#include <fstream>

#define LOGERROR(msg, ...) wprintf(msg, __VA_ARGS__)
#define LOGINFO LOGERROR

bool ColonyPicking(const char *ImageFile, const char *OutputImage, std::wofstream &Statistics);

void ShowUsage() {
    wprintf(L"\nUSAGE: COLONYPICKER <File or Folder path>\n");
    wprintf(L"\nEXAMPLE:");
    wprintf(L"\n  COLONYPICKER *.jpg");
    wprintf(L"\n  COLONYPICKER C:\\MSWORK");
    wprintf(L"\n  COLONYPICKER C:\\MSWORK\\*.jpg");
    wprintf(L"\n  COLONYPICKER C:\\MSWORK\\Colony.jpg\n");
}

void Convert(const std::wstring &WideString, std::string &AsciiString) {
    // NOTE: cannot convert non-ascii chars
    AsciiString.assign(WideString.begin(), WideString.end());
}

void Run(LPCWSTR Path) {
    BOOL Ret = FALSE;
    DWORD Status = 0;
    HANDLE SearchHandle = INVALID_HANDLE_VALUE;
    std::wofstream Stats;

    WIN32_FIND_DATA FindData;
    ZeroMemory(&FindData, sizeof(FindData));

    std::wstring SearchPath(Path);
    std::wstring RootPath(Path);
    std::wstring OutputDir;
    if ( SearchPath[SearchPath.length()-1] == L'\\' ) {
        SearchPath += L"*";
    }
    else {
        DWORD Attributes = GetFileAttributes(Path);
        if ( Attributes!=INVALID_FILE_ATTRIBUTES  && (Attributes & FILE_ATTRIBUTE_DIRECTORY) ) {
            // single directory
            SearchPath += L"\\*";
            RootPath += L'\\';
        }
        else {
            // single file or pattern
            int Pos = RootPath.find_last_of(L'\\');
            RootPath = Pos==std::wstring::npos ? L"" : RootPath.substr(0, Pos+1);
        }
    }

    OutputDir = RootPath + L"Output";
    CreateDirectory(OutputDir.c_str(), NULL);

    SearchHandle = FindFirstFile(SearchPath.c_str(), &FindData);
    if ( SearchHandle==INVALID_HANDLE_VALUE ) {
        LOGERROR(L"FindFirstFile failed - 0x%08x\n", GetLastError());
        goto cleanup;
    }

    Stats.open(OutputDir + L"\\Stats.csv");
    Stats << "FileName, Colony#, X, Y, Radius" << std::endl;

    do {
        CONST DWORD InvalidAttributes = FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_OFFLINE |  FILE_ATTRIBUTE_REPARSE_POINT;

        if ( !(FindData.dwFileAttributes&InvalidAttributes) ) {
            std::string InputFileA;
            std::string OutputFileA;
            
            Convert(OutputDir + L"\\" + FindData.cFileName, OutputFileA);
            Convert(RootPath + FindData.cFileName, InputFileA);
            
            //LOGINFO(L"  %s -> %s\n", InputFile.c_str(), OutputFile.c_str());

            if ( ColonyPicking(InputFileA.c_str(), OutputFileA.c_str(), Stats) ) {
                //LOGINFO(L"PASSED: %s -> %s\n", InputFile.c_str(), OutputFile.c_str());
            }
            else {
                LOGINFO(L"FAILED: %S\n", InputFileA.c_str());
            }
        }

        Ret = FindNextFile(SearchHandle, &FindData);
        Status = GetLastError();
        if ( !Ret ) {
            if ( Status!=ERROR_NO_MORE_FILES )
                LOGERROR(L"FindNextFile failed - 0x%08x\n", GetLastError());
            goto cleanup;
        }
    } while (1);
    
cleanup:
    if ( Stats.is_open() )
        Stats.close();

    if ( SearchHandle!=INVALID_HANDLE_VALUE )
        FindClose(SearchHandle);
}

int __cdecl wmain(int argc, wchar_t *argv[]) {
    if ( argc<2 ) {
        ShowUsage();
        return 1;
    }
    
    Run(argv[1]);

    return 0;
}
