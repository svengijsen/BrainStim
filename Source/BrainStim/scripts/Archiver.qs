var archiverObj = new Archiver();

//archiverObj.compressFile("E:/test123.zip", "E:/test.txt");
//Log(archiverObj.getFileList("E:/test123.zip"));
//archiverObj.extractDir("E:/test123.zip", "E:/");
archiverObj.compressDir("C:/Users/svengijsen/Desktop/Routes.zip", "C:/Users/svengijsen/Desktop/Routes", true);
arrFileList = archiverObj.getFileList("C:/Users/svengijsen/Desktop/Routes.zip");
for(var nIndex=0;nIndex<arrFileList.length;nIndex++)
{
	Log(nIndex + ": " + arrFileList[nIndex]);
}
archiverObj = null;




/*! /brief Compress a single file.
*
*  This function compresses a single file.
* @param sCompressedFilePath a string containing the path to the archive.
* @param sFileToCompressPath a string containing the file to compress.
* @return a Boolean value representing whether the file could be compressed.
*/
//bool compressFile(QString sCompressedFilePath, QString sFileToCompressPath);

/*! /brief Compress (multiple) file(s).
*
*  This function compresses (multiple) file(s).
* @param sCompressedFilePath a string containing the path to the archive.
* @param lFileToCompressPath a string list containing the file(s) to compress.
* @return a Boolean value representing whether the file(s) could be compressed.
*/
//bool compressFiles(QString sCompressedFilePath, QStringList lFilesToCompressPath);

/*! /brief Compress a directory.
*
*  This function compresses a directory.
* @param sCompressedFilePath a string containing the path to the archive.
* @param sDirPath a string containing the directory path to compress.
* @param bRecursive a boolean value determining whether sub-directories should also be included.
* @return a Boolean value representing whether the directory could be compressed.
*/
//bool compressDir(QString sCompressedFilePath, QString sDirPath = QString(), bool bRecursive = true);

/*! /brief Extracts a file.
*
*  This function extracts a file.
* @param sCompressedFilePath a string containing the path to the archive.
* @param sFilePath a string containing the file to extract.
* @param sFileDestPath a string containing the directory path to where the file should be extracted.
* @return a string value containing the full path of the extracted file, is empty on failure.
*/
//QString extractFile(QString sCompressedFilePath, QString sFilePath, QString sFileDestPath = QString());

/*! /brief Extracts (multiple) file(s).
*
*  This function extracts (multiple) file(s).
* @param sCompressedFilePath a string containing the path to the archive.
* @param lFilePaths a string list containing the file(s) to extract.
* @param sFileDestPath a string containing the directory path to where the file(s) should be extracted.
* @return a string list containing the full paths of the extracted file(s), is empty on failure.
*/
//QStringList extractFiles(QString sCompressedFilePath, QStringList lFilePaths, QString sFileDestPath = QString());

/*! /brief Extracts a whole archive.
*
*  This function extracts a whole archive.
* @param sCompressedFilePath a string containing the path to the archive.
* @param sFileDestPath a string containing the directory path to where the archive should be extracted.
* @return a string list containing the full paths of the extracted file(s), is empty on failure.
*/
//QStringList extractDir(QString sCompressedFilePath, QString sFileDestPath = QString());

/*! /brief Get a list of compressed files from a archive.
*
*  This function returns list of compressed files from a archive.
* @param sCompressedFilePath a string containing the path to the archive.
* @return a string list containing the full paths of compressed file(s).
*/
//QStringList getFileList(QString sCompressedFilePath);