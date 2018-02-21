#include "stdafx.h"
#include "Shader.h"

#include <Windows.h>
#include <fstream>

#include "SPIRV\GlslangToSpv.h"

#pragma comment(lib, "SPIRVd.lib")

std::wstring getEnvVar(const std::wstring& key)
{
	const DWORD bufferSize = 256;
	WCHAR buffer[bufferSize];
	DWORD bytes = GetEnvironmentVariable(key.c_str(), buffer, bufferSize);
	if (bytes > 0)
	{
		return std::wstring(buffer);
	}
	return L"";
}

Shader::Shader(const char* fileName)
{
	std::ifstream fileStream(fileName, std::ios::ate);

	if (fileStream.is_open() == true)
	{
		WIN32_FILE_ATTRIBUTE_DATA attribute = {};
		const std::string file = fileName;
		const std::wstring wfile(file.begin(), file.end());
		GetFileAttributesEx(wfile.c_str(), GetFileExInfoStandard, &attribute);

		WIN32_FILE_ATTRIBUTE_DATA attributeSpirv = {};
		const std::string fileSpirv = fileName + std::string(".spv");
		const std::wstring wfileSpirv(fileSpirv.begin(), fileSpirv.end());
		GetFileAttributesEx(wfileSpirv.c_str(), GetFileExInfoStandard, &attributeSpirv);

		if (CompareFileTime(&attribute.ftLastWriteTime, &attributeSpirv.ftLastWriteTime) >= 0)
		{
			// Recompile shader
			std::wstring vulkanSdk = getEnvVar(L"VULKAN_SDK");
			#ifdef _WIN64
			std::wstring spirvValidator = vulkanSdk + L"\\Bin\\glslangValidator.exe";
			#else
			std::wstring spirvValidator = vulkanSdk + L"\\Bin32\\glslangValidator.exe";
			#endif

			const DWORD bufferLength = 256;
			WCHAR buffer[bufferLength];
			GetCurrentDirectory(bufferLength, buffer);

			std::wstring shaderFileSrc = std::wstring(buffer) + L"\\" + wfile;
			std::wstring shaderFileDst = std::wstring(buffer) + L"\\" + wfileSpirv;

			std::wstring commandLine = L"-V Shaders/test2.vert";

			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			// set the size of the structures
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

			//glslang::TIntermediate intermediate;
			//glslang::GlslangToSpv(intermediate)

			//size_t fileSize = static_cast<size_t>(fileStream.tellg());
			//std::vector<char> fileBuffer(fileSize);

			//fileStream.seekg(0);
			//fileStream.read(fileBuffer.data(), fileSize);

			//spv_result_t spvResult = spvTextToBinary(context, fileBuffer.data(), fileSize, &binary, &diagnostic);
			//spvResult;

			//if (diagnostic != nullptr)
			//{
			//	spvDiagnosticPrint(diagnostic);
			//	spvDiagnosticDestroy(diagnostic);
			//}

			//spvContextDestroy(context);

			/*shaderc_compiler_t shaderCompiler = shaderc_compiler_initialize();

			shaderc_compilation_result_t compilationResult = shaderc_compile_into_spv(shaderCompiler, fileBuffer.data(), fileBuffer.size(), shaderc_shader_kind::shaderc_vertex_shader, fileSpirv.c_str(), "main", nullptr);
			compilationResult;

			shaderc_compiler_release(shaderCompiler);*/

			SHELLEXECUTEINFO startInfo;
			
			memset(&startInfo, 0, sizeof(startInfo));
			
			startInfo.cbSize = sizeof(startInfo);
			startInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE | SEE_MASK_WAITFORINPUTIDLE;
			startInfo.nShow = SW_SHOW;
			startInfo.lpVerb = NULL;
			startInfo.lpFile = spirvValidator.c_str();
			startInfo.lpParameters = commandLine.c_str();
			
			startInfo.nShow = SW_SHOWNOACTIVATE;
			
			auto window = ShellExecuteEx(&startInfo);
			window;
			
			WaitForSingleObject(startInfo.hProcess, INFINITE);

			DWORD exitCode;

			GetExitCodeProcess(startInfo.hProcess, &exitCode);
			
			CloseHandle(startInfo.hProcess);

			//// start the program up
			//CreateProcess(spirvValidator.c_str(),   // the path
			//	const_cast<LPWSTR>(commandLine.c_str()),        // Command line
			//	NULL,           // Process handle not inheritable
			//	NULL,           // Thread handle not inheritable
			//	TRUE,          // Set handle inheritance to FALSE
			//	0,              // No creation flags
			//	NULL,           // Use parent's environment block
			//	NULL/*(vulkanSdk + L"\\Bin\\").c_str()*/,           // Use parent's starting directory 
			//	&si,            // Pointer to STARTUPINFO structure
			//	&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
			//);
			//
			//WaitForSingleObject(pi.hProcess, INFINITE);
		}
		else
		{
			int t = 1;
			t;
		}
	}

	fileStream.close();
}
