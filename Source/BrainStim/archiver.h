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


#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <QObject>
#include <QtScript>
#include <QScriptable>

#ifndef QUAZIP_STATIC
#define QUAZIP_STATIC
#endif
#include "quazip/JlCompress.h"

//!  The Archiver class. 
/*!
The Archiver can be used to compress and extract from/to a (*.zip) file.
*/

class Archiver : public QObject, protected QScriptable
{
	Q_OBJECT
		Q_CLASSINFO("ScriptAPIClassName", "Archiver");//Can't use defines here!, moc doesn't handle defines, not needed here

public:
	Archiver(QObject *parent = NULL);
	Archiver(const Archiver& other){ Q_UNUSED(other); }//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro
	~Archiver();

	static QScriptValue ctor__archiver(QScriptContext* context, QScriptEngine* engine);

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script
	/*! \brief Compress a single file.
	*
	*  This function compresses a single file.
	* @param sCompressedFilePath a string containing the path to the archive.
	* @param sFileToCompressPath a string containing the file to compress.
	* @return a Boolean value representing whether the file could be compressed.
	*/
	bool compressFile(QString sCompressedFilePath, QString sFileToCompressPath);
	/*! \brief Compress (multiple) file(s).
	*
	*  This function compresses (multiple) file(s).
	* @param sCompressedFilePath a string containing the path to the archive.
	* @param lFilesToCompressPath a string list containing the file(s) to compress.
	* @return a Boolean value representing whether the file(s) could be compressed.
	*/
	bool compressFiles(QString sCompressedFilePath, QStringList lFilesToCompressPath);
	/*! \brief Compress a directory.
	*
	*  This function compresses a directory.
	* @param sCompressedFilePath a string containing the path to the archive.
	* @param sDirPath a string containing the directory path to compress.
	* @param bRecursive a boolean value determining whether sub-directories should also be included.
	* @return a Boolean value representing whether the directory could be compressed.
	*/
	bool compressDir(QString sCompressedFilePath, QString sDirPath = QString(), bool bRecursive = true);
	/*! \brief Extracts a file.
	*
	*  This function extracts a file.
	* @param sCompressedFilePath a string containing the path to the archive.
	* @param sFilePath a string containing the file to extract.
	* @param sFileDestPath a string containing the directory path to where the file should be extracted.
	* @return a string value containing the full path of the extracted file, is empty on failure.
	*/
	QString extractFile(QString sCompressedFilePath, QString sFilePath, QString sFileDestPath = QString());
	/*! \brief Extracts (multiple) file(s).
	*
	*  This function extracts (multiple) file(s).
	* @param sCompressedFilePath a string containing the path to the archive.
	* @param lFilePaths a string list containing the file(s) to extract.
	* @param sFileDestPath a string containing the directory path to where the file(s) should be extracted.
	* @return a string list containing the full paths of the extracted file(s), is empty on failure.
	*/
	QStringList extractFiles(QString sCompressedFilePath, QStringList lFilePaths, QString sFileDestPath = QString());
	/*! \brief Extracts a whole archive.
	*
	*  This function extracts a whole archive.
	* @param sCompressedFilePath a string containing the path to the archive.
	* @param sFileDestPath a string containing the directory path to where the archive should be extracted.
	* @return a string list containing the full paths of the extracted file(s), is empty on failure.
	*/
	QStringList extractDir(QString sCompressedFilePath, QString sFileDestPath = QString());
	/*! \brief Get a list of compressed files from a archive.
	*
	*  This function returns list of compressed files from a archive.
	* @param sCompressedFilePath a string containing the path to the archive.
	* @return a string list containing the full paths of compressed file(s).
	*/
	QStringList getFileList(QString sCompressedFilePath);

private:
	QScriptEngine* currentScriptEngine;

};

Q_DECLARE_METATYPE(Archiver*)

#endif // ARCHIVER_H
