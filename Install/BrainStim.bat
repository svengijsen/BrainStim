@echo off

:START
SET DEBUGRELEASE=
SET ARCHITECTURE=
SET QT_VERSION=
SET QSCINTILLA_PATH=C:\Qt\QScintilla-gpl-2.7.2\lib

SET /p QT_VERSION= Define the used Qt version, ie '5.1.0'[=default]? 
SET /p DEBUGRELEASE= Start the (d)ebug[=default] or (r)elease version? 
SET /p ARCHITECTURE= Start the win32(1)[=default] or the x64(2) version? 

IF /I '%QT_VERSION%'=='' SET QT_VERSION=5.1.0
IF /I '%DEBUGRELEASE%'=='' SET DEBUGRELEASE=d
IF /I '%ARCHITECTURE%'=='' SET ARCHITECTURE=1
ECHO Your input was: Architecture(%ARCHITECTURE%) on %QT_VERSION%
ECHO Additional to use library paths:
ECHO    %QSCINTILLA_PATH%
IF /I '%ARCHITECTURE%'=='1' GOTO WIN32
IF /I '%ARCHITECTURE%'=='2' GOTO X64
GOTO START

:WIN32
	ECHO WIN32 was choosen
	SET QT_QPA_PLATFORM_PLUGIN_PATH=C:\Qt\%QT_VERSION%\win32\plugins\platforms
	SET Path=C:\Qt\%QT_VERSION%\win32\bin;%Path%
	
	IF /I '%DEBUGRELEASE%'=='d' SET Path=%QSCINTILLA_PATH%\Win32\Debug;%Path%
	IF /I '%DEBUGRELEASE%'=='d' CALL BrainStim_win32_Debug.exe
	
	IF /I '%DEBUGRELEASE%'=='r' SET Path=%QSCINTILLA_PATH%\Win32\Release;%Path%
	IF /I '%DEBUGRELEASE%'=='r' BrainStim_win32_Release.exe

	GOTO START
	
:X64
	ECHO X64 was choosen
	SET QT_QPA_PLATFORM_PLUGIN_PATH=C:\Qt\%QT_VERSION%\x64\plugins\platforms
	SET Path=C:\Qt\%QT_VERSION%\x64\bin;%Path%

	IF /I '%DEBUGRELEASE%'=='d' SET Path=%QSCINTILLA_PATH%\x64\Debug;%Path%
	IF /I '%DEBUGRELEASE%'=='d' BrainStim_x64_Debug.exe

	IF /I '%DEBUGRELEASE%'=='r' SET Path=%QSCINTILLA_PATH%\x64\Release;%Path%
	IF /I '%DEBUGRELEASE%'=='r' BrainStim_x64_Release.exe

	GOTO START

