#pragma once
class SetEditorTest
{
public:
	static void applyPatches();
	static void remakeAllObjects();
	static void draw();
	static void activateLayer(const char* layerName);
	static void deactivateLayer(const char* layerName);
};