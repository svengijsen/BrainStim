@ECHO OFF
CLS
ECHO param1=%1
ECHO param2=%2
ECHO param3=%3

SET ROOTDIR=%CD%
SET ARCHITECTURE=%1
SET DEBUGRELEASE=%2
SET QT_VERSION=%3
SET QSCINTILLA_PATH=E:\Libraries\QScintilla-gpl-2.7.2\lib
SET MATLAB_PATH=C:\Program Files\MATLAB\R2014a\bin\win64

IF [%1] EQU [] SET /p ARCHITECTURE= Start the win32(1)[=default] or the x64(2) version? 
IF [%2] EQU [] SET /p DEBUGRELEASE= Start the (d)ebug[=default] or (r)elease version? 
IF [%3] EQU [] SET /p QT_VERSION= Define the used Qt version, ie '5.3.2'[=default]? 
		
IF /I '%QT_VERSION%'=='' SET QT_VERSION=5.3.2
IF /I '%DEBUGRELEASE%'=='' SET DEBUGRELEASE=d
IF /I '%ARCHITECTURE%'=='' SET ARCHITECTURE=1
ECHO Your input was: Architecture(%ARCHITECTURE%) on %QT_VERSION%
ECHO Additional to use library paths:
ECHO %QSCINTILLA_PATH%
ECHO %MATLAB_PATH%
MD Xsd
COPY ..\Source\Plugins\ExperimentManager\xsd Xsd
IF /I '%ARCHITECTURE%'=='1' GOTO WIN32
IF /I '%ARCHITECTURE%'=='2' GOTO X64
GOTO END
:WIN32
	ECHO WIN32 was choosen
	SET QT_ROOT_DIR=E:\Libraries\Qt%QT_VERSION%_32bit\5.3\msvc2013_opengl
	ECHO %QT_ROOT_DIR%
	SET QT_QPA_PLATFORM_PLUGIN_PATH=%QT_ROOT_DIR%\plugins\platforms
	SET Path=%QT_ROOT_DIR%\bin;%Path%
	SET OGRE_PATH=E:\Libraries\Ogre\OGRE-SDK-1.8.2-vc110-x86-28.05.2013\bin
	SET INPOUT_PATH=E:\Libraries\InpOut_1500\Win32
	SET QT_SCRIPT_BINDINGS_PATH=E:\Libraries\Qt5.3.2_ScriptBindings\Win32\bindings\script
	SET QUAZIP_PATH=E:\Libraries\quazip-0.7.1\Win32
	
	ECHO START COPYING...
	COPY qt_32.conf qt.conf /y
	COPY %INPOUT_PATH%\inpout32.dll inpout32.dll /y
	MD Plugins\Win32\script
	COPY %QT_SCRIPT_BINDINGS_PATH% Plugins\Win32\script
	MD Qml\plugins\Win32\BrainStim_QMLExtensions
	COPY ..\Source\QmlExtensions\Plugins\DefaultPlugin\BrainStimExtensions Qml\plugins\Win32\BrainStim_QMLExtensions
	MD Plugins\Win32\platforms
	COPY %QT_ROOT_DIR%\plugins\platforms Plugins\Win32\platforms

	MD Plugins\Win32\audio
	COPY %QT_ROOT_DIR%\plugins\audio Plugins\Win32\audio
	MD Plugins\Win32\imageformats
	COPY %QT_ROOT_DIR%\plugins\imageformats Plugins\Win32\imageformats
	MD Plugins\Win32\mediaservice
	COPY %QT_ROOT_DIR%\plugins\mediaservice Plugins\Win32\mediaservice
	MD Plugins\Win32\platforms
	COPY %QT_ROOT_DIR%\plugins\platforms Plugins\Win32\platforms
	MD Plugins\Win32\playlistformats
	COPY %QT_ROOT_DIR%\plugins\playlistformats Plugins\Win32\playlistformats
	MD Plugins\Win32\printsupport
	COPY %QT_ROOT_DIR%\plugins\printsupport Plugins\Win32\printsupport
	MD Plugins\Win32\sqldrivers
	COPY %QT_ROOT_DIR%\plugins\sqldrivers Plugins\Win32\sqldrivers

	SET QML_PLUGIN_PRE_DIR=Qml\plugins\Win32
	MD %QML_PLUGIN_PRE_DIR%\Enginio
	COPY %QT_ROOT_DIR%\qml\Enginio %QML_PLUGIN_PRE_DIR%\Enginio
	MD %QML_PLUGIN_PRE_DIR%\Qt
	COPY %QT_ROOT_DIR%\qml\Qt %QML_PLUGIN_PRE_DIR%\Qt
	MD %QML_PLUGIN_PRE_DIR%\QtBluetooth
	COPY %QT_ROOT_DIR%\qml\QtBluetooth %QML_PLUGIN_PRE_DIR%\QtBluetooth
	MD %QML_PLUGIN_PRE_DIR%\QtGraphicalEffects
	COPY %QT_ROOT_DIR%\qml\QtGraphicalEffects %QML_PLUGIN_PRE_DIR%\QtGraphicalEffects
	MD %QML_PLUGIN_PRE_DIR%\QtMultimedia
	COPY %QT_ROOT_DIR%\qml\QtMultimedia %QML_PLUGIN_PRE_DIR%\QtMultimedia
	MD %QML_PLUGIN_PRE_DIR%\QtNfc
	COPY %QT_ROOT_DIR%\qml\QtNfc %QML_PLUGIN_PRE_DIR%\QtNfc
	MD %QML_PLUGIN_PRE_DIR%\QtPositioning
	COPY %QT_ROOT_DIR%\qml\QtPositioning %QML_PLUGIN_PRE_DIR%\QtPositioning
	MD %QML_PLUGIN_PRE_DIR%\QtQml
	COPY %QT_ROOT_DIR%\qml\QtQml %QML_PLUGIN_PRE_DIR%\QtQml
	MD %QML_PLUGIN_PRE_DIR%\QtQuick
	COPY %QT_ROOT_DIR%\qml\QtQuick %QML_PLUGIN_PRE_DIR%\QtQuick
	MD %QML_PLUGIN_PRE_DIR%\QtQuick.2
	COPY %QT_ROOT_DIR%\qml\QtQuick.2 %QML_PLUGIN_PRE_DIR%\QtQuick.2
	MD %QML_PLUGIN_PRE_DIR%\QtSensors
	COPY %QT_ROOT_DIR%\qml\QtSensors %QML_PLUGIN_PRE_DIR%\QtSensors
	MD %QML_PLUGIN_PRE_DIR%\QtTest
	COPY %QT_ROOT_DIR%\qml\QtTest %QML_PLUGIN_PRE_DIR%\QtTest
	MD %QML_PLUGIN_PRE_DIR%\QtWebKit
	COPY %QT_ROOT_DIR%\qml\QtWebKit %QML_PLUGIN_PRE_DIR%\QtWebKit
	MD %QML_PLUGIN_PRE_DIR%\QtWinExtras
	COPY %QT_ROOT_DIR%\qml\QtWinExtras %QML_PLUGIN_PRE_DIR%\QtWinExtras
	MD %QML_PLUGIN_PRE_DIR%\Qt\labs
	COPY %QT_ROOT_DIR%\qml\Qt\labs %QML_PLUGIN_PRE_DIR%\Qt\labs
	MD %QML_PLUGIN_PRE_DIR%\Qt\WebSockets
	COPY %QT_ROOT_DIR%\qml\Qt\WebSockets %QML_PLUGIN_PRE_DIR%\Qt\WebSockets
	MD %QML_PLUGIN_PRE_DIR%\Qt\labs\folderlistmodel
	COPY %QT_ROOT_DIR%\qml\Qt\labs\folderlistmodel %QML_PLUGIN_PRE_DIR%\Qt\labs\folderlistmodel
	MD %QML_PLUGIN_PRE_DIR%\Qt\labs\settings
	COPY %QT_ROOT_DIR%\qml\Qt\labs\settings %QML_PLUGIN_PRE_DIR%\Qt\labs\settings
	MD %QML_PLUGIN_PRE_DIR%\QtGraphicalEffects\private
	COPY %QT_ROOT_DIR%\qml\QtGraphicalEffects\private %QML_PLUGIN_PRE_DIR%\QtGraphicalEffects\private
	MD %QML_PLUGIN_PRE_DIR%\QtQml\Models.2
	COPY %QT_ROOT_DIR%\qml\QtQml\Models.2 %QML_PLUGIN_PRE_DIR%\QtQml\Models.2
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls %QML_PLUGIN_PRE_DIR%\QtQuick\Controls
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs
	COPY %QT_ROOT_DIR%\qml\QtQuick\Dialogs %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Layouts
	COPY %QT_ROOT_DIR%\qml\QtQuick\Layouts %QML_PLUGIN_PRE_DIR%\QtQuick\Layouts
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\LocalStorage
	COPY %QT_ROOT_DIR%\qml\QtQuick\LocalStorage %QML_PLUGIN_PRE_DIR%\QtQuick\LocalStorage
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Particles.2
	COPY %QT_ROOT_DIR%\qml\QtQuick\Particles.2 %QML_PLUGIN_PRE_DIR%\QtQuick\Particles.2
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\PrivateWidgets
	COPY %QT_ROOT_DIR%\qml\QtQuick\PrivateWidgets %QML_PLUGIN_PRE_DIR%\QtQuick\PrivateWidgets
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Window.2
	COPY %QT_ROOT_DIR%\qml\QtQuick\Window.2 %QML_PLUGIN_PRE_DIR%\QtQuick\Window.2
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\XmlListModel
	COPY %QT_ROOT_DIR%\qml\QtQuick\XmlListModel %QML_PLUGIN_PRE_DIR%\QtQuick\XmlListModel
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Private
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls\Private %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Private
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls\Styles %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Base
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls\Styles\Base %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Base
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Desktop
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls\Styles\Desktop %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Desktop
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Base\images
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls\Styles\Base\images %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Base\images
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\images
	COPY %QT_ROOT_DIR%\qml\QtQuick\Dialogs\images %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\images
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\Private
	COPY %QT_ROOT_DIR%\qml\QtQuick\Dialogs\Private %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\Private
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\qml
	COPY %QT_ROOT_DIR%\qml\QtQuick\Dialogs\qml %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\qml
	MD %QML_PLUGIN_PRE_DIR%\QtWebKit\experimental
	COPY %QT_ROOT_DIR%\qml\QtWebKit\experimental %QML_PLUGIN_PRE_DIR%\QtWebKit\experimental
	
	IF /I '%DEBUGRELEASE%'=='d' ECHO Debug Mode was choosen
	IF /I '%DEBUGRELEASE%'=='d' SET Path=%QSCINTILLA_PATH%\Win32\Debug;%Path%
	IF /I '%DEBUGRELEASE%'=='d' COPY %OGRE_PATH%\Debug\OgreMain_d.dll OgreMain_d.dll /y
	IF /I '%DEBUGRELEASE%'=='d' COPY %OGRE_PATH%\Debug\RenderSystem_GL_d.dll RenderSystem_GL_d.dll /y
	IF /I '%DEBUGRELEASE%'=='d' COPY %QUAZIP_PATH%\Debug\quazip.dll quazip.dll /y
	IF /I '%DEBUGRELEASE%'=='d' COPY "..\Source\QmlExtensions\Plugins\DefaultPlugin\qmldir_debug_win32" "Qml\plugins\Win32\BrainStim_QMLExtensions\qmldir" /y
	IF /I '%DEBUGRELEASE%'=='d' COPY %QT_ROOT_DIR%\bin\Qt5SerialPortd.dll Qt5SerialPortd.dll /y
	
	
	IF /I '%DEBUGRELEASE%'=='r' ECHO Release Mode was choosen
	IF /I '%DEBUGRELEASE%'=='r' SET Path=%QSCINTILLA_PATH%\Win32\Release;%Path%
	IF /I '%DEBUGRELEASE%'=='r' COPY %OGRE_PATH%\Release\OgreMain.dll OgreMain.dll /y
	IF /I '%DEBUGRELEASE%'=='r' COPY %OGRE_PATH%\Release\RenderSystem_GL.dll RenderSystem_GL.dll /y
	IF /I '%DEBUGRELEASE%'=='r' COPY %QUAZIP_PATH%\Release\quazip.dll quazip.dll /y
	IF /I '%DEBUGRELEASE%'=='r' COPY "..\Source\QmlExtensions\Plugins\DefaultPlugin\qmldir_release_win32" "Qml\plugins\Win32\BrainStim_QMLExtensions\qmldir" /y
	IF /I '%DEBUGRELEASE%'=='r' COPY %QT_ROOT_DIR%\bin\Qt5SerialPort.dll Qt5SerialPort.dll /y
	IF /I '%DEBUGRELEASE%'=='r' CALL "..\Source\QmlExtensions\Plugins\DefaultPlugin\createqmltypes.bat"
	
	SET Path=%MATLAB_PATH%;%Path%
	
	CD /d %ROOTDIR%

	GOTO END
	
:X64
	ECHO X64 was choosen
	SET QT_ROOT_DIR=E:\Libraries\Qt%QT_VERSION%_64bit\5.3\msvc2013_64_opengl
	QT_QPA_PLATFORM_PLUGIN_PATH=%QT_ROOT_DIR%\plugins\platforms
	SET Path=%QT_ROOT_DIR%\bin;%Path%
	SET OGRE_PATH=E:\Libraries\Ogre\OGRE-SDK-1.8.2-vc110-x64-28.05.2013\bin
	SET INPOUT_PATH=E:\Libraries\InpOut_1500\x64
	SET QT_SCRIPT_BINDINGS_PATH=E:\Libraries\Qt5.3.2_ScriptBindings\x64\bindings\script
	SET QUAZIP_PATH=E:\Libraries\quazip-0.7.1\x64
	
	ECHO START COPYING...
	COPY qt_64.conf qt.conf /y
	COPY %INPOUT_PATH%\inpoutx64.dll inpoutx64.dll /y
	MD Plugins\x64\script
	COPY %QT_SCRIPT_BINDINGS_PATH% Plugins\x64\script
	MD Qml\plugins\x64\BrainStim_QMLExtensions
	COPY ..\Source\QmlExtensions\Plugins\DefaultPlugin\BrainStimExtensions Qml\plugins\x64\BrainStim_QMLExtensions
	MD Plugins\x64\platforms
	COPY %QT_ROOT_DIR%\plugins\platforms Plugins\x64\platforms
	
	MD Plugins\x64\audio
	COPY %QT_ROOT_DIR%\plugins\audio Plugins\x64\audio
	MD Plugins\x64\imageformats
	COPY %QT_ROOT_DIR%\plugins\imageformats Plugins\x64\imageformats
	MD Plugins\x64\mediaservice
	COPY %QT_ROOT_DIR%\plugins\mediaservice Plugins\x64\mediaservice
	MD Plugins\x64\platforms
	COPY %QT_ROOT_DIR%\plugins\platforms Plugins\x64\platforms
	MD Plugins\x64\playlistformats
	COPY %QT_ROOT_DIR%\plugins\playlistformats Plugins\x64\playlistformats
	MD Plugins\x64\printsupport
	COPY %QT_ROOT_DIR%\plugins\printsupport Plugins\x64\printsupport
	MD Plugins\x64\sqldrivers
	COPY %QT_ROOT_DIR%\plugins\sqldrivers Plugins\x64\sqldrivers
	
	SET QML_PLUGIN_PRE_DIR=Qml\plugins\x64
	MD %QML_PLUGIN_PRE_DIR%\Enginio
	COPY %QT_ROOT_DIR%\qml\Enginio %QML_PLUGIN_PRE_DIR%\Enginio
	MD %QML_PLUGIN_PRE_DIR%\Qt
	COPY %QT_ROOT_DIR%\qml\Qt %QML_PLUGIN_PRE_DIR%\Qt
	MD %QML_PLUGIN_PRE_DIR%\QtBluetooth
	COPY %QT_ROOT_DIR%\qml\QtBluetooth %QML_PLUGIN_PRE_DIR%\QtBluetooth
	MD %QML_PLUGIN_PRE_DIR%\QtGraphicalEffects
	COPY %QT_ROOT_DIR%\qml\QtGraphicalEffects %QML_PLUGIN_PRE_DIR%\QtGraphicalEffects
	MD %QML_PLUGIN_PRE_DIR%\QtMultimedia
	COPY %QT_ROOT_DIR%\qml\QtMultimedia %QML_PLUGIN_PRE_DIR%\QtMultimedia
	MD %QML_PLUGIN_PRE_DIR%\QtNfc
	COPY %QT_ROOT_DIR%\qml\QtNfc %QML_PLUGIN_PRE_DIR%\QtNfc
	MD %QML_PLUGIN_PRE_DIR%\QtPositioning
	COPY %QT_ROOT_DIR%\qml\QtPositioning %QML_PLUGIN_PRE_DIR%\QtPositioning
	MD %QML_PLUGIN_PRE_DIR%\QtQml
	COPY %QT_ROOT_DIR%\qml\QtQml %QML_PLUGIN_PRE_DIR%\QtQml
	MD %QML_PLUGIN_PRE_DIR%\QtQuick
	COPY %QT_ROOT_DIR%\qml\QtQuick %QML_PLUGIN_PRE_DIR%\QtQuick
	MD %QML_PLUGIN_PRE_DIR%\QtQuick.2
	COPY %QT_ROOT_DIR%\qml\QtQuick.2 %QML_PLUGIN_PRE_DIR%\QtQuick.2
	MD %QML_PLUGIN_PRE_DIR%\QtSensors
	COPY %QT_ROOT_DIR%\qml\QtSensors %QML_PLUGIN_PRE_DIR%\QtSensors
	MD %QML_PLUGIN_PRE_DIR%\QtTest
	COPY %QT_ROOT_DIR%\qml\QtTest %QML_PLUGIN_PRE_DIR%\QtTest
	MD %QML_PLUGIN_PRE_DIR%\QtWebKit
	COPY %QT_ROOT_DIR%\qml\QtWebKit %QML_PLUGIN_PRE_DIR%\QtWebKit
	MD %QML_PLUGIN_PRE_DIR%\QtWinExtras
	COPY %QT_ROOT_DIR%\qml\QtWinExtras %QML_PLUGIN_PRE_DIR%\QtWinExtras
	MD %QML_PLUGIN_PRE_DIR%\Qt\labs
	COPY %QT_ROOT_DIR%\qml\Qt\labs %QML_PLUGIN_PRE_DIR%\Qt\labs
	MD %QML_PLUGIN_PRE_DIR%\Qt\WebSockets
	COPY %QT_ROOT_DIR%\qml\Qt\WebSockets %QML_PLUGIN_PRE_DIR%\Qt\WebSockets
	MD %QML_PLUGIN_PRE_DIR%\Qt\labs\folderlistmodel
	COPY %QT_ROOT_DIR%\qml\Qt\labs\folderlistmodel %QML_PLUGIN_PRE_DIR%\Qt\labs\folderlistmodel
	MD %QML_PLUGIN_PRE_DIR%\Qt\labs\settings
	COPY %QT_ROOT_DIR%\qml\Qt\labs\settings %QML_PLUGIN_PRE_DIR%\Qt\labs\settings
	MD %QML_PLUGIN_PRE_DIR%\QtGraphicalEffects\private
	COPY %QT_ROOT_DIR%\qml\QtGraphicalEffects\private %QML_PLUGIN_PRE_DIR%\QtGraphicalEffects\private
	MD %QML_PLUGIN_PRE_DIR%\QtQml\Models.2
	COPY %QT_ROOT_DIR%\qml\QtQml\Models.2 %QML_PLUGIN_PRE_DIR%\QtQml\Models.2
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls %QML_PLUGIN_PRE_DIR%\QtQuick\Controls
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs
	COPY %QT_ROOT_DIR%\qml\QtQuick\Dialogs %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Layouts
	COPY %QT_ROOT_DIR%\qml\QtQuick\Layouts %QML_PLUGIN_PRE_DIR%\QtQuick\Layouts
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\LocalStorage
	COPY %QT_ROOT_DIR%\qml\QtQuick\LocalStorage %QML_PLUGIN_PRE_DIR%\QtQuick\LocalStorage
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Particles.2
	COPY %QT_ROOT_DIR%\qml\QtQuick\Particles.2 %QML_PLUGIN_PRE_DIR%\QtQuick\Particles.2
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\PrivateWidgets
	COPY %QT_ROOT_DIR%\qml\QtQuick\PrivateWidgets %QML_PLUGIN_PRE_DIR%\QtQuick\PrivateWidgets
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Window.2
	COPY %QT_ROOT_DIR%\qml\QtQuick\Window.2 %QML_PLUGIN_PRE_DIR%\QtQuick\Window.2
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\XmlListModel
	COPY %QT_ROOT_DIR%\qml\QtQuick\XmlListModel %QML_PLUGIN_PRE_DIR%\QtQuick\XmlListModel
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Private
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls\Private %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Private
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls\Styles %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Base
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls\Styles\Base %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Base
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Desktop
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls\Styles\Desktop %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Desktop
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Base\images
	COPY %QT_ROOT_DIR%\qml\QtQuick\Controls\Styles\Base\images %QML_PLUGIN_PRE_DIR%\QtQuick\Controls\Styles\Base\images
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\images
	COPY %QT_ROOT_DIR%\qml\QtQuick\Dialogs\images %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\images
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\Private
	COPY %QT_ROOT_DIR%\qml\QtQuick\Dialogs\Private %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\Private
	MD %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\qml
	COPY %QT_ROOT_DIR%\qml\QtQuick\Dialogs\qml %QML_PLUGIN_PRE_DIR%\QtQuick\Dialogs\qml
	MD %QML_PLUGIN_PRE_DIR%\QtWebKit\experimental
	COPY %QT_ROOT_DIR%\qml\QtWebKit\experimental %QML_PLUGIN_PRE_DIR%\QtWebKit\experimental

	IF /I '%DEBUGRELEASE%'=='d' ECHO Debug Mode was choosen
	IF /I '%DEBUGRELEASE%'=='d' SET Path=%QSCINTILLA_PATH%\x64\Debug;%Path%
	IF /I '%DEBUGRELEASE%'=='d' COPY %OGRE_PATH%\Debug\OgreMain_d.dll OgreMain_d.dll /y
	IF /I '%DEBUGRELEASE%'=='d' COPY %OGRE_PATH%\Debug\RenderSystem_GL_d.dll RenderSystem_GL_d.dll /y
	IF /I '%DEBUGRELEASE%'=='d' COPY %QUAZIP_PATH%\Debug\quazip.dll quazip.dll /y
	IF /I '%DEBUGRELEASE%'=='d' COPY "..\Source\QmlExtensions\Plugins\DefaultPlugin\qmldir_debug_x64" "Qml\plugins\x64\BrainStim_QMLExtensions\qmldir" /y
	IF /I '%DEBUGRELEASE%'=='d' COPY %QT_ROOT_DIR%\bin\Qt5SerialPortd.dll Qt5SerialPortd.dll /y

	IF /I '%DEBUGRELEASE%'=='r' ECHO Release Mode was choosen
	IF /I '%DEBUGRELEASE%'=='r' SET Path=%QSCINTILLA_PATH%\x64\Release;%Path%
	IF /I '%DEBUGRELEASE%'=='r' COPY %OGRE_PATH%\Release\OgreMain.dll OgreMain.dll /y
	IF /I '%DEBUGRELEASE%'=='r' COPY %OGRE_PATH%\Release\RenderSystem_GL.dll RenderSystem_GL.dll /y
	IF /I '%DEBUGRELEASE%'=='r' COPY %QUAZIP_PATH%\Release\quazip.dll quazip.dll /y
	IF /I '%DEBUGRELEASE%'=='r' COPY "..\Source\QmlExtensions\Plugins\DefaultPlugin\qmldir_release_x64" "Qml\plugins\x64\BrainStim_QMLExtensions\qmldir" /y	
	IF /I '%DEBUGRELEASE%'=='r' COPY %QT_ROOT_DIR%\bin\Qt5SerialPort.dll Qt5SerialPort.dll /y
	IF /I '%DEBUGRELEASE%'=='r' CALL "..\Source\QmlExtensions\Plugins\DefaultPlugin\createqmltypes.bat"
	
	SET Path=%MATLAB_PATH%;%Path%
	
	CD /d %ROOTDIR%

	GOTO END

	
:END
CD /d %ROOTDIR%
REM PAUSE
