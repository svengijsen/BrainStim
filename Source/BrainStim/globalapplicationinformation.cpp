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

#include "mainappinfo.h"
#include "globalapplicationinformation.h"
#include "maindefines.h"
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKitWidgets/QWebPage>
#include <QResource>
#include <QBuffer>
#include <QSharedMemory>
#include <QDir>

GlobalApplicationInformation::GlobalApplicationInformation(QObject *parent) : webView(NULL), AppRegistrySettings(NULL)
{
	Q_UNUSED(parent);
	sCurrentSettingsFilePath = MainAppInfo::appDirPath() + "/" + MAIN_PROGRAM_INTERNAL_NAME + ".ini";
	initialize();
};

GlobalApplicationInformation::~GlobalApplicationInformation()
{
	if(AppRegistrySettings)
	{
		delete AppRegistrySettings;
		AppRegistrySettings = NULL;
	}
	if(webView)
	{
		delete webView;
		webView = NULL;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GlobalApplicationInformation::initialize()
{
	resetInternalStructure(true);
	composeJavaScriptConfigurationFile();
	initAndParseRegistrySettings(sCurrentSettingsFilePath);
	copyMainAppInformationStructureToSharedMemory(mainAppInformation);
}

void GlobalApplicationInformation::resetInternalStructure(bool bFullSystemReset)
{
	mainAppInformation.sCurrentUserName = "";
	if (bFullSystemReset)
	{
		mainAppInformation.sCompanyName = "";
		mainAppInformation.sFileVersion = "";
		mainAppInformation.sInternalName = "";
	}

	mainAppInformation.bDoNotLoadScriptBindings = false;
	mainAppInformation.bOpenExtDebug = false;
	mainAppInformation.rRendererType = 1;//SvgView::OpenGL;
	mainAppInformation.bHQAntiAlias = false;
	mainAppInformation.bAllowMultipleInheritance = false;
	mainAppInformation.bAllowCustomUserLogins = false;
	mainAppInformation.bEnableNetworkServer = false;
	mainAppInformation.sHostAddress = "";
	mainAppInformation.nHostPort = 0;
}

QDataStream &operator<<(QDataStream &out, const GlobalApplicationInformation::MainAppInformationStructure &mainAppInformationStructure)
{
	out << mainAppInformationStructure.sCurrentUserName;
	out << mainAppInformationStructure.sCompanyName;
	out	<< mainAppInformationStructure.sInternalName;
	out	<< mainAppInformationStructure.sFileVersion;
	out << mainAppInformationStructure.bDoNotLoadScriptBindings;
	out << mainAppInformationStructure.bOpenExtDebug; 
	out << (int)mainAppInformationStructure.rRendererType;
	out << mainAppInformationStructure.bHQAntiAlias; 
	out << mainAppInformationStructure.bAllowMultipleInheritance;
	out << mainAppInformationStructure.bAllowCustomUserLogins;
	out << mainAppInformationStructure.bEnableNetworkServer;
	out << mainAppInformationStructure.sHostAddress;
	out << mainAppInformationStructure.nHostPort;
	out << mainAppInformationStructure.lScriptIncludeDirectories;
	return out;
}

QDataStream &operator>>(QDataStream &in, GlobalApplicationInformation::MainAppInformationStructure &mainAppInformationStructure)
{
	//in >> title >> artist >> year;
	int nTemp;
	in >> mainAppInformationStructure.sCurrentUserName;
	in >> mainAppInformationStructure.sCompanyName;
	in >> mainAppInformationStructure.sInternalName;
	in >> mainAppInformationStructure.sFileVersion;
	in >> mainAppInformationStructure.bDoNotLoadScriptBindings;
	in >> mainAppInformationStructure.bOpenExtDebug; 
	in >> nTemp;
	mainAppInformationStructure.rRendererType = nTemp;//(SvgView::RendererType)nTemp;
	in >> mainAppInformationStructure.bHQAntiAlias; 
	in >> mainAppInformationStructure.bAllowMultipleInheritance;
	in >> mainAppInformationStructure.bAllowCustomUserLogins;
	in >> mainAppInformationStructure.bEnableNetworkServer;
	in >> mainAppInformationStructure.sHostAddress;
	in >> mainAppInformationStructure.nHostPort;
	in >> mainAppInformationStructure.lScriptIncludeDirectories;
	return in;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GlobalApplicationInformation::addAppScriptIncludePath(const QString &sPath)
{
	if(sPath.isEmpty())
		return false;
	QString sCanopPath = QDir(sPath).canonicalPath();
	if(mainAppInformation.lScriptIncludeDirectories.contains(sCanopPath) == false)
		mainAppInformation.lScriptIncludeDirectories.append(sCanopPath);
	return true;
}

void GlobalApplicationInformation::setCompanyName(const QString &sName)
{
	mainAppInformation.sCompanyName = sName;
	QCoreApplication::setOrganizationDomain(mainAppInformation.sCompanyName);
	QCoreApplication::setOrganizationName(mainAppInformation.sCompanyName);
}

QString GlobalApplicationInformation::getCompanyName()
{
	return mainAppInformation.sCompanyName;
}

void GlobalApplicationInformation::setFileVersionString(const QString &sVersion)	
{
	mainAppInformation.sFileVersion = sVersion;
	QCoreApplication::setApplicationVersion(mainAppInformation.sFileVersion);
}

QString GlobalApplicationInformation::getFileVersionString()
{
	return mainAppInformation.sFileVersion;
}

void GlobalApplicationInformation::setInternalName(const QString &sIntName)		
{
	mainAppInformation.sInternalName = sIntName;
	QCoreApplication::setApplicationName(mainAppInformation.sInternalName);
}

QString GlobalApplicationInformation::getInternalName()
{
	return mainAppInformation.sInternalName;
}

QString GlobalApplicationInformation::getTitle()
{
#ifdef WIN64
	return mainAppInformation.sInternalName +  "(v" + mainAppInformation.sFileVersion + ", 64-bits)";
#else
	return mainAppInformation.sInternalName +  "(v" + mainAppInformation.sFileVersion + ", 32-bits)";
#endif
}

bool GlobalApplicationInformation::shouldLoadScriptBindings()
{
	return !(mainAppInformation.bDoNotLoadScriptBindings);
}

QString GlobalApplicationInformation::currentUserName() const
{
	return mainAppInformation.sCurrentUserName;
}

bool GlobalApplicationInformation::shouldEnableNetworkServer()
{
	return mainAppInformation.bEnableNetworkServer;
}

QString GlobalApplicationInformation::getHostAddress()
{
	return mainAppInformation.sHostAddress;//"";//"127.0.0.1";//"137.120.137.130";//QHostAddress::Any;
}

quint16 GlobalApplicationInformation::getHostPort()
{
	return mainAppInformation.nHostPort;//200;//0=default
}


////////////////////////////////////////////////////////////////

bool GlobalApplicationInformation::switchRootSettingsGroup(const QString &sNewGroup)
{
	QString sCurrentGroupName = AppRegistrySettings->group();
	if (sCurrentGroupName.isEmpty())
	{
		AppRegistrySettings->beginGroup(sNewGroup);
		return true;
	}
	else if (sCurrentGroupName == sNewGroup)
	{
		return true;
	}
	else if ((sNewGroup == REGISTRY_SETTINGS_SYSTEM) || (sNewGroup == REGISTRY_SETTINGS_USER))
	{
		AppRegistrySettings->endGroup();
		return switchRootSettingsGroup(sNewGroup);
	}
	return false;
}

bool GlobalApplicationInformation::initAndParseRegistrySettings(const QString &sSettingsFilePath)
{
	if (AppRegistrySettings == NULL)
	{
		//AppRegistrySettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, getCompanyName(), getTitle());
		if (sSettingsFilePath.isEmpty())
			return false;
		//if (QFile(sSettingsFilePath).exists() == false)
		AppRegistrySettings = new QSettings(sSettingsFilePath, QSettings::IniFormat);
	}

	QString sMainAppInstallRootPath = QDir(QCoreApplication::applicationDirPath()).absolutePath();
	bool bUserSpecified = (mainAppInformation.sCurrentUserName.isEmpty() == false);

	switchRootSettingsGroup(REGISTRY_SETTINGS_SYSTEM);
	if (AppRegistrySettings->contains(REGISTRY_ENABLECUSTOMUSERLOGINS))
	{
		mainAppInformation.bAllowCustomUserLogins = AppRegistrySettings->value(REGISTRY_ENABLECUSTOMUSERLOGINS).toBool();
	}
	else //key doesn't exist, default value here!
	{
		mainAppInformation.bAllowCustomUserLogins = false;
		AppRegistrySettings->setValue(REGISTRY_ENABLECUSTOMUSERLOGINS, mainAppInformation.bAllowCustomUserLogins);
	}
	
	if (bUserSpecified)
	{
		switchRootSettingsGroup(REGISTRY_SETTINGS_USER);
		AppRegistrySettings->beginGroup(mainAppInformation.sCurrentUserName);
	}
	
	if (AppRegistrySettings->contains(REGISTRY_DONOTLOADSCRIPTEXTENSION)) 
	{
		mainAppInformation.bDoNotLoadScriptBindings = AppRegistrySettings->value(REGISTRY_DONOTLOADSCRIPTEXTENSION).toBool();
	}
	else //key doesn't exist, default value here!
	{
		mainAppInformation.bDoNotLoadScriptBindings = false;
		AppRegistrySettings->setValue(REGISTRY_DONOTLOADSCRIPTEXTENSION, mainAppInformation.bDoNotLoadScriptBindings);
	}

	if (AppRegistrySettings->contains(REGISTRY_ENABLENETWORKSERVER)) 
	{
		mainAppInformation.bEnableNetworkServer = AppRegistrySettings->value(REGISTRY_ENABLENETWORKSERVER).toBool();
	}
	else //key doesn't exist, default value here!
	{
		mainAppInformation.bEnableNetworkServer = false;
		AppRegistrySettings->setValue(REGISTRY_ENABLENETWORKSERVER, mainAppInformation.bEnableNetworkServer);
	}
//REGISTRY_SERVERHOSTADDRESS
//REGISTRY_SERVERHOSTPORT	
	if (AppRegistrySettings->contains(REGISTRY_SERVERHOSTADDRESS)) 
	{
		mainAppInformation.sHostAddress = AppRegistrySettings->value(REGISTRY_SERVERHOSTADDRESS).toString();
	}
	else //key doesn't exist, default value here!
	{
		mainAppInformation.sHostAddress = "";
		AppRegistrySettings->setValue(REGISTRY_SERVERHOSTADDRESS, mainAppInformation.sHostAddress);
	}

	if (AppRegistrySettings->contains(REGISTRY_SERVERHOSTPORT)) 
	{
		mainAppInformation.nHostPort = (quint16)AppRegistrySettings->value(REGISTRY_SERVERHOSTPORT).toInt();
	}
	else //key doesn't exist, default value here!
	{
		mainAppInformation.nHostPort = 0;
		AppRegistrySettings->setValue(REGISTRY_SERVERHOSTPORT, (int)mainAppInformation.nHostPort);
	}

	if (AppRegistrySettings->contains(REGISTRY_OPENINEXTERNALDEBUGGER)) 
	{
		mainAppInformation.bOpenExtDebug = AppRegistrySettings->value(REGISTRY_OPENINEXTERNALDEBUGGER).toBool();
	}
	else //key doesn't exist, default value here!
	{
		mainAppInformation.bOpenExtDebug = false;
		AppRegistrySettings->setValue(REGISTRY_OPENINEXTERNALDEBUGGER, mainAppInformation.bOpenExtDebug);
	}

	if (AppRegistrySettings->contains(REGISTRY_RENDERTYPE)) 
	{
		mainAppInformation.rRendererType = AppRegistrySettings->value(REGISTRY_RENDERTYPE).toInt();//(SvgView::RendererType)
	}
	else //key doesn't exist, default value here!
	{
		mainAppInformation.rRendererType = 1;//SvgView::OpenGL;
		AppRegistrySettings->setValue(REGISTRY_RENDERTYPE, (int)mainAppInformation.rRendererType);
	}

	if (AppRegistrySettings->contains(REGISTRY_HQANTIALIAS)) 
	{
		mainAppInformation.bHQAntiAlias = AppRegistrySettings->value(REGISTRY_HQANTIALIAS).toBool();		
	}
	else //key doesn't exist, default value here!
	{
		mainAppInformation.bHQAntiAlias = false;
		AppRegistrySettings->setValue(REGISTRY_HQANTIALIAS, mainAppInformation.bHQAntiAlias);
	}

	if (AppRegistrySettings->contains(REGISTRY_ALLOWMULTIPLEINHERITANCE)) 
	{
		mainAppInformation.bAllowMultipleInheritance = AppRegistrySettings->value(REGISTRY_ALLOWMULTIPLEINHERITANCE).toBool();		
	}
	else //key doesn't exist, default value here!
	{
		mainAppInformation.bAllowMultipleInheritance = false;
		AppRegistrySettings->setValue(REGISTRY_ALLOWMULTIPLEINHERITANCE, mainAppInformation.bAllowMultipleInheritance);
	}

	if (AppRegistrySettings->contains(REGISTRY_SCRIPTING_INCLUDEPATHS)) 
	{
		mainAppInformation.lScriptIncludeDirectories = AppRegistrySettings->value(REGISTRY_SCRIPTING_INCLUDEPATHS).toStringList();		
	}
	else //key doesn't exist, default value here!
	{
		mainAppInformation.lScriptIncludeDirectories.append(sMainAppInstallRootPath + "/" + MAIN_PROGRAM_DOC_INCLUDENAME);
		mainAppInformation.lScriptIncludeDirectories.append(sMainAppInstallRootPath + "/" + MAIN_PROGRAM_DOC_INCLUDENAME + "/" + MAIN_PROGRAM_JAVASCRIPT_DIRNAME);
		mainAppInformation.lScriptIncludeDirectories.append(sMainAppInstallRootPath + "/" + MAIN_PROGRAM_DOC_INCLUDENAME + "/" + MAIN_PROGRAM_QML_DIRNAME);
		mainAppInformation.lScriptIncludeDirectories.append(sMainAppInstallRootPath + "/" + MAIN_PROGRAM_DOC_INCLUDENAME + "/" + MAIN_PROGRAM_QTSCRIPT_DIRNAME);
		
		AppRegistrySettings->setValue(REGISTRY_SCRIPTING_INCLUDEPATHS, mainAppInformation.lScriptIncludeDirectories);
	}

	AppRegistrySettings->sync();
}

bool GlobalApplicationInformation::setSettingsInformation(const QString &sName, const QVariant &vValue, const QString &sType)
{
	bool bUserSpecified = (mainAppInformation.sCurrentUserName.isEmpty() == false);
	bool bRetval = false;
	if (bUserSpecified && (sName == REGISTRY_ENABLECUSTOMUSERLOGINS))
		switchRootSettingsGroup(REGISTRY_SETTINGS_SYSTEM);
	if(sType!="")
	{
		bRetval = true;
		if(sType.toLower()=="bool")
			AppRegistrySettings->setValue(sName, vValue.toBool());
		else if(sType.toLower()=="int")
			AppRegistrySettings->setValue(sName, vValue.toInt());
		else if(sType.toLower()=="string")
			AppRegistrySettings->setValue(sName, vValue.toString());
		else if(sType.toLower()=="stringlist")
			AppRegistrySettings->setValue(sName, vValue.toStringList());
		else if(sType.toLower()=="point")
			AppRegistrySettings->setValue(sName, vValue.toPoint());
		else if(sType.toLower()=="size")
			AppRegistrySettings->setValue(sName, vValue.toSize());
		else if (sType.toLower() == "bytearray")
			AppRegistrySettings->setValue(sName, vValue.toByteArray());
		else if (sType.toLower() == "rectf")
			AppRegistrySettings->setValue(sName, vValue.toRectF());
		else if (sType.toLower() == "rect")
			AppRegistrySettings->setValue(sName, vValue.toRect());
		else
			bRetval = false;
	}
	if (bUserSpecified && (sName == REGISTRY_ENABLECUSTOMUSERLOGINS))
	{
		switchRootSettingsGroup(REGISTRY_SETTINGS_USER);
		AppRegistrySettings->beginGroup(mainAppInformation.sCurrentUserName);
	}
	return bRetval;
}

bool GlobalApplicationInformation::getRegisteredUserCredentials(QStringList &lUserNames, QList<QByteArray> &baPasswordHashes)
{
	switchRootSettingsGroup(REGISTRY_SETTINGS_USER);
	lUserNames = AppRegistrySettings->childGroups();
	foreach(QString sCurrentUserName, lUserNames)
	{
		AppRegistrySettings->beginGroup(sCurrentUserName);
		if (AppRegistrySettings->contains(REGISTRY_USER_PASSWORDHASH))
		{
			baPasswordHashes.append(AppRegistrySettings->value(REGISTRY_USER_PASSWORDHASH).toByteArray());
		}
		else
		{
			baPasswordHashes.append(QByteArray(""));
		}
		AppRegistrySettings->endGroup();
	}
	switchRootSettingsGroup(REGISTRY_SETTINGS_SYSTEM);
	return (lUserNames.isEmpty() == false);
}

bool GlobalApplicationInformation::getSettingsInformation(const QString &sName, QVariant &vCurrentValue)
{
	bool bUserSpecified = (mainAppInformation.sCurrentUserName.isEmpty() == false);
	bool bRetval = false;
	if (sName == REGISTRY_ENABLECUSTOMUSERLOGINS)
		switchRootSettingsGroup(REGISTRY_SETTINGS_SYSTEM);
	bRetval = AppRegistrySettings->contains(sName);
	if (bRetval)
		vCurrentValue = AppRegistrySettings->value(sName);
	else
		vCurrentValue = NULL;
	if (bUserSpecified && (sName == REGISTRY_ENABLECUSTOMUSERLOGINS))
	{
		switchRootSettingsGroup(REGISTRY_SETTINGS_USER);
		AppRegistrySettings->beginGroup(mainAppInformation.sCurrentUserName);
	}
	return bRetval;
}

/////////////////////////////////////////////////////////////////////////////////////////

void GlobalApplicationInformation::parseJavaScriptConfigurationFile()
{
	setCompanyName(invokeJavaScriptConfigurationFile("BrainStimInfo.GetMainAppCompanyName()").toString());
	setInternalName(invokeJavaScriptConfigurationFile("BrainStimInfo.GetMainAppInternalName()").toString());
	setFileVersionString(invokeJavaScriptConfigurationFile("BrainStimInfo.GetCurrentRelease()").toString());
}

void GlobalApplicationInformation::composeJavaScriptConfigurationFile()
{
	bool bConnected = false;
	QWebView *webViewCast = qobject_cast<QWebView *>(webView);
	if(webViewCast == NULL)
	{
		webView = new QWebView();
		webViewCast = qobject_cast<QWebView *>(webView);
		webViewCast->setObjectName(QString::fromUtf8("webView"));
		bConnected = connect(webViewCast, SIGNAL(loadFinished(bool)), SLOT(parseJavaScriptConfigurationFile()),Qt::DirectConnection);
	}
	QString sPath = (":/resources/brainstim.js");
	QResource res(sPath);
	QByteArray bResourceData;
	bResourceData=reinterpret_cast< const char* >( res.data() ), res.size();
	webViewCast->page()->mainFrame()->evaluateJavaScript(bResourceData);
	sPath = (":/resources/versioning.html");
	res.setFileName(sPath);
	bResourceData = reinterpret_cast< const char* >( res.data() ), res.size();
	webViewCast->setContent(bResourceData);//Invokes the loadFinished() signal after loading
}

QString GlobalApplicationInformation::getJavaScriptConfigurationFileContents()
{
	QString sPath = (":/resources/brainstim.js");
	QResource tmpRes;
	tmpRes.setFileName(sPath);
	QByteArray baResourceData = reinterpret_cast< const char* >( tmpRes.data());
	return baResourceData;
}

QVariant GlobalApplicationInformation::invokeJavaScriptConfigurationFile(const QString &sCode)
{
	if(webView)
		return qobject_cast<QWebView *>(webView)->page()->mainFrame()->evaluateJavaScript(sCode);
	return NULL;
}

void GlobalApplicationInformation::showJavaScriptConfigurationFile()
{
	if(webView)
	{
		qobject_cast<QWebView *>(webView)->show();
	}
}

bool GlobalApplicationInformation::setCurrentUserCredentials(const QString &sUserName, const QByteArray &baPasswordHash)
{
	if (mainAppInformation.sCurrentUserName != sUserName)
	{
		resetInternalStructure(false);
		mainAppInformation.sCurrentUserName = sUserName;
		initAndParseRegistrySettings(sCurrentSettingsFilePath);
		if (sUserName.isEmpty()==false)
		{
			AppRegistrySettings->setValue(REGISTRY_USER_PASSWORDHASH, baPasswordHash);
		}
		copyMainAppInformationStructureToSharedMemory(mainAppInformation);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GlobalApplicationInformation::MainAppInformationStructure GlobalApplicationInformation::getStaticMainAppInformationStructureFromSharedMemory()
{
	MainAppInformationStructure tmpAppInfoStruct;
	QSharedMemory sharedMemory;
	sharedMemory.setKey(MAIN_PROGRAM_SHARED_MEM_KEY);//Sets a new key for this shared memory object. If key and the current key are the same, the function returns without doing anything.
	if (sharedMemory.attach(QSharedMemory::ReadOnly))//ReadWrite))//Attempts to attach the process to the shared memory segment identified by the key that was passed to the constructor.
	{
		QBuffer buffer;
		QDataStream inStream(&buffer);
		sharedMemory.lock();
		buffer.setData((char*)sharedMemory.constData(), sharedMemory.size());
		buffer.open(QBuffer::ReadOnly);
		inStream >> tmpAppInfoStruct;
		sharedMemory.unlock();
		return tmpAppInfoStruct;
	}
	return tmpAppInfoStruct;
}

//bool GlobalApplicationInformation::fetchMainAppInformationStructure()
//{
//	QSharedMemory sharedMemory;
//	sharedMemory.setKey(MAIN_PROGRAM_SHARED_MEM_KEY);//Sets a new key for this shared memory object. If key and the current key are the same, the function returns without doing anything.
//	if (sharedMemory.attach(QSharedMemory::ReadOnly))//ReadWrite))//Attempts to attach the process to the shared memory segment identified by the key that was passed to the constructor.
//	{
//		QBuffer buffer;
//		QDataStream inStream(&buffer);
//		sharedMemory.lock();
//		buffer.setData((char*)sharedMemory.constData(), sharedMemory.size());
//		buffer.open(QBuffer::ReadOnly);
//		inStream >> mainAppInformation;
//		sharedMemory.unlock();
//		return true;
//	}
//	return false;
//}

bool GlobalApplicationInformation::copyMainAppInformationStructureToSharedMemory(const MainAppInformationStructure &mainAppInformationStructure)
{
	QSharedMemory sharedMemory;
	sharedMemory.setKey(MAIN_PROGRAM_SHARED_MEM_KEY);//Sets a new key for this shared memory object. If key and the current key are the same, the function returns without doing anything.
	if (sharedMemory.attach(QSharedMemory::ReadWrite))//Attempts to attach the process to the shared memory segment identified by the key that was passed to the constructor.
	{
		QBuffer buffer;
		buffer.open(QBuffer::ReadWrite);
		QDataStream outStream(&buffer);
		outStream << mainAppInformationStructure;
		int nSize = buffer.size();

		sharedMemory.lock();
		char *cDestination = (char*)sharedMemory.data();
		const char *cSource = buffer.data().data();
		memcpy(cDestination, cSource, qMin(sharedMemory.size(), nSize));
		sharedMemory.unlock();
		return true;
	}
	return false;
}
