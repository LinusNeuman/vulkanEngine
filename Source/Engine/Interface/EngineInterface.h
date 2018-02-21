#pragma once

class Engine;

class EngineInterface
{
public:
	EngineInterface();
	~EngineInterface();
	EngineInterface(const EngineInterface&) = delete;

	void SetWidth(const unsigned int width);
	void SetHeight(const unsigned int height);
	void SetApplicationName(const char* name);
	void Run();

	EngineInterface& operator=(const EngineInterface&) = delete;

private:
	Engine* myEngine;
};
