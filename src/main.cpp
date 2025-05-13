#include <stdio.h>
#include "peb.h"

typedef int (*PMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);
typedef FILE* (*Pfopen)(char const*, char const*);
typedef size_t (*Pfwrite)(void const*, size_t, size_t, FILE*);
typedef int (*Pfclose)(FILE*);
typedef HMODULE (*PLoadLibraryA)(LPCSTR);
typedef BOOL (*PFreeLibrary)(HMODULE);

int main()
{
    HMODULE user32Dll = _PROC(LoadLibraryA)(_S("user32.dll"));

    _PROC(MessageBoxA)(NULL, _S("Hello World"), _S("Info"), MB_OK);

    FILE* fp = _PROC(fopen)(_S("hello.txt"), _S("wb"));
    _PROC(fwrite)(_S("Hello World"), 11, 1, fp);
    _PROC(fclose)(fp);

    _PROC(FreeLibrary)(user32Dll);

    return 0;
}