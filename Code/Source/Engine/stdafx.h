#pragma once

#include <assert.h>
#include <cstdlib>
#include "vulkan/vulkan.h"
#include <vector>
#include <array>
#include "Engine\Memory\Memory.h"

int globalIndentLevel = 0;

void PrintMessage(const std::string& message, int indentOffset = 0);
void PrintMessageGreen(const std::string& message, int indentOffset = 0);
void PrintWarningMessage(const std::string& message, int indentOffset = 0);
void PrintErrorMessage(const std::string& message, int indentOffset = 0);
void ShiftIndentLevel(int indentOffset = 0);

void ErrorCheck(VkResult vkResult);

#define VK_ALLOCATOR nullptr

#define ENGINE_NAME "Vulkan Engine"
