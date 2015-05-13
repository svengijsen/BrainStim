@ECHO OFF
CALL AutoStartConfigure.bat

IF /I '%ARCHITECTURE%'=='1' GOTO WIN32
IF /I '%ARCHITECTURE%'=='2' GOTO X64
GOTO END

:WIN32
	IF /I '%DEBUGRELEASE%'=='d' CALL BrainStim_Win32_Debug.exe
	IF /I '%DEBUGRELEASE%'=='r' CALL BrainStim_Win32_Release.exe

:X64
	IF /I '%DEBUGRELEASE%'=='d' CALL BrainStim_x64_Debug.exe
	IF /I '%DEBUGRELEASE%'=='r' CALL BrainStim_x64_Release.exe


:END
PAUSE
