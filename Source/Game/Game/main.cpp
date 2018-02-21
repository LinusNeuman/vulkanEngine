#include "EngineInterface.h"
#include <cstdlib>

int main()
{
	EngineInterface* engine = new EngineInterface();
	engine->SetApplicationName("Vulkan Engine");
	engine->Run();

	delete engine;
	engine = nullptr;

	return 0;
}
