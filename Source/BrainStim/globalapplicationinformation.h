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


#ifndef GLOBALAPPLICATIONINFORMATION_H
#define GLOBALAPPLICATIONINFORMATION_H

#include <QCoreApplication>
#include <QSettings>
#include <QStringList>
#include "maindefines.h"

//!  The GlobalApplicationInformation Class. 
/*!
  This class is internally used for storing and sharing the global application information and settings.
*/
class GlobalApplicationInformation : public QObject
{
	Q_OBJECT

public:
	GlobalApplicationInformation(QObject *parent = NULL);
	~GlobalApplicationInformation();

	typedef struct strcDocContentInfoStructure
	{
		QString strDocContent;
		QString strDocExtension;
		QString strDocHomeDir;
		bool bIsFile;
	} DocContentInfoStructure;

	typedef struct strMainAppInformation 
	{
		QString sCurrentUserName;
		QString sCompanyName;
		QString sInternalName;
		QString sFileVersion;
		bool bDoNotLoadScriptBindings;
		bool bOpenExtDebug;
		int rRendererType;//SvgView::RendererType
		bool bHQAntiAlias;
		bool bAllowMultipleInheritance;
		bool bAllowCustomUserLogins;
		bool bEnableNetworkServer;
		QString sHostAddress;
		QStringList lScriptIncludeDirectories;
		QString sUserPluginsRootDir;
		quint16 nHostPort;
		//See also serialization!!! -->  Initialize(), operator>>() and operator<<()
	} MainAppInformationStructure;

	void initialize(bool &bFirstTimeUserInitialization);

	MainAppInformationStructure getMainAppInformationStructure() {return mainAppInformation;};
	static MainAppInformationStructure getStaticMainAppInformationStructureFromSharedMemory();
	static bool copyMainAppInformationStructureToSharedMemory(const MainAppInformationStructure &mainAppInformationStructure);
	
	bool getRegisteredUserCredentials(QStringList &lUserNames, QList<QByteArray> &baPasswordHashes);
	bool setCurrentUserCredentials(const QString &sUserName, const QByteArray &baPasswordHash);
	void setCompanyName(const QString &sName);
	QString getCompanyName();
	void setFileVersionString(const QString &sVersion);
	QString getFileVersionString();
	void setInternalName(const QString &sIntName);
	QString getInternalName(); 
	QString getTitle();
	bool shouldLoadScriptBindings();
	QString currentUserName() const;
	bool addAppScriptIncludePath(const QString &sPath);
	QStringList getAppScriptIncludePaths()	{return mainAppInformation.lScriptIncludeDirectories;};
	bool shouldEnableNetworkServer();
	QString getHostAddress();
	quint16 getHostPort();

	bool initAndParseSettingFile(const QString &sSettingsFilePath, const QString &sDefaultSettingsAppDirectory = "");

	bool getSettingsInformation(const QString &sName, QVariant &vCurrentValue);
	bool setSettingsInformation(const QString &sName, const QVariant &vValue, const QString &sType);

	QVariant invokeJavaScriptConfigurationFile(const QString &sCode);
	QString getJavaScriptConfigurationFileContents();
	void showJavaScriptConfigurationFile();
	QString getConfigurationFilePath();//The ini file!

public slots:
	void parseJavaScriptConfigurationFile();

private:
	void discoverCustomPluginsPath();
	void resetInternalStructure(bool bFullSystemReset = false);
	void composeJavaScriptConfigurationFile();
	bool switchRootSettingsGroup(const QString &sNewGroup);
	//bool fetchMainAppInformationStructure();

	MainAppInformationStructure mainAppInformation;
	QSettings *AppRegistrySettings;
	QObject *webView;
	QString sCurrentSettingsFilePath;

public:
	enum ActiveScriptMode
	{
		NoScript				= 0x00000,
		Pending					= 0x00001,
		Executing				= 0x00002,
		Debugging				= 0x00003,
		Stopping				= 0x00004
	};
	Q_DECLARE_FLAGS(ActiveScriptModes, ActiveScriptMode)	

	enum ScriptRunMode
	{
		Execute					= 0x00000,
		Debug					= 0x00001,
		Abort					= 0x00002
	};
	Q_DECLARE_FLAGS(ScriptRunModes, ScriptRunMode)

	enum MainProgramModeFlag
	{
		Default						= 0,
		DisableAllScriptBindings	= 1,
		DisableAllPlugins			= 2,
		DisableSplash				= 4,
		DisableNetworkServer		= 8,
		VerboseMode					= 16,
		ExecuteDocument				= 32,
		AllowMultipleInstances		= 64,
		UserFirstTimeInit			= 128
	};
	Q_DECLARE_FLAGS(MainProgramModeFlags, MainProgramModeFlag)

	enum DocType 
	{
		DOCTYPE_UNDEFINED		= 0x00000,
		DOCTYPE_QTSCRIPT		= 0x00001,
		DOCTYPE_JAVASCRIPT		= 0x00002,
		DOCTYPE_SVG				= 0x00003,
		DOCTYPE_XML				= 0x00004,
		DOCTYPE_PYTHON			= 0x00005,
		DOCTYPE_PERL			= 0x00006,
		DOCTYPE_CPP				= 0x00007,
		DOCTYPE_HTML			= 0x00008,
		DOCTYPE_PLUGIN_DEFINED	= 0x00009
	};
	Q_DECLARE_FLAGS(DocTypes, DocType)

	enum DocTypeStyle
	{
		DOCTYPE_STYLE_UNDEFINED		= 0,
		DOCTYPE_STYLE_ECMA			= 1,
		DOCTYPE_STYLE_PLAINTEXT		= 2,
		DOCTYPE_STYLE_XML			= 3,
		DOCTYPE_STYLE_QML			= 4,
		DOCTYPE_STYLE_PYTHON		= 5,
		DOCTYPE_STYLE_PERL			= 6,
		DOCTYPE_STYLE_CPP			= 7,
		DOCTYPE_STYLE_HTML			= 8,
	};
	Q_DECLARE_FLAGS(DocTypeStyles, DocTypeStyle)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GlobalApplicationInformation::ScriptRunModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(GlobalApplicationInformation::MainProgramModeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(GlobalApplicationInformation::ActiveScriptModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(GlobalApplicationInformation::DocTypes)
Q_DECLARE_OPERATORS_FOR_FLAGS(GlobalApplicationInformation::DocTypeStyles)

QDataStream &operator<<(QDataStream &out, const GlobalApplicationInformation::MainAppInformationStructure &mainAppInformationStructure);
QDataStream &operator>>(QDataStream &in, GlobalApplicationInformation::MainAppInformationStructure &mainAppInformationStructure);

#endif // GLOBALAPPLICATIONINFORMATION_H
