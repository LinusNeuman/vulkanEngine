#include "stdafx.h"
#include "Memory.h"
#include <new>
#include <map>
#include <iostream>
#include <string>
#include <sstream>

#ifdef new
#undef new
#endif

std::map<void*, std::size_t>* allocations = nullptr;

bool allocating = false;

void* operator new(std::size_t bytes)
{
	void* pointer = malloc(bytes);
	if (allocations != nullptr && allocating == false)
	{
		allocating = true;
		(*allocations)[pointer] = bytes;
		allocating = false;
	}

	return pointer;
}

void operator delete(void* pointer) noexcept
{
	free(pointer);

	if (allocations != nullptr && allocating == false)
	{
		std::map<void*, std::size_t>::iterator iterator = (*allocations).find(pointer);
		if (iterator != allocations->end())
		{
			allocations->erase(pointer);
		}
	}
}

void* operator new[](std::size_t bytes)
{
	void* pointer = malloc(bytes);
	if (allocations != nullptr && allocating == false)
	{
		allocating = true;
		(*allocations)[pointer] = bytes;
		allocating = false;
	}

	return pointer;
}

void operator delete[](void* pointer) noexcept
{
	free(pointer);

	if (allocations != nullptr)
	{
		std::map<void*, std::size_t>::iterator iterator = (*allocations).find(pointer);
		if (iterator != allocations->end())
		{
			allocations->erase(pointer);
		}
	}
}

void InitializeMemory(const unsigned long long /*MegaByteRAM*/)
{
	//unsigned long long bytes = MegaByteRAM * 1024 * 1024;

	allocations = new std::map<void*, std::size_t>();

	/*memory = reinterpret_cast<char*>(malloc(bytes));
	memset(memory, 0, bytes);*/

	//memoryOffset = 0;

	//allocations.resize(1024);
}

void DestroyMemory()
{
	int unallocated = 0;
	std::size_t bytes = 0;

	PrintMessage("Memory Manager");
	ShiftIndentLevel(1);

	PrintMessage("Allocated Blocks");
	ShiftIndentLevel(1);
	for (std::map<void*, std::size_t>::iterator iterator = allocations->begin(); iterator != allocations->end(); ++iterator)
	{
		++unallocated;
		bytes += iterator->second;

		std::ostringstream stringStream;

		stringStream << iterator->first << " allocated " << iterator->second;

		PrintWarningMessage(stringStream.str().c_str());
	}
	PrintMessage("", -1);

	PrintMessage("Summary");
	ShiftIndentLevel(1);

	PrintMessage("Count: " + std::to_string(unallocated));
	if (bytes < 1024)
	{
		PrintMessage("Allocated: " + std::to_string(bytes) + " Bytes");
	}
	else if (bytes < 1024 * 1024)
	{
		PrintWarningMessage("Allocated: " + std::to_string(bytes / 1024) + " Kilobytes");
	}
	else
	{
		if ((bytes / (1024 * 1024)) > 50)
		{
			PrintErrorMessage("Allocated: " + std::to_string(bytes / (1024 * 1024)) + " Megabytes");
		}
		else
		{
			PrintWarningMessage("Allocated: " + std::to_string(bytes / (1024 * 1024)) + " Megabytes");
		}
	}
	PrintMessage("", -1);
	ShiftIndentLevel(-1);
	
	allocating = true;
	delete allocations;

	if (unallocated > 0)
	{
#if _DEBUG
		system("pause");
#endif
	}
}

void Allocate(const char * file, int line, const char * function)
{
	file;
	line;
	function;

	int test = 1;
	test;
}
