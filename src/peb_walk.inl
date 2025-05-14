#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>

#include "utils.h"

extern "C" {
    extern PPEB GetPEB();
}

// Make pointer of specific type relative to a image base
#define MAKE_PTR_FROM_MODULE( Module, Type , Offset ) ((Type)    \
    ((LPBYTE)((Module)->DllBase) +                                \
     (Offset)                                                   \
    ))

template<typename T, size_t N>
__forceinline T GetProcAddress(const char(&functionName)[N])
{
    const PLIST_ENTRY moduleList = &GetPEB()->Ldr->InMemoryOrderModuleList;

    for (PLIST_ENTRY entry = moduleList->Flink; entry != moduleList->Blink; entry = entry->Flink)
    {
        // Get module base address from 'LDR_DATA_TABLE_ENTRY' and lookup the 
        // module export table from IMAGE_OPTIONAL_HEADER
        PLDR_DATA_TABLE_ENTRY Module      = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
        PIMAGE_DOS_HEADER dosHeader       = MAKE_PTR_FROM_MODULE(Module, PIMAGE_DOS_HEADER, 0);
        PIMAGE_NT_HEADERS imageFileHeader = MAKE_PTR_FROM_MODULE(Module, PIMAGE_NT_HEADERS, dosHeader->e_lfanew);

        const IMAGE_DATA_DIRECTORY& dataDictionary =
            imageFileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

        // Skip modules that don't export (e.g. current process)
        if (dataDictionary.VirtualAddress == 0) {
            continue;
        }

        PIMAGE_EXPORT_DIRECTORY exportDirectory = MAKE_PTR_FROM_MODULE(Module, PIMAGE_EXPORT_DIRECTORY, dataDictionary.VirtualAddress);
        PDWORD nameTable     = MAKE_PTR_FROM_MODULE(Module, PDWORD, exportDirectory->AddressOfNames);
        PWORD ordinalTable   = MAKE_PTR_FROM_MODULE(Module, PWORD, exportDirectory->AddressOfNameOrdinals);
        PDWORD functionTable = MAKE_PTR_FROM_MODULE(Module, PDWORD, exportDirectory->AddressOfFunctions);

        for (DWORD nameIndex = 0; nameIndex < exportDirectory->NumberOfNames; ++nameIndex)
        {
            LPCCH name = MAKE_PTR_FROM_MODULE(Module, LPCCH, nameTable[nameIndex]);

            // Use hash comparison instead of checking for strings to avoid string 
            // literals appearing in build. The 'functionName' hash is computed at compile time
            // with a different seed for every call avoiding easy resolve of API names 
            // by dictionary attacks
            constexpr unsigned int compileTimeSeed  = seed(__FILE__, __TIME__);
            const unsigned int functionNameHash = fnv1a32(functionName, compileTimeSeed);
            if (fnv1a32(name, strlen(name), compileTimeSeed) == functionNameHash)
            {
                return MAKE_PTR_FROM_MODULE(Module, T, functionTable[ordinalTable[nameIndex]]);
            }
        }
    }

    return nullptr;
}