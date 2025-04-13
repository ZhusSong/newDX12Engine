#include "EngineFactory.h"
#if defined(_WIN32)
#include "Windows/WindowsEngine.h"
#endif

FEngineFactory::FEngineFactory()
{

}

FEngine* FEngineFactory::CreateEngine()
{
#if defined(_WIN32)
	return new FWindowsEngine();
#elif defined(__linux__)

#endif // DEBUG
}
