#include "Configuration.h"

#include "Gameplay/QSSRestore.h"

#include "System/ArchiveTreePatcher.h"
#include "System/EnemyTrigger.h"

extern "C" __declspec(dllexport) void Init(ModInfo * modInfo)
{
	// Load configuration
	Configuration::load();

	// Set random seed
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	//---------------System---------------
	ArchiveTreePatcher::applyPatches();
	EnemyTrigger::applyPatches();

	//---------------Gameplay---------------
	QSSRestore::applyPatches();
}

extern "C" __declspec(dllexport) void PostInit()
{
	if (GetModuleHandle(TEXT("EnemyTrigger.dll")))
	{
		MessageBox(nullptr, TEXT("\"Enemy Trigger\" mod should not be enabled with this mod."), TEXT("Unleashed All in One"), MB_ICONERROR);
		exit(-1);
	}
	
	if (GetModuleHandle(TEXT("QSSRestoration.dll")))
	{
		MessageBox(nullptr, TEXT("\"QSS Restoration\" mod should not be enabled with this mod."), TEXT("Unleashed All in One"), MB_ICONERROR);
		exit(-1);
	}
}

extern "C" void __declspec(dllexport) OnFrame()
{

}