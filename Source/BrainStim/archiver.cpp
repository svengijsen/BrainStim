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


#include "archiver.h"

QScriptValue Archiver::ctor__archiver(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new Archiver(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

/*! \brief The Archiver constructor.
*
*   You do not need to specify the parent object.
*	The BrainStim script engine automatically retrieves the parent role
*/
Archiver::Archiver(QObject *parent) : QObject(parent)
{
	currentScriptEngine = NULL;
}

/*! \brief The Archiver destructor.
*
*   You do not need call the destructor.
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
Archiver::~Archiver()
{

}

bool Archiver::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		//QObject *someObject = this;//new MyObject;
		QScriptValue objectValue = currentScriptEngine->newQObject(this);
		currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		return true;
	}
	return false;
}

bool Archiver::compressFile(QString sCompressedFilePath, QString sFileToCompressPath)
{
	return JlCompress::compressFile(sCompressedFilePath, sFileToCompressPath);
}

bool Archiver::compressFiles(QString sCompressedFilePath, QStringList lFilesToCompressPath)
{
	return JlCompress::compressFiles(sCompressedFilePath, lFilesToCompressPath);
}

bool Archiver::compressDir(QString sCompressedFilePath, QString sDirPath, bool bRecursive)
{
	return JlCompress::compressDir(sCompressedFilePath, sDirPath, bRecursive);
}

QString Archiver::extractFile(QString sCompressedFilePath, QString sFilePath, QString sFileDestPath)
{
	return JlCompress::extractFile(sCompressedFilePath, sFilePath, sFileDestPath);
}

QStringList Archiver::extractFiles(QString sCompressedFilePath, QStringList lFilePaths, QString sFileDestPath)
{
	return JlCompress::extractFiles(sCompressedFilePath, lFilePaths, sFileDestPath);
}

QStringList Archiver::extractDir(QString sCompressedFilePath, QString sFileDestPath)
{
	return JlCompress::extractDir(sCompressedFilePath, sFileDestPath);
}

QStringList Archiver::getFileList(QString sCompressedFilePath)
{
	return JlCompress::getFileList(sCompressedFilePath);
}

