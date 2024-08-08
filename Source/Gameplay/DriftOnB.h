#pragma once
namespace SUC::Gameplay
{
	class DriftOnB
	{
	public:
		static void RegisterHooks();
		static bool CheckForBDrift();
	};
}