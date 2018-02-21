#include "stdafx.h"
#include <sstream>
#include <iostream>
#include <Windows.h>

#ifndef PUBLISH

void AddOutputIndent()
{
	for (int i = 0; i < globalIndentLevel; ++i)
	{
		std::cout << "  ";
	}
}

void PrintMessage(const std::string& message, int indentOffset)
{
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

	WORD color = 0;
	color |= 1 << 0; // Blue
	color |= 1 << 1; // Green
	color |= 1 << 2; // Red
	color |= 1 << 3;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hstdout, &csbi);
	SetConsoleTextAttribute(hstdout, color);

	globalIndentLevel += indentOffset;
	AddOutputIndent();
	std::cout << message << std::endl;

	SetConsoleTextAttribute(hstdout, csbi.wAttributes);
}

void PrintMessageGreen(const std::string& message, int indentOffset)
{
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

	WORD color = 0;
	color |= 0 << 0; // Blue
	color |= 1 << 1; // Green
	color |= 0 << 2; // Red
	color |= 1 << 3;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hstdout, &csbi);
	SetConsoleTextAttribute(hstdout, color);

	globalIndentLevel += indentOffset;
	AddOutputIndent();
	std::cout << message << std::endl;

	SetConsoleTextAttribute(hstdout, csbi.wAttributes);
}

void PrintWarningMessage(const std::string& message, int indentOffset)
{
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

	WORD color = 0;
	color |= 0 << 0; // Blue
	color |= 1 << 1; // Green
	color |= 1 << 2; // Red
	color |= 1 << 3;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hstdout, &csbi);
	SetConsoleTextAttribute(hstdout, color);

	globalIndentLevel += indentOffset;
	AddOutputIndent();
	std::cout << message << std::endl;

	SetConsoleTextAttribute(hstdout, csbi.wAttributes);
}

void PrintErrorMessage(const std::string& message, int indentOffset)
{
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

	WORD color = 0;
	color |= 0 << 0; // Blue
	color |= 0 << 1; // Green
	color |= 1 << 2; // Red
	color |= 1 << 3;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hstdout, &csbi);
	SetConsoleTextAttribute(hstdout, color);

	globalIndentLevel += indentOffset;
	AddOutputIndent();
	std::cout << message << std::endl;

	SetConsoleTextAttribute(hstdout, csbi.wAttributes);
}

void ShiftIndentLevel(int indentOffset)
{
	globalIndentLevel += indentOffset;
}

#else

void PrintMessage(const std::string& , int ) { }
void PrintMessageGreen(const std::string& , int ) { }
void PrintWarningMessage(const std::string& , int ) { }
void PrintErrorMessage(const std::string& , int ) { }
void ShiftIndentLevel(int ) { }

#endif

void ErrorCheck(VkResult vkResult)
{
	if (vkResult < 0)
	{
		std::ostringstream stream;

		switch (vkResult)
		{
		case VK_SUCCESS:
			stream << "VK_SUCCESS";
			break;
		case VK_NOT_READY:
			stream << "VK_NOT_READY";
			break;
		case VK_TIMEOUT:
			stream << "VK_TIMEOUT";
			break;
		case VK_EVENT_SET:
			stream << "VK_EVENT_SET";
			break;
		case VK_EVENT_RESET:
			stream << "VK_EVENT_RESET";
			break;
		case VK_INCOMPLETE:
			stream << "VK_INCOMPLETE";
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			stream << "VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			stream << "VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			stream << "VK_ERROR_INITIALIZATION_FAILED";
			break;
		case VK_ERROR_DEVICE_LOST:
			stream << "VK_ERROR_DEVICE_LOST";
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			stream << "VK_ERROR_MEMORY_MAP_FAILED";
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			stream << "VK_ERROR_LAYER_NOT_PRESENT";
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			stream << "VK_ERROR_EXTENSION_NOT_PRESENT";
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			stream << "VK_ERROR_FEATURE_NOT_PRESENT";
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			stream << "VK_ERROR_INCOMPATIBLE_DRIVER";
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			stream << "VK_ERROR_TOO_MANY_OBJECTS";
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			stream << "VK_ERROR_FORMAT_NOT_SUPPORTED";
			break;
		case VK_ERROR_FRAGMENTED_POOL:
			stream << "VK_ERROR_FRAGMENTED_POOL";
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			stream << "VK_ERROR_SURFACE_LOST_KHR";
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			stream << "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
			break;
		case VK_SUBOPTIMAL_KHR:
			stream << "VK_SUBOPTIMAL_KHR";
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			stream << "VK_ERROR_OUT_OF_DATE_KHR";
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			stream << "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			stream << "VK_ERROR_VALIDATION_FAILED_EXT";
			break;
		case VK_ERROR_INVALID_SHADER_NV:
			stream << "VK_ERROR_INVALID_SHADER_NV";
			break;
		case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
			stream << "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
			break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
			stream << "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
			break;
		case VK_ERROR_NOT_PERMITTED_EXT:
			stream << "VK_ERROR_NOT_PERMITTED_EXT";
			break;
		}

		PrintErrorMessage("Vulkan Error: " + stream.str());
#ifdef _WIN32
		std::string txt = stream.str();
		std::wstring wtxt(txt.begin(), txt.end());
		MessageBox(NULL, wtxt.c_str(), L"Vulkan Error!", 0);
#endif
		assert(0 && "Vulkan Error!");
		std::exit(-1);
	}
}
