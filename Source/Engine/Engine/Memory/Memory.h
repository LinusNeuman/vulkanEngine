#pragma once
#include <new>

void InitializeMemory(const unsigned long long MegaByteRAM);
void DestroyMemory();

void Allocate(const char* file, int line, const char* function);
//#define new (Allocate(__FILE__, __LINE__, __FUNCTION__), false) ? 0 : new
