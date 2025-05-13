#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include "utils.h"

#define FUNCTION(Name) \
    GetProcAddressFromPEB<P##Name>(#Name)

extern "C" {
    extern PPEB GetPEB();
}

template<typename T, size_t N>
__forceinline T GetProcAddressFromPEB(const char(&functionName)[N])
{
#define RESOLVE_MODULE_RELATIVE(Type, Offset) \
    ((Type)((LPBYTE)(moduleEntry->DllBase) + (Offset)))

    PPEB peb = GetPEB();
    PLIST_ENTRY firstEntry = peb->Ldr->InMemoryOrderModuleList.Flink;

    for (PLIST_ENTRY entry = firstEntry; entry != firstEntry->Blink; entry = entry->Flink)
    {
        LDR_DATA_TABLE_ENTRY* moduleEntry =
            CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        PIMAGE_DOS_HEADER dosHeader = RESOLVE_MODULE_RELATIVE(PIMAGE_DOS_HEADER, 0);
        PIMAGE_NT_HEADERS64 imageFileHeader = RESOLVE_MODULE_RELATIVE(PIMAGE_NT_HEADERS64, dosHeader->e_lfanew);

        const IMAGE_DATA_DIRECTORY& dataDictionary =
            imageFileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

        if (dataDictionary.VirtualAddress == 0)
            continue;

        PIMAGE_EXPORT_DIRECTORY exportDirectory = RESOLVE_MODULE_RELATIVE(PIMAGE_EXPORT_DIRECTORY, dataDictionary.VirtualAddress);
        PDWORD nameTable = RESOLVE_MODULE_RELATIVE(PDWORD, exportDirectory->AddressOfNames);
        PWORD ordinalTable = RESOLVE_MODULE_RELATIVE(PWORD, exportDirectory->AddressOfNameOrdinals);
        PDWORD functionTable = RESOLVE_MODULE_RELATIVE(PDWORD, exportDirectory->AddressOfFunctions);

        constexpr unsigned int compileTimeSeed = seed(__FILE__, __TIME__);;

        for (DWORD nameIndex = 0; nameIndex < exportDirectory->NumberOfNames; ++nameIndex)
        {
            LPCCH name = RESOLVE_MODULE_RELATIVE(LPCCH, nameTable[nameIndex]);

            if (fnv1a32(name, strlen(name), compileTimeSeed) == fnv1a32(functionName, compileTimeSeed))
            {
                return RESOLVE_MODULE_RELATIVE(T, functionTable[ordinalTable[nameIndex]]);
            }
        }
    }

#undef RESOLVE_MODULE_RELATIVE

    return nullptr;
}