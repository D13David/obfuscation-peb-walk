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
    HMODULE user32Dll = FUNCTION(LoadLibraryA)(ObfuscatedString("user32.dll"));

    FUNCTION(MessageBoxA)(NULL, ObfuscatedString("Hello World"), ObfuscatedString("Info"), MB_OK);

    FILE* fp = FUNCTION(fopen)(ObfuscatedString("hello.txt"), ObfuscatedString("wb"));
    FUNCTION(fwrite)(ObfuscatedString("Hello World"), 11, 1, fp);
    FUNCTION(fclose)(fp);

    FUNCTION(FreeLibrary)(user32Dll);

    return 0;
}