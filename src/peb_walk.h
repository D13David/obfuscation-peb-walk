#pragma once

/*
 * Dynamically Resolve a Function Address by Walking the Export Table of Loaded Modules
 *
 * This technique is commonly used in scenarios such as:
 *   - Malware and shellcode to avoid detection by static analysis
 *   - Custom dynamic loaders or plugin systems
 *   - Avoiding dependencies on the Import Address Table (IAT)
 *
 * The example demonstrates how to obtain the address of the 'LoadLibraryA' function from the
 * export table of the loaded `kernel32.dll` module without relying on static imports.
 *
 * -----------------------------------------------------------------------------
 * Assumptions:
 *   - The target module (`kernel32.dll` in this case) is already loaded into the process memory.
 *   - A function pointer typedef exists for each target procedure, using the naming convention:
 *       P<FunctionName>, e.g., `PLoadLibraryA` for `LoadLibraryA`
 *   - A macro or function `_PROC(FunctionName)` is defined to resolve and return the function
 *     address by:
 *       1. Enumerating the loaded modules in the process (e.g., via PEB or ToolHelp).
 *       2. Identifying the target module (`kernel32.dll`).
 *       3. Parsing the module's export table to locate the requested function.
 *
 * -----------------------------------------------------------------------------
 * Example Usage:
 *   typedef HMODULE (*PLoadLibraryA)(LPCSTR lpLibFileName);
 *   PLoadLibraryA LoadLibraryA = _PROC(LoadLibraryA);  // Resolve address dynamically
 *   HMODULE hUser32 = LoadLibraryA("user32.dll");      // Load another DLL at runtime
 *
 */
#define _PROC(Name) GetProcAddress<P##Name>(#Name)

template<typename T, size_t N>
T GetProcAddress(const char(&functionName)[N]);

#include "peb_walk.inl"