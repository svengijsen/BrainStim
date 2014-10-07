SET PATH=C:\Qt\Ogre\OGRE-SDK-1.8.2-vc110-x86-28.05.2013\bin\Release;%PATH%
set root=E:\Projects\BrainStim\Install\Qml\plugins\Win32
CD /d %root%
C:\Qt\Qt5.3.2_32bit\5.3\msvc2013_opengl\bin\qmlplugindump.exe -notrelocatable BrainStim_QMLExtensions 1.0 /BrainStim_QMLExtensions > C:\Users\svengijsen\Documents\Projects\BrainStim\Install\Qml\plugins\Win32\BrainStim_QMLExtensions\BrainStim_QMLExtensions.qmltypes

SET PATH=C:\Qt\Ogre\OGRE-SDK-1.8.2-vc110-x64-28.05.2013\bin\Release;%PATH%
set root=E:\Projects\BrainStim\Install\Qml\plugins\x64
CD /d %root%
C:\Qt\Qt5.3.2_64bit\5.3\msvc2013_64_opengl\bin\qmlplugindump.exe -notrelocatable BrainStim_QMLExtensions 1.0 /BrainStim_QMLExtensions > C:\Users\svengijsen\Documents\Projects\BrainStim\Install\Qml\plugins\x64\BrainStim_QMLExtensions\BrainStim_QMLExtensions.qmltypes