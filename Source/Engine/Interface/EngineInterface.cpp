#include "stdafx.h"
#include "EngineInterface.h"
#include "Engine/Engine.h"

EngineInterface::EngineInterface()
{
	myEngine = Engine::GetInstance();
}

EngineInterface::~EngineInterface()
{
	Engine::DestroyInstance();
	myEngine = nullptr;
}

void EngineInterface::SetWidth(const unsigned int width)
{
	myEngine->SetWidth(width);
}

void EngineInterface::SetHeight(const unsigned int height)
{
	myEngine->SetHeight(height);
}

void EngineInterface::SetApplicationName(const char* name)
{
	myEngine->SetApplicationName(name);
}

void EngineInterface::Run()
{
	myEngine->Run();
}
