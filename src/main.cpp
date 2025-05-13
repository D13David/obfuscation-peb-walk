#include <stdio.h>
#include "peb.h"

typedef int (*PMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);
typedef FILE* (*Pfopen)(char const*, char const*);

int main()
{
    LoadLibraryA(ObfuscatedString("user32.dll"));

    FUNCTION(MessageBoxA)(NULL, ObfuscatedString("Hello World"), ObfuscatedString("Info"), MB_OK);

    FILE* fp = FUNCTION(fopen)("hello.txt", "wb");

    return 0;
}