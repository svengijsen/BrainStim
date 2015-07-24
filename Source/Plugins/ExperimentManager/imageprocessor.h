//ExperimentManagerplugin
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


#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QtScript>
#include <QScriptable>
#include <QColor>
#include <QPixmap>

//!  The ImageProcessor class. 
/*!
  The Image Processor can be used to edit Png (*.png) and custom Dat (*.dat) files.  
*/
class ImageProcessor : public QObject, protected QScriptable
{
	Q_OBJECT
	Q_CLASSINFO("ScriptAPIClassName", "ImageProcessor");//Can't use defines here!, moc doesn't handle defines, not needed here

public:

	 /*! The enum ScalingMethod holds a Scaling Method for the ImageProcessor object */
	enum ScalingMethod
	{	
		ScalingMethod_MonoColored							= 0, /*!< default, Mono Colored. */
		ScalingMethod_SmoothGreyColored						= 1, /*!< Smoothed (Grey value Colored). */
		ScalingMethod_SmoothMonoColored						= 2, /*!< Smoothed and converted to a Mono Color values, threshold value = 255. */
		ScalingMethod_SmoothMonoColoredCustomThreshold		= 3  /*!< Smoothed and converted to a Mono Color values, custom threshold value. */
	};

	ImageProcessor(QObject *parent = NULL);
	ImageProcessor(const ImageProcessor& other ){Q_UNUSED(other);}//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro
	~ImageProcessor();

	static QScriptValue ctor__imageProcessor(QScriptContext* context, QScriptEngine* engine);

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script
	
	/*! \brief Converts and saves a Png (*.png) file to the custom Dat (*.dat) file format.
	*
	*  This function Converts and saves a Png (*.png) file to the custom Dat (*.dat) file format.
	* @param strSource a String containing the path to the source Png (*.png) file.
	* @param strDestination a String containing the path to destination Dat (*.dat) file.
	* @param bOverwrite a Boolean value determing whether the destination file should be automatically overwritten when it already exists.
	* @return a Boolean value representing whether the target file could be saved.
	*/
	bool ConvertPngToDatFile(QString strSource, QString strDestination, bool bOverwrite = false);
	/*! \brief Converts and saves a custom Dat (*.dat) file to a Png (*.png) file format.
	*
	*  This function Converts and saves a custom Dat (*.dat) file to a Png (*.png) file format.
	* @param strSource a String containing the path to the source Dat (*.dat) file.
	* @param strDestination a String containing the path to destination Png (*.png) file.
	* @param bOverwrite a Boolean value determing whether the destination file should be automatically overwritten when it already exists.
	* @return a Boolean value representing whether the target file could be saved.
	*/	
	bool ConvertDatToPngFile(QString strSource, QString strDestination, bool bOverwrite = false);
	/*! \brief Scales a a Png (*.png) file by a specified Ratio.
	*
	*  This function Scales and saves a Png (*.png) file to a specified destination.
	* @param strSource a String containing the path to the source Png (*.png) file.
	* @param strDestination a String containing the path to destination Png (*.png) file.
	* @param nRatio an integer value containing Ratio to which the image should be scaled.
	* @param nMethod an integer value containing the Scaling Method, see ImageProcessor::ScalingMethod.
	* @param nColorThreshold an integer value containing the Threshold value in case the defined Scaling Method is #ScalingMethod_SmoothMonoColoredCustomThreshold.
	* @param bOverwrite a Boolean value determining whether the destination file should be automatically overwritten when it already exists.
	* @param bSmoothFiltering a Boolean value determining whether a the resulting image is transformed using bilinear filtering.
	* @return a Boolean value representing whether the target file could be saved.
	*/	
	bool ScalePngFile(QString strSource, QString strDestination, int nRatio = 1, int nMethod = 0, int nColorThreshold = 255, bool bOverwrite = false, bool bSmoothFiltering = false);
	/*! \brief Scales a a Png (*.png) file by a specified Size.
	*
	*  This function Scales and saves a Png (*.png) file to a specified destination.
	* @param strSource a String containing the path to the source Png (*.png) file.
	* @param strDestination a String containing the path to destination Png (*.png) file.
	* @param nXPixels an integer value containing the number of pixels (width) to which the image should be scaled.
	* @param nYPixels an integer value containing the number of pixels (height) to which the image should be scaled.
	* @param nMethod an integer value containing the Scaling Method, see ImageProcessor::ScalingMethod.
	* @param nColorThreshold an integer value containing the Threshold value in case the defined Scaling Method is #ScalingMethod_SmoothMonoColoredCustomThreshold.
	* @param bOverwrite a Boolean value determining whether the destination file should be automatically overwritten when it already exists.
	* @param bSmoothFiltering a Boolean value determining whether a the resulting image is transformed using bilinear filtering.
	* @return a Boolean value representing whether the target file could be saved.
	*/	
	bool ScalePngFileBySize(QString strSource, QString strDestination, int nXPixels, int nYPixels, int nMethod = 0, int nColorThreshold = 255, bool bOverwrite = false, bool bSmoothFiltering = false);
	/*! \brief Creates a mean Png image (*.png) from one or more Png (*.png) images.
	*
	*  This function Creates a mean Png image (*.png) from one or more Png (*.png) images specified by a source list and destination directory path.
	* @param sourceImagePaths a StringList containing the path(s) to the source Png (*.png) file(s).
	* @param destinationPath a String containing the path to destination mean Image Png (*.png) file.
	* @param bOverwrite a boolean value determining whether the destination file should be automatically overwritten when it already exists.
	* @return a Boolean value representing whether the mean image destination file could be saved.
	*/	
	bool CreateMeanImageFromPngFiles(const QStringList &sourceImagePaths,const QString &destinationPath, const bool bOverwrite = false);
	/*! \brief Concatenates one or more DAT image(s) (*.dat) to one CDAT (*.cdat) file.
	*
	*  This function Concatenates one or more DAT (*.dat) image file(s) to one CDAT (*.cdat) file specified by a source list and destination directory path.
	* @param sourceImagePaths a StringList containing the path(s) to the source DAT (*.dat) file(s).
	* @param destinationPath a String containing the path to destination concatenated CDAT (*.cdat) file.
	* @param bOverwrite a boolean value determining whether the destination file should be automatically overwritten when it already exists.
	* @return a Boolean value representing whether the concatenated destination file could be saved.
	*/		
	bool ConcatenateDatFiles(const QStringList &sourceImagePaths,const QString &destinationPath, const bool bOverwrite = false);
	/*! \brief Splits a CDAT (*.cdat) file into one or more DAT (*.dat) image file(s).
	*
	*  This function Splits a CDAT (*.cdat) file into one or more DAT (*.dat) image file(s), specified by a source and destination directory path.
	* @param sourceImagePath a String containing the path to the source CDAT (*.cdat) file.
	* @param destinationPath a String containing the path to destination directory where the DAT (*.dat) file(s) are saved.
	* @param destPreFileName the prefix used for saving the split DAT (*.dat) file(s) --> "<destPreFileName>_<0 based DAT file index>.dat", e.g. studyxyz_3.dat.
	* @param bOverwrite a boolean value determining whether the destination file(s) should be automatically overwritten when it already exists.
	* @return a Boolean value representing whether the split image file(s) could be saved to the destination directory.
	*/	
	int SplitCDatFile(const QString &sourceImagePath,const QString &destinationPath, const QString &destPreFileName, const bool bOverwrite = false);	
	/*! \brief Returns a specified QImage from a CDAT (*.cdat) file.
	*
	*  This function Returns a specified QImage from a CDAT (*.cdat) file, specified by a source CDAT (*.cdat) file path and a image index (0 based).
	* @param sourcePath a String containing the path to the source CDAT (*.cdat) file.
	* @param nIndex a integer containing the index from the requested image in the source CDAT (*.cdat) file.
	* @return a QImage object that contains the requested image data, if this function could not execute properly it return an QImage object with size(w=0,h=0).
	*/	
	QImage getImageFromCDATFile(const QString &sourcePath, const int &nIndex = 0);
	/*! \brief Returns the amount of sub-images a specified CDAT (*.cdat) file contains.
	*
	*  This function Returns the amount of sub-images a specified CDAT (*.cdat) file contains, specified by a source CDAT (*.cdat) file path.
	* @param sourcePath a String containing the path to the source CDAT (*.cdat) file.
	* @return a integer containing the number of sub-images, if this function could not execute properly it returns -1.
	*/			
	int getCDATFileNrOfSubFrames(const QString &sourcePath);

private:
	QScriptEngine* currentScriptEngine;
	
};

Q_DECLARE_METATYPE(ImageProcessor*)

#endif // IMAGEPROCESSOR_H
