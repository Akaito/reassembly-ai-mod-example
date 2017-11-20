:: If post-build.bat is failing from Visual Studio and you can't tell
::   why, un-comment-out the "pause" at the end of this script and run
::   it manually, so you can see what it's trying to do and failing on.
:: TL;DR: This script "just" copies some files from the local mod-assets and build output to
::   your mod's files.  The crazy syntax is to make it decently well-behaved in Windows.
@ECHO OFF

:: First arg: mod name (optionally quoted)

:: Remember to update your mod's directory!!
:: Note!  "robocopy" gets *really* confused by backslashes sometimes.
::   So be sure you don't leave a trailing one in your path here.
::   Also be sure the path here is double-quoted.
set MY_MOD_OUTPUT="%USERPROFILE%\Saved Games\Reassembly\mods\%~1"
:: %foo:old=new% syntax: replace "old" with "new" in variable.  Here, replace quotes with nothing.
set MY_AI_OUTPUT="%MY_MOD_OUTPUT:"=%\ai"
::echo %MY_MOD_OUTPUT%
::echo %MY_AI_OUTPUT%
::pause

:: Don't do any copying work if it doesn't look like we were run by Visual Studio.
if "%1"=="" (
    echo "Warning: Only makes sense to run this from Visual Studio via Build."
    exit /B 1
)

:: Copy AI DLL and PDB (for debugging) to your mod's AI dir.
:: That "%~dp0" craziness just means "this .bat file's location".
::   It *is* supposed to be against "Release" with no backslash.
::   Otherwise robocopy gets horribly confused by the "\\" since
::   "%~dp0" has a trailing backslash.
robocopy "%~dp0Release" %MY_AI_OUTPUT% *.dll *.pdb /FP
:: errorlevel 1: did something (copied some stuff)
if errorlevel 1 goto robocopy_success
:: errorlevel 0: did nothing (all skipped)
if errorlevel 0 goto robocopy_success
:: Otherwise, robocopy returned something unexpected.  Quit.
exit /B 1

:robocopy_success

:: Provide default template files if not already present.
::   Or, if you choose to modify mod-assets files, put your own in place.
::   Completing a build should completely prepare a mod for use.
if not exist "%MY_MOD_OUTPUT:"=%\factions.lua" (
    :: "%~dp0" has a trailing slash, which confuses robocopy.  "%~dp0." removes it.
    ::   Above comment no longer relevant.  Kept in case I need to know that again later.
    robocopy "%~dp0mod-assets" %MY_MOD_OUTPUT% factions.lua /FP
)
if not exist "%MY_MOD_OUTPUT:"=%\regions.lua" (
    robocopy "%~dp0mod-assets" %MY_MOD_OUTPUT% regions.lua /FP
)
:: Since ships could be renamed to anything, only provide default ships if folder isn't present.
:: Even if folder is empty, no default ships will be copied to output location.
if not exist "%MY_MOD_OUTPUT:"=%\ships" (
    robocopy "%~dp0mod-assets\ships" "%MY_MOD_OUTPUT:"=%\ships" *.lua /FP
)
::exit /B 1

::pause
exit /B 0