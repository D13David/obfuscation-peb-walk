#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "peb_walk.h"

//------------------------------------------------------------------------------
// typedefs for dynamic function pointers
//------------------------------------------------------------------------------
typedef int     (*PMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);
typedef FILE*   (*Pfopen)(char const*, char const*);
typedef size_t  (*Pfwrite)(void const*, size_t, size_t, FILE*);
typedef int     (*Pfclose)(FILE*);
typedef VOID    (*POutputDebugStringA)(LPCSTR);
typedef HMODULE (*PLoadLibraryA)(LPCSTR);
typedef BOOL    (*PFreeLibrary)(HMODULE);

int WinMain(
        _In_ HINSTANCE hInstance, 
        _In_opt_ HINSTANCE hPrevInstance, 
        _In_ LPSTR lpCmdLine, 
        _In_ int nShowCmd
    )
{
    POutputDebugStringA print = _PROC(OutputDebugStringA);

    print(_S("INFO: Dynamically load user32.dll using obfuscated string so MessageBoxA will be resolved\n"));
    HMODULE user32Dll = _PROC(LoadLibraryA)(_S("user32.dll"));

    print(_S("INFO: Show a message box using the resolved MessageBoxA from user32.dll\n"));
    _PROC(MessageBoxA)(NULL, _S("Hello World"), _S("Info"), MB_OK);

    print(_S("INFO: Write \"Hello World\" to hello.txt using dynamically resolved C runtime functions\n"));
    FILE* fp = _PROC(fopen)(_S("hello.txt"), _S("wb"));
    if (fp)
    {
        _PROC(fwrite)(_S("Hello World"), 11, 1, fp);
        _PROC(fclose)(fp);
    }

    print(_S("INFO: Free the user32.dll module\n"));
    _PROC(FreeLibrary)(user32Dll);

    return 0;
}