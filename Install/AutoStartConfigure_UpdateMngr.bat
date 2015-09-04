@ECHO OFF
CLS

ECHO param1=%1
ECHO param2=%2
ECHO param3=%3
ECHO param4=%4
ECHO param5=%5

SET ROOTDIR=%CD%
SET ARCHITECTURE=%1
SET DEBUGRELEASE=%2
SET QT_VERSION=%3
SET LIBRARY_DIR=%4
IF [%4] EQU [] (FOR /F "usebackq tokens=1" %%i IN (`PowerShell -NoProfile -ExecutionPolicy Bypass -Command "Include\PowerShell\getXMLValue.ps1 -inputfile '..\Source\base.props' -tagname 'LIBRARY_DIR'"`) DO SET LIBRARY_DIR=%%i)
IF [%5] NEQ [] (
	CD /d %5
	ECHO Current path set to: %5
	)
ECHO Library directory set to: %LIBRARY_DIR%

SET QT_FETCHED_VERSION=
FOR /F "usebackq tokens=1" %%i IN (`PowerShell -NoProfile -ExecutionPolicy Bypass -Command "Include\PowerShell\getXMLValue.ps1 -inputfile '..\Source\base.props' -tagname 'QTVERSION'"`) DO SET QT_FETCHED_VERSION=%%i
IF [%1] EQU [] SET /p ARCHITECTURE= Start the win32(1)[=default] or the x64(2) version? 
IF [%2] EQU [] SET /p DEBUGRELEASE= Start the (d)ebug[=default] or (r)elease version? 
IF [%3] EQU [] SET /p QT_VERSION= Define the used Qt version, ie '%QT_FETCHED_VERSION%'[=default]? 		
IF /I '%QT_VERSION%'=='' SET QT_VERSION=%QT_FETCHED_VERSION%
ECHO %QT_VERSION%
IF /I '%DEBUGRELEASE%'=='' SET DEBUGRELEASE=d
IF /I '%ARCHITECTURE%'=='' SET ARCHITECTURE=1
ECHO Your input was: Architecture(%ARCHITECTURE%) on %QT_VERSION%
IF /I '%ARCHITECTURE%'=='1' GOTO WIN32
IF /I '%ARCHITECTURE%'=='2' GOTO X64
GOTO END
:WIN32
	ECHO WIN32 was choosen
	SET QT_ROOT_DIR=%LIBRARY_DIR%\Qt%QT_VERSION%_32bit\5.3\msvc2013_opengl
	ECHO %QT_ROOT_DIR%
	SET QT_QPA_PLATFORM_PLUGIN_PATH=%QT_ROOT_DIR%\plugins\platforms
	SET QUAZIP_PATH=%LIBRARY_DIR%\quazip-0.7.1\Win32
	SET Path=%QT_ROOT_DIR%\bin;%QUAZIP_PATH%;%Path%
	
	ECHO START COPYING...
	COPY qt_32.conf qt.conf /y
	
	IF /I '%DEBUGRELEASE%'=='d' ECHO Debug Mode was choosen
	IF /I '%DEBUGRELEASE%'=='d' SET Path=%QT_ROOT_DIR%\bin;%QUAZIP_PATH%\Debug;%Path%
	
	IF /I '%DEBUGRELEASE%'=='r' ECHO Release Mode was choosen
	IF /I '%DEBUGRELEASE%'=='r' SET Path=%QT_ROOT_DIR%\bin;%QUAZIP_PATH%\Release;%Path%
	
	CD /d %ROOTDIR%

	GOTO END
	
:X64
	ECHO X64 was choosen
	SET QT_ROOT_DIR=%LIBRARY_DIR%\Qt%QT_VERSION%_64bit\5.3\msvc2013_64_opengl
	SET QT_QPA_PLATFORM_PLUGIN_PATH=%QT_ROOT_DIR%\plugins\platforms
	SET QUAZIP_PATH=%LIBRARY_DIR%\quazip-0.7.1\x64
		
	ECHO START COPYING...
	COPY qt_64.conf qt.conf /y
	
	IF /I '%DEBUGRELEASE%'=='d' ECHO Debug Mode was choosen
	IF /I '%DEBUGRELEASE%'=='d' SET Path=%QT_ROOT_DIR%\bin;%QUAZIP_PATH%\Debug;%Path%
	
	IF /I '%DEBUGRELEASE%'=='r' ECHO Release Mode was choosen
	IF /I '%DEBUGRELEASE%'=='r' SET Path=%QT_ROOT_DIR%\bin;%QUAZIP_PATH%\Release;%Path%
	
	CD /d %ROOTDIR%

	GOTO END

	
:END
CD /d %ROOTDIR%