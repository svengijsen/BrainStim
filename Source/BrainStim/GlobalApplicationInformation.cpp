//BrainStim
//Copyright (C) 2014  Sven Gijsen
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

#include "GlobalApplicationInformation.h"
#include "maindefines.h"
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKitWidgets/QWebPage>
#include <QResource>
#include <QBuffer>
#include <QSharedMemory>
#include <QDir>

GlobalApplicationInformation::GlobalApplicationInformation(QObject *parent)
{
	Q_UNUSED(parent);
	webView = NULL;
	Initialize();
	composeJavaScriptConfigurationFile();
	AppRegistrySettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, getCompanyName(), getTitle());//getCompanyName(),getTitle());//QSettings::IniFormat
	initAndParseRegistrySettings();
	copyMainAppInformationStructureToSharedMemory(mainAppInformation);
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

void GlobalApplicationInformation::Initialize()
{
	mainAppInformation.sCompanyName = "";
	mainAppInformation.sFileVersion = "";
	mainAppInformation.sInternalName = "";

	mainAppInformation.bDoNotLoadScriptBindings = false;
	mainAppInformation.bOpenExtDebug = false;
	mainAppInformation.rRendererType = 1;//SvgView::OpenGL;
	mainAppInformation.bHQAntiAlias = false;
	mainAppInformation.bAllowMultipleInheritance = false;
	mainAppInformation.bEnableNetworkServer = false;
	mainAppInformation.sHostAddress = "";
	mainAppInformation.nHostPort = 0;
}

QDataStream &operator<<(QDataStream &out, const GlobalApplicationInformation::MainAppInformationStructure &mainAppInformationStructure)
{
	out << mainAppInformationStructure.sCompanyName;
	out	<< mainAppInformationStructure.sInternalName;
	out	<< mainAppInformationStructure.sFileVersion;
	out << mainAppInformationStructure.bDoNotLoadScriptBindings;
	out << mainAppInformationStructure.bOpenExtDebug; 
	out << (int)mainAppInformationStructure.rRendererType;
	out << mainAppInformationStructure.bHQAntiAlias; 
	out << mainAppInformationStructure.bAllowMultipleInheritance;
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
	in >> mainAppInformationStructure.sCompanyName;
	in >> mainAppInformationStructure.sInternalName;
	in >> mainAppInformationStructure.sFileVersion;
	in >> mainAppInformationStructure.bDoNotLoadScriptBindings;
	in >> mainAppInformationStructure.bOpenExtDebug; 
	in >> nTemp;
	mainAppInformationStructure.rRendererType = nTemp;//(SvgView::RendererType)nTemp;
	in >> mainAppInformationStructure.bHQAntiAlias; 
	in >> mainAppInformationStructure.bAllowMultipleInheritance;
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

void GlobalApplicationInformation::initAndParseRegistrySettings()
{
	QString sMainAppInstallRootPath = QDir(QCoreApplication::applicationDirPath()).absolutePath();

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

bool GlobalApplicationInformation::setRegistryInformation(const QString &sName, const QVariant &vValue, const QString &sType)
{
	if(sType!="")
	{
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
		else
			return false;
		return true;
	}
	return false;
}

bool GlobalApplicationInformation::checkRegistryInformation(const QString &sName)
{
	return AppRegistrySettings->contains(sName);
}

QVariant GlobalApplicationInformation::getRegistryInformation(const QString &sName)
{
	if(AppRegistrySettings->contains(sName) == false)
		return NULL;
	return AppRegistrySettings->value(sName);	
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
	QString sPath = (":/resources/BrainStim.js");
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
	QString sPath = (":/resources/BrainStim.js");
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
