#pragma once

class UpdateDirector
{
	static void AddUpdateCommand(void* context, float elapsedTime, const char* name);
	static void AddRenderCommand(void* context, float elapsedTime, const char* name);
	static void AddUpdateAndRenderCommand(void* context, float elapsedTime, const char* name);
	 
	static void CallUnknownFunction(void* context, float elapsedTime);
	 
	static void* FinishRenderer(void* context);
	static void* FinishUpdater(void* context);
};
