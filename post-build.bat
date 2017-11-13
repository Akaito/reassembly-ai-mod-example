:: If post-build.bat is failing from Visual Studio and you can't tell
::   why, un-comment-out the "pause" at the end of this script and run
::   it manually, so you can see what it's trying to do and failing on.

:: First arg: mod name (optionally quoted)

:: Remember to update your mod's directory!!
:: Note!  "robocopy" gets *really* confused by backslashes sometimes.
::   So be sure you don't leave a trailing one in your path here.
::   Also be sure the path here is double-quoted.
set MY_AI_OUTPUT="%USERPROFILE%\Saved Games\Reassembly\mods\%~1\ai"
::echo %MY_AI_OUTPUT%
::pause

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
exit /B 1

:robocopy_success
::pause
exit /B 0