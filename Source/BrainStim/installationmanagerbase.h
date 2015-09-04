//BrainStim
//Copyright (C) 2015  Sven Gijsen
//
//This file is part of BrainStim.
//BrainStim is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef INSTALLATIONMANAGERBASE_H
#define INSTALLATIONMANAGERBASE_H

#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>
#include "mainappinfo.h"
#include <QTemporaryDir>
#include <QLibrary>

#ifndef QUAZIP_STATIC
#define QUAZIP_STATIC
#endif
#include "quazip/JlCompress.h"

//Windows UAC
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
 
#define INSTALLMNGR_SETTING_SECTION_CONFIGURATION	"Configuration"
#define INSTALLMNGR_SETTING_SECTION_INSTALLATION	"Installation"
#define INSTALLMNGR_SETTING_SETTING_ENABLED			"Enabled"
#define INSTALLMNGR_SETTING_SETTING_NAME			"Name"
#define INSTALLMNGR_SETTING_SETTING_FILES			"Files"

#define INSTALLMNGR_SETTING_MACRO_APPLICATIONPATH	"<MainAppPath>"			//These can be shared (are read-only) and need administrator privileges to install
#define INSTALLMNGR_SETTING_MACRO_DEFAULTPLUGINPATH	"<PluginDefaultPath>"	//These can be shared (are read-only) and need administrator privileges to install
#define INSTALLMNGR_SETTING_MACRO_USERPLUGINPATH	"<PluginUserPath>"
#define INSTALLMNGR_SETTING_MACRO_PLUGINNAME		"<PluginName>"
#define INSTALLMNGR_SETTING_MACRO_USERAPPPATH		"<UserAppPath>"

#define INSTALLMNGR_UPDATEMNGR_FILENAME				"UpdateMngr"
#define INSTALLMNGR_UPDATEMNGR_BATCH_FILENAME		"AutoStartConfigure_UpdateMngr.bat"
#define INSTALLMNGR_ELEVATION_BATCH_FILENAME		"Elevation.bat"

#define INSTALLMNGR_INSTALL_INI_FILEEXT				"ini"
#define INSTALLMNGR_INSTALL_ZIP_FILEEXT				"zip"
#define INSTALLMNGR_INSTALL_LST_FILEEXT				"lst"

class installationManagerBase
{
public:

	enum InstallResult
	{
		InstallResult_PluginsInstalled					=  1,
		InstallResult_NoChanges							=  0, 
		InstallResult_ErrorNoValidSourceFileInZipFound	= -1,
		InstallResult_ErrorCreatingTempExtractDir		= -2,
		InstallResult_ErrorUnknownFileType				= -3,
		InstallResult_ErrorIniFileInsidePluginsDir		= -4,
		InstallResult_ErrorCannotOverwriteExistingFile	= -5,
		InstallResult_ErrorNotAllSourceFilesAvailable	= -6,
		InstallResult_ErrorCannotReplaceOriginalFile	= -7,
		InstallResult_ErrorCannotCreateDestinationPath	= -8,
		InstallResult_ErrorCannotCopySourceFile			= -9,
		InstallResult_ErrorIniFileNotFound				= -10,
		InstallResult_ErrorInsufficientRights			= -11,
		InstallResult_ErrorCouldNotReadListFile			= -12,
		InstallResult_ErrorNoValidSourceFileInLstFound  = -13,
		InstallResult_ErrorNoValidLstSourceFile			= -14,
		InstallResult_ErrorRenaming						= -15
	};
	
	installationManagerBase(QObject *parent = NULL) {Q_UNUSED(parent)};
	~installationManagerBase() {};

	static QString resolveFileDirectoryPath(const QString &sFileDirectoryPath, const QString &sMainAppsUserPluginDirPath, const QString &sMainAppDirPath, const QString &sIniCompleteBaseName, const QString &sMainAppsDefaultPluginDirPath, const QString &sUserAppPathPath, bool &bNeedsAdminPrivileges)
	{
		QString sRetval = sFileDirectoryPath;
		if ((sRetval.contains(INSTALLMNGR_SETTING_MACRO_APPLICATIONPATH, Qt::CaseInsensitive)) || (sRetval.contains(INSTALLMNGR_SETTING_MACRO_DEFAULTPLUGINPATH, Qt::CaseInsensitive)))
			bNeedsAdminPrivileges = true;
		//else
		//	bNeedsAdminPrivileges = false;
		sRetval.replace(INSTALLMNGR_SETTING_MACRO_USERPLUGINPATH, sMainAppsUserPluginDirPath, Qt::CaseInsensitive);
		sRetval.replace(INSTALLMNGR_SETTING_MACRO_APPLICATIONPATH, sMainAppDirPath, Qt::CaseInsensitive);
		sRetval.replace(INSTALLMNGR_SETTING_MACRO_PLUGINNAME, sIniCompleteBaseName, Qt::CaseInsensitive); //QFileInfo(sIniFileName).completeBaseName(), Qt::CaseInsensitive);
		sRetval.replace(INSTALLMNGR_SETTING_MACRO_DEFAULTPLUGINPATH, sMainAppsDefaultPluginDirPath, Qt::CaseInsensitive);
		sRetval.replace(INSTALLMNGR_SETTING_MACRO_USERAPPPATH, sUserAppPathPath, Qt::CaseInsensitive);
		return sRetval;
	};

	static InstallResult installPlugin(const QString &sMainAppDirPath, const QString &sMainAppsDefaultPluginDirPath, const QString &sMainAppsUserPluginDirPath, const QString &sPluginInstallFilePath, const QString &sUserAppPath, const bool &bOverWriteExistingFiles, const bool &bRenameSourceInstallFileOnSuccess = false)
	{
		QFileInfo fiPluginInstallFilePath(sPluginInstallFilePath);
		QString sIniFilePath = "";
		if (fiPluginInstallFilePath.completeSuffix() == INSTALLMNGR_INSTALL_INI_FILEEXT)
		{
			sIniFilePath = sPluginInstallFilePath;
		}
		else if (fiPluginInstallFilePath.completeSuffix() == INSTALLMNGR_INSTALL_LST_FILEEXT)
		{
			QFile fPluginInstallFilePath(sPluginInstallFilePath);
			if (fPluginInstallFilePath.exists())
			{
				if (fPluginInstallFilePath.open(QIODevice::ReadOnly) == false)
				{
					qDebug() << __FUNCTION__ << "The installation list file could not be opened, " + fPluginInstallFilePath.errorString();
					return InstallResult_ErrorCouldNotReadListFile;
				}
				else
				{
					QTextStream in(&fPluginInstallFilePath);
					int nRetVal = 0;
					while (!in.atEnd())
					{
						QString sTmpIniFile = in.readLine();
						if (QFile(sTmpIniFile).exists())
						{
							if ((QFileInfo(sTmpIniFile).completeSuffix() == INSTALLMNGR_INSTALL_INI_FILEEXT) || (QFileInfo(sTmpIniFile).completeSuffix() == INSTALLMNGR_INSTALL_ZIP_FILEEXT))
							{
								int nTempRetVal = installPlugin(sMainAppDirPath, sMainAppsDefaultPluginDirPath, sMainAppsUserPluginDirPath, sTmpIniFile, sUserAppPath, bOverWriteExistingFiles, bRenameSourceInstallFileOnSuccess);
								if (nTempRetVal < 0)
								{
									nRetVal = nTempRetVal;
									break;
								}
								nRetVal = nRetVal + nTempRetVal;
							}
						}
					}
					fPluginInstallFilePath.close();
					if (nRetVal > 0)
					{
						if (bRenameSourceInstallFileOnSuccess)
							if (renameInstalledFile(fPluginInstallFilePath.fileName()) == false)
								return InstallResult_ErrorRenaming;
						return InstallResult_PluginsInstalled;
					}
					qDebug() << __FUNCTION__ << "The lst-file doesn't seem to include a valid source file!";
					return InstallResult_ErrorNoValidSourceFileInLstFound;
				}
			}
			qDebug() << __FUNCTION__ << "No valid lst-file found!";
			return InstallResult_ErrorNoValidLstSourceFile;
		}
		else if (fiPluginInstallFilePath.completeSuffix() == INSTALLMNGR_INSTALL_ZIP_FILEEXT)
		{			
			QStringList lArchiverFiles = JlCompress::getFileList(sPluginInstallFilePath);
			QStringList lArchiverIniFiles;
			foreach(QString sTmpFileArchiverPath, lArchiverFiles)
			{
				if (QFileInfo(sTmpFileArchiverPath).completeSuffix() == INSTALLMNGR_INSTALL_INI_FILEEXT)
					lArchiverIniFiles.append(sTmpFileArchiverPath);
			}
			if (lArchiverIniFiles.isEmpty())
			{
				qDebug() << __FUNCTION__ << "The compressed file doesn't seem to be a valid installer package, no configuration (*.ini) file found!";
				return InstallResult_ErrorNoValidSourceFileInZipFound;
			}
			QTemporaryDir dir;
			if (dir.isValid() == false)
			{
				qDebug() << __FUNCTION__ << "Could not create a temporarily directory for the extraction of the of the installer package, please try to extract it yourself and use the extracted configuration (*.ini) file for the installation process!";
				return InstallResult_ErrorCreatingTempExtractDir;
			}
			QStringList lExtractedFiles = JlCompress::extractDir(sPluginInstallFilePath, dir.path());
			int nRetVal = 0;
			foreach(QString sTmpExtractedFilePath, lExtractedFiles)
			{
				if ((QFileInfo(sTmpExtractedFilePath).completeSuffix() == INSTALLMNGR_INSTALL_INI_FILEEXT) || (QFileInfo(sTmpExtractedFilePath).completeSuffix() == INSTALLMNGR_INSTALL_ZIP_FILEEXT))
				{
					int nTempRetVal = installPlugin(sMainAppDirPath, sMainAppsDefaultPluginDirPath, sMainAppsUserPluginDirPath, sTmpExtractedFilePath, sUserAppPath, bOverWriteExistingFiles, bRenameSourceInstallFileOnSuccess);
					if (nTempRetVal < 0)
					{
						nRetVal = nTempRetVal;
						break;
					}
					nRetVal = nRetVal + nTempRetVal;
				}
			}
			dir.remove();
			if (nRetVal > 0)
			{
				if (bRenameSourceInstallFileOnSuccess)
					if (renameInstalledFile(sPluginInstallFilePath) == false)
						return InstallResult_ErrorRenaming;
				return InstallResult_PluginsInstalled;
			}
		}
		else
		{
			return InstallResult_ErrorUnknownFileType;
		}

		QFile fPluginIniFile(sIniFilePath);
		if (fPluginIniFile.exists())
		{
			QString sIniFileAbsDir = QFileInfo(sIniFilePath).absolutePath();
			if (QDir(sMainAppsUserPluginDirPath).canonicalPath() == QDir(sIniFileAbsDir).canonicalPath())//Is a INI file inside the plugins directory? 
				return InstallResult_ErrorIniFileInsidePluginsDir;

			QSettings pluginSettings(sIniFilePath, QSettings::IniFormat);
			pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_CONFIGURATION);
			QString sPluginInternalName = pluginSettings.value(INSTALLMNGR_SETTING_SETTING_NAME, "").toString();
			pluginSettings.endGroup();
			pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_INSTALLATION);
			QStringList lFileList;
			lFileList = pluginSettings.value(INSTALLMNGR_SETTING_SETTING_FILES, QStringList()).toStringList();
			pluginSettings.endGroup();

			QString sIniFileName = QFileInfo(sIniFilePath).fileName();
			//if (isRegisteredPlugin(QFileInfo(sIniFilePath).baseName()))
			if (QFile(sMainAppsUserPluginDirPath + "/" + sIniFileName).exists())
			{
				//int confirm = QMessageBox::question(NULL, "Uninstall already present plugin?", "A plugin with the same name (" + sPluginInternalName + ") is already in use.\nTo proceed this installation that active plugin first needs to be un-installed, do you wish to proceed?", QMessageBox::Ok | QMessageBox::Cancel);
				if (bOverWriteExistingFiles == false)//(confirm == QMessageBox::Ok)
				{
					//if (unistallRegisteredPlugin(sPluginInternalName) == false)
					//{
					//	qDebug() << __FUNCTION__ << "Could not uninstall the plugin: " << sPluginInternalName;
					//	return ??;
					//}
					//}
					//else// if (QMessageBox::Cancel)
					//{
					qDebug() << __FUNCTION__ << "File already exists (" + sMainAppsUserPluginDirPath + "/" + sIniFileName + ")!" << sPluginInternalName;
					return InstallResult_ErrorCannotOverwriteExistingFile;
				}
			}

			if (lFileList.isEmpty() == false)
			{
				//Check whether all files are present and mark those indexes of files which already exist in installation directory
				bool bAllFilesAvailableCheck = true;
				QList<int> nPresentInCurrentInstallIndexes;
				bool bNeedsAdminPrivileges = false;
				for (int i = 0; i < lFileList.count(); i++)
				{
					lFileList[i] = resolveFileDirectoryPath(lFileList[i], sMainAppsUserPluginDirPath, sMainAppDirPath, QFileInfo(sIniFileName).completeBaseName(), sMainAppsDefaultPluginDirPath, sUserAppPath, bNeedsAdminPrivileges);
				}
				if (bNeedsAdminPrivileges)
				{
					if (IsRunAsAdministrator() == false)
					{
						qDebug() << "The plugin " << sPluginInternalName << " needs administrator privileges to install!";
						return InstallResult_ErrorInsufficientRights;
					}
				}
				for (int i = 0; i < lFileList.count(); i++)
				{
					QString sFileName = QFileInfo(lFileList[i]).fileName();
					QFile fSourceFilePath(sIniFileAbsDir + "/" + sFileName);
					QFile fInstallationFilePath(lFileList[i]); //sMainAppsPluginDirPath + "/" + lFileList[i]);
					if (fSourceFilePath.exists() == false)
					{
						bAllFilesAvailableCheck = false;
						break;
					}
					if (fInstallationFilePath.exists())// QFileInfo(sMainAppsPluginDirPath + "/" + lFileList[i]).exists())
						nPresentInCurrentInstallIndexes.append(i);
				}
				if (bAllFilesAvailableCheck == false)
					return InstallResult_ErrorNotAllSourceFilesAvailable;

				if (nPresentInCurrentInstallIndexes.isEmpty() == false)
				{
					//Todo handle shared files here!!, first disable all and then reboot, install and enable all
					if (bOverWriteExistingFiles == false)
					{
						QString sSkippedFiles;
						foreach(int nIndex, nPresentInCurrentInstallIndexes)
						{
							sSkippedFiles = sSkippedFiles + "\n" + lFileList[nIndex];
						}
						qDebug() << __FUNCTION__ << "Some installation files (for the " << sPluginInternalName << " plugin) were skipped because they were already present and in use: " << sSkippedFiles;
						//see below uncommented copy step!!!
					}
				}
				//Copy the files to the installation folder
				for (int i = 0; i < lFileList.count(); i++)
				{
					if ((nPresentInCurrentInstallIndexes.contains(i) == false) || (bOverWriteExistingFiles))
					{
						QString sFileName = QFileInfo(lFileList[i]).fileName();
						QString sSourceFilePath = sIniFileAbsDir + "/" + sFileName;

						if (QFile(lFileList[i]).exists())
						{
							QLibrary tmpLib;
							QFile tmpFile;
							tmpFile.setFileName(lFileList[i]);
							tmpLib.setFileName(lFileList[i]);
							if (tmpLib.isLibrary(lFileList[i]))
							{
								//qDebug() << "isLibrary";
								if (tmpLib.isLoaded())
								{
									//qDebug() << "unload";
									if (tmpLib.unload()==false)
									{
										//qDebug() << "unload unsuccessful";
									}
								}
							}

							bool bRemoveSucceeded = QFile::remove(lFileList[i]);
							if (QFileInfo(lFileList[i]).exists())
								bRemoveSucceeded = false;
							if (bRemoveSucceeded == false)
							{
								qDebug() << __FUNCTION__ << "Could not replace the already present file (" << lFileList[i] << ")!";
								return InstallResult_ErrorCannotReplaceOriginalFile;
							}
						}
						//Make sure that the destination folder exists...
						if (QFileInfo(lFileList[i]).absoluteDir().exists() == false)
						{
							if (QDir().mkpath(QFileInfo(lFileList[i]).absolutePath()) == false)
							{
								qDebug() << __FUNCTION__ << "Could not create the destination directory path: " << QFileInfo(lFileList[i]).absolutePath();
								return InstallResult_ErrorCannotCreateDestinationPath;
							}
						}
						if (QFile::copy(sSourceFilePath, lFileList[i]) == false)
						{
							qDebug() << __FUNCTION__ << "Could not copy a plugin installation file (" << sSourceFilePath << ") to the plugin folder: " << lFileList[i];
							return InstallResult_ErrorCannotCopySourceFile;
						}
					}
				}
				if (bRenameSourceInstallFileOnSuccess)
					if (renameInstalledFile(sIniFilePath) == false)
						return InstallResult_ErrorRenaming;
				return InstallResult_PluginsInstalled;
			}
		}
		return InstallResult_ErrorIniFileNotFound;
	};

	static bool renameInstalledFile(const QString &sOldFileName)
	{
		QFileInfo fi(sOldFileName);
		if (fi.exists() == false)
			return false;
		return QFile(sOldFileName).rename(fi.absolutePath() + "/" + fi.baseName() + "." + fi.completeSuffix() + "_");
	};

	////////////////////Windows UAC/////////////////////
	static BOOL IsRunAsAdministrator()
	{
		BOOL fIsRunAsAdmin = FALSE;
		DWORD dwError = ERROR_SUCCESS;
		PSID pAdministratorsGroup = NULL;

		// Allocate and initialize a SID of the administrators group.
		SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
		if (!AllocateAndInitializeSid(
			&NtAuthority,
			2,
			SECURITY_BUILTIN_DOMAIN_RID,
			DOMAIN_ALIAS_RID_ADMINS,
			0, 0, 0, 0, 0, 0,
			&pAdministratorsGroup))
		{
			dwError = GetLastError();
			goto Cleanup;
		}

		// Determine whether the SID of administrators group is enabled in 
		// the primary access token of the process.
		if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
		{
			dwError = GetLastError();
			goto Cleanup;
		}

	Cleanup:
		// Centralized cleanup for all allocated resources.
		if (pAdministratorsGroup)
		{
			FreeSid(pAdministratorsGroup);
			pAdministratorsGroup = NULL;
		}

		// Throw the error if something failed in the function.
		if (ERROR_SUCCESS != dwError)
		{
			throw dwError;
		}

		return fIsRunAsAdmin;
	};

	static void ElevateNow()
	{
		BOOL bAlreadyRunningAsAdministrator = FALSE;
		try
		{
			bAlreadyRunningAsAdministrator = IsRunAsAdministrator();
		}
		catch (...)
		{
			qDebug() << "Failed to determine if application was running with administrator rights";
			//DWORD dwErrorCode = GetLastError();
			//TCHAR szMessage[256];
			//_stprintf_s(szMessage, ARRAYSIZE(szMessage), _T("Error code returned was 0x%08lx"), dwErrorCode);
			//std::cout << szMessage << std::endl;
		}
		if (!bAlreadyRunningAsAdministrator)
		{
			RestartApplication(true,true,"","-m");
		}
	};

	static bool ExecuteUpdateMngr(const bool &bAsAdmin, const QStringList &lParameters = QStringList(), const QString &sCoutFilePath = QString(""))
	{
		bool bRetval = false;
		QString sUpdateMngrFileName = QString(INSTALLMNGR_UPDATEMNGR_FILENAME) + QString(".exe");
		QFile fUpdateMngrExeFile;
		fUpdateMngrExeFile.setFileName(sUpdateMngrFileName);

		SHELLEXECUTEINFO sei = { sizeof(sei) };
		sei.hwnd = NULL;
		sei.nShow = SW_HIDE;//SW_NORMAL;
		if (bAsAdmin)
			sei.lpVerb = L"runas";
		else
			sei.lpVerb = L"open"; 

		if (fUpdateMngrExeFile.exists() == false) //Are we running inside Visual Studio?
		{
			// Check windows
			#if _WIN32 || _WIN64
			#if _WIN64
			#define ENVIRONMENT64
			#else
			#define ENVIRONMENT32
			#endif
			#endif
			// Check GCC
			#if __GNUC__
			#if __x86_64__ || __ppc64__
			#define ENVIRONMENT64
			#else
			#define ENVIRONMENT32
			#endif
			#endif
			QString sDebugMode;		//Mode
			QString sDebugModeArg;
			QString sArchitecture;	//Architecture
			QString sArchitectureArg;

			#ifdef _DEBUG
						sDebugMode = "Debug";
						sDebugModeArg = "d";
			#else
						sDebugMode = "Release";
						sDebugModeArg = "r";
			#endif
			#ifdef ENVIRONMENT32
						sArchitecture = "Win32";
						sArchitectureArg = "1";
			#else
						sArchitecture = "x64";
						sArchitectureArg = "2";
			#endif
			sUpdateMngrFileName = QString(INSTALLMNGR_UPDATEMNGR_FILENAME) + "_" + sArchitecture + "_" + sDebugMode + ".exe";
			fUpdateMngrExeFile.setFileName(sUpdateMngrFileName);
			if (fUpdateMngrExeFile.exists() == false)
				return false;

			sei.lpFile = L"cmd";
			sei.fMask = SEE_MASK_NOCLOSEPROCESS;

			QString sCurrentPath = QDir::currentPath().replace("/","\\") + "\\";
			QString sInstallMngrCommandArg = lParameters.join(" ");//ie. "-p E:\\Projects\\BrainStim\\Install\\Updates\\USBHIDDevice\\usbhiddeviceplugin_Win32_Debug\\usbhiddeviceplugin_Win32_Debug.ini install -o";
			QString sCoutCommandString = "";
			if (sCoutFilePath.isEmpty() == false)
				sCoutCommandString = " >> " + sCoutFilePath;
			QString sParameters = "/c \"" + sCurrentPath + INSTALLMNGR_UPDATEMNGR_BATCH_FILENAME + " " + sArchitectureArg + " " + sDebugModeArg + " " + QT_VERSION_STR + " E:\\Libraries\\" + " " + sCurrentPath + " && " + sCurrentPath + sUpdateMngrFileName + " " + sInstallMngrCommandArg + sCoutCommandString + "\"";
			
			sei.lpParameters = (LPCWSTR)sParameters.utf16();
			if (!ShellExecuteEx(&sei))
			{
				return false;
			}
			else
			{
				bRetval = true;
			}
			//Another example
			//QProcess process;
			//process.setEnvironment(QStringList() << "");
			//process.start("E:\\Projects\\BrainStim\\Install\\elevate.exe");//"E:\\Projects\\BrainStim\\Install\\test.bat");//, QStringList() << "E:\\Libraries\\Qt5.3.2_32bit\\5.3\\msvc2013_opengl\\bin" << "-p" << "E:\\Projects\\BrainStim\\Install\\Updates\\USBHIDDevice\\usbhiddeviceplugin_Win32_Debug\\usbhiddeviceplugin_Win32_Debug.ini install" << "-o");
			//if (process.waitForStarted())
			//{
			//	if (process.waitForFinished())
			//	{
			//		//qDebug() << __FUNCTION__ << sConfigureFileName << "output: \n" << process->readAll();
			//		while (process.waitForReadyRead())
			//		{
			//			qDebug() << __FUNCTION__ << process.readAll();
			//		}
			//	}
			//}
		}
		else
		{
			sei.lpFile = (const wchar_t*)QString(sUpdateMngrFileName).utf16();
			sei.fMask = SEE_MASK_NOCLOSEPROCESS;
			sei.lpParameters = (const wchar_t*)QString(lParameters.join(" ")).utf16();

			if (!ShellExecuteEx(&sei))
			{
				DWORD dwError = GetLastError();
				if (dwError == ERROR_CANCELLED)
				{
					// The user refused to allow privileges elevation.
					qDebug() << "User did not allow elevation for the application";
				}
			}
			else
			{
				bRetval = true;
			}
		}
		if (sei.hProcess)
		{
			// Wait until child process exits.
			WaitForSingleObject(sei.hProcess, 10000);
			CloseHandle(sei.hProcess);
		}
		return bRetval;
	};

	static bool RestartApplication(const bool &bAsAdmin, const bool &bAutoQuitSelf = true, const QString &sAppPath = "", const QString &sParameters = "")
	{
		QString sChoosenAppPath = "";
		if (sAppPath.isEmpty())
		{
			wchar_t szPath[MAX_PATH];
			if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)) == false)
				return false;
			sChoosenAppPath = QString::fromWCharArray(szPath);
		}
		else
		{
			if (sAppPath.toLower() != "cmd")
				if (QFile(sAppPath).exists() == false)
					return false;
			sChoosenAppPath = sAppPath;
		}

		// Launch it as administrator
		SHELLEXECUTEINFO sei = { sizeof(sei) };
		if (bAsAdmin)
			sei.lpVerb = L"runas";
		else
			sei.lpVerb = L"open";
		sei.lpFile = (const wchar_t*)QString(sChoosenAppPath).utf16();
		sei.hwnd = NULL;
		sei.nShow = SW_NORMAL;
		sei.lpParameters = (const wchar_t*)QString(sParameters).utf16();//"param1 param2 \"\"\"text with quotes\"\"\" param4";

		if (!ShellExecuteEx(&sei))
		{
			DWORD dwError = GetLastError();
			if (dwError == ERROR_CANCELLED)
			{
				// The user refused to allow privileges elevation.
				qDebug() << "End user did not allow elevation";
			}
		}
		else
		{
			if (bAutoQuitSelf)
				_exit(1);  // Quit itself
			return true;
		}
		return false;
	}
 };

#endif // INSTALLATIONMANAGERBASE_H
