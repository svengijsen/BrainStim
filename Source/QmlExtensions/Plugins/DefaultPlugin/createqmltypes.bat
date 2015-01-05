@ECHO OFF
CLS

SET ARCHITECTURE=%1
IF [%1] EQU [] SET /p ARCHITECTURE= Start the win32(1)[=default] or the x64(2) version? 
IF /I '%ARCHITECTURE%'=='' SET ARCHITECTURE=1
ECHO Your input was: Architecture(%ARCHITECTURE%)

ECHO Step 1: Make sure to build latest version in 32-bit release, press a key when done...
PAUSE
ECHO Step 2: Open and edit the qmldir file in E:\Projects\BrainStim\Install\Qml\plugins\Win32\BrainStim_QMLExtensions, press a key when done...
PAUSE

IF /I '%ARCHITECTURE%'=='1' GOTO WIN32
IF /I '%ARCHITECTURE%'=='2' GOTO X64
GOTO END

:WIN32
	ECHO Win32 was choosen
	SET PATH=E:\Libraries\Ogre\OGRE-SDK-1.8.2-vc110-x86-28.05.2013\bin\Release;%PATH%
	set root=E:\Projects\BrainStim\Install\Qml\plugins\Win32
	CD /d %root%
	E:\Libraries\Qt5.3.2_32bit\5.3\msvc2013_opengl\bin\qmlplugindump.exe -notrelocatable BrainStim_QMLExtensions 1.0 /BrainStim_QMLExtensions > E:\Projects\BrainStim\Install\Qml\plugins\Win32\BrainStim_QMLExtensions\BrainStim_QMLExtensions.qmltypes
	ECHO Done!
	GOTO END
	
:X64
	ECHO X64 was choosen
	SET PATH=E:\Libraries\Ogre\OGRE-SDK-1.8.2-vc110-x64-28.05.2013\bin\Release;%PATH%
	set root=E:\Projects\BrainStim\Install\Qml\plugins\x64
	CD /d %root%
	E:\Libraries\Qt5.3.2_64bit\5.3\msvc2013_64_opengl\bin\qmlplugindump.exe -notrelocatable BrainStim_QMLExtensions 1.0 /BrainStim_QMLExtensions > E:\Projects\BrainStim\Install\Qml\plugins\x64\BrainStim_QMLExtensions\BrainStim_QMLExtensions.qmltypes
	ECHO Done!
	GOTO END
	
:END
PAUSE