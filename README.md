# Unleashed Conversion
This is the source for a mod that never came to be, called Unleashed Conversion. This was supposed to be a mod that would've converted Sonic Generations into a gameplay-accurate Sonic Unleashed, before Unleashed Recomp started development. This mod contains a title screen port, a World Map port, fixes for Modern Sonic to be closer to how he was in Unleashed, cutscene ports, all stages from Unleashed, and custom objects.
The mod itself is extremely unfinished, as its development was halted in Januray 2025 after being told about Unleashed Recomp privately.

## Build Instructions
1. Install v142 build tools using Visual Studio Installer
2. Clone the repository with its dependencies (recursive)
3. Search "Edit the system environment variables", then "Environment variables", then add a new variable under "User variables" called **"GensModPath"**, set the value of this variable to your mods directory *(NOT the SUC mod directory, but where you put your mods)*
4. Open the solution and build.

## Code style rules
**Class names:** `ClassName`
**Struct Names:** `SStructName` (e.g `SManagerData`)
**Enum Names:** `EEnumName` (e.g `EOperationType`)
**Interface Names:** `IInterfaceName` (e.g `IDebugDraw`)
**Member variable Names:** `m_Variable`, add an S for static variables (e.g `ms_VariableStatic`)
**Function/Coroutine Names:** `FunctionName`
**Arguments:** any name but preferably `in_ArgumentName`

If possible, organize functions so that exports are all in one place, hooks are all in one place, etc.
If you need to access a variable from Generations or you need to add a constant for the mod, please define it as a macro in the Macros header
Try to name your classes according to what they are in Unleashed (not mandatory)
