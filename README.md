# Sonic Unleashed Conversion
Mod that tries to recreate Sonic Unleashed on PC using Sonic Generations and BlueBlur

## Build Instructions
1. Install v142 build tools using Visual Studio Installer
2. Clone the repository with its dependencies (recursive)
3. Search "Edit the system environment variables", then "Environment variables", then add a new variable under "User variables" called **"GensModPath"**, set the value of this variable to your mods directory *(NOT the SUC mod directory, but where you put your mods)*
4. Open the solution and build.

## Naming Convention
**Class names:** `ClassName`
**Struct Names:** `SStructName` (e.g `SManagerData`)
**Enum Names:** `EEnumName` (e.g `EOperationType`)
**Interface Names:** `IInterfaceName` (e.g `IDebugDraw`)
**Member variable Names:** `m_Variable`, add an S for static variables (e.g `ms_VariableStatic`)
**Function/Coroutine Names:** `FunctionName`
**Arguments:** any name but preferably `in_ArgumentName`
