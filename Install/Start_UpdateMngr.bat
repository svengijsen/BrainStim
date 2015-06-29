@ECHO OFF
CALL AutoStartConfigure_UpdateMngr.bat

IF /I '%ARCHITECTURE%'=='1' GOTO WIN32
IF /I '%ARCHITECTURE%'=='2' GOTO X64
GOTO END

:WIN32
	IF /I '%DEBUGRELEASE%'=='d' CALL UpdateMngr_Win32_Debug.exe
	IF /I '%DEBUGRELEASE%'=='r' CALL UpdateMngr_Win32_Release.exe
	GOTO END

:X64
	IF /I '%DEBUGRELEASE%'=='d' CALL UpdateMngr_x64_Debug.exe
	IF /I '%DEBUGRELEASE%'=='r' CALL UpdateMngr_x64_Release.exe

:END
PAUSE
