//ExperimentManagerplugin
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


#include "ImageProcessor.h"
#include <QImage>
#include <QPixmap>
#include "Global.h"
#include <QtOpenGL>
#include <qgl.h>

QScriptValue ImageProcessor::ctor__imageProcessor(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new ImageProcessor(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
} 

/*! \brief The ImageProcessor constructor.
*
*   You do not need to specify the parent object. 
*	The BrainStim script engine automatically retrieves the parent role
*/
ImageProcessor::ImageProcessor(QObject *parent)	: QObject(parent)
{
	currentScriptEngine = NULL;
}

/*! \brief The ImageProcessor destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
ImageProcessor::~ImageProcessor()
{

}

bool ImageProcessor::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
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

bool ImageProcessor::ConvertPngToDatFile(QString strSource, QString strDestination, bool bOverwrite)
{
	QFile fileSource(strSource);
	QFile fileDest(strDestination);
	QFileInfo fileDestInfo(strDestination);
	if (!fileSource.exists())
		return false;
	if (fileDest.exists() && (bOverwrite == false))
		return false;
	if (!fileDestInfo.absoluteDir().exists())
	{
		if (!QDir().mkdir(fileDestInfo.absolutePath()))
			return false;
	}
	QPixmap tmpPixmap;
	if(!tmpPixmap.load(strSource,"PNG"))
		return false;
	if (fileDest.open(QIODevice::WriteOnly))
	{
		QImage imgWrite = tmpPixmap.toImage().convertToFormat(QImage::Format_ARGB32);
		QDataStream output(&fileDest);
		quint32 magic = ARGB_FORMAT_HEADER;
		quint32 width = imgWrite.width();
		quint32 height = imgWrite.height();
		output << magic << width << height;
		for (quint32 y = 0; y < height; ++y) 
		{
			QRgb *scanLine = (QRgb *)imgWrite.scanLine(y);
			for (quint32 x = 0; x < width; ++x)
			{
				output << scanLine[x];
			}
		}
		fileDest.close();
		return true;
	}
	return false;
}

bool ImageProcessor::ConvertDatToPngFile(QString strSource, QString strDestination, bool bOverwrite)
{
	QFile fileSource(strSource);
	QFile fileDest(strDestination);
	QFileInfo fileDestInfo(strDestination);
	if (!fileSource.exists())
		return false;
	if (fileDest.exists() && (bOverwrite == false))
		return false;
	if (!fileDestInfo.absoluteDir().exists())
	{
		if (!QDir().mkdir(fileDestInfo.absolutePath()))
			return false;
	}
	if(fileSource.open(QIODevice::ReadOnly))
	{
		quint32 magic;
		quint32 width;
		quint32 height;
		quint32 x,y;
		bool bSaveResult = false;
		QRgb *scanLine;
		
		QDataStream input(&fileSource);
		//int nBytesRead = input.readRawData(magic,sizeof(magic));
		input >> magic >> width >> height;
		if(magic == ARGB_FORMAT_HEADER)
		{
            //int nDataSize = width*height*sizeof(QRgb);
			QImage tmpImage(width,height,QImage::Format_ARGB32);
			for (y = 0; y < height; ++y) 
			{
				scanLine = (QRgb *)tmpImage.scanLine(y);
				for (x = 0; x < width; ++x)
				{
					input >> scanLine[x];
				}
			}
			bSaveResult = tmpImage.save(strDestination,"PNG");
			fileSource.close();
			return bSaveResult;
		}
		fileSource.close();
		return false;
	}
	return false;
}

bool ImageProcessor::ScalePngFile(QString strSource, QString strDestination, int nRatio, int nMethod, int nColorThreshold, bool bOverwrite, bool bSmoothFiltering)
{
	if (nRatio <= 0)
		return false;
	QFile fileSource(strSource);
	if (!fileSource.exists())
		return false;
	QPixmap tmpPixmap;
	if(!tmpPixmap.load(strSource,"PNG"))
		return false;
	QSize newSize = tmpPixmap.size()/nRatio;
	return ScalePngFileBySize(strSource,strDestination,newSize.width(),newSize.height(),nMethod,nColorThreshold,bOverwrite,bSmoothFiltering);
}

bool ImageProcessor::ScalePngFileBySize(QString strSource, QString strDestination, int nXPixels, int nYPixels, int nMethod, int nColorThreshold, bool bOverwrite, bool bSmoothFiltering)
{
	if ((nXPixels <= 0)||(nYPixels <= 0))
		return false;
	bool bSaveResult = false;
	QFile fileSource(strSource);
	QFile fileDest(strDestination);
	QFileInfo fileDestInfo(strDestination);
	if (!fileSource.exists())
		return false;
	if (fileDest.exists() && (bOverwrite == false))
		return false;
	if (!fileDestInfo.absoluteDir().exists())
	{
		if (!QDir().mkdir(fileDestInfo.absolutePath()))
			return false;
	}
	QPixmap tmpPixmap;
	if(!tmpPixmap.load(strSource,"PNG"))
		return false;
	QSize newSize(nXPixels,nYPixels);
	Qt::TransformationMode filterMode;
	if(bSmoothFiltering)
		filterMode = Qt::SmoothTransformation;//The resulting image is transformed using bilinear filtering.
	else
		filterMode = Qt::FastTransformation;//The transformation is performed quickly, with no smoothing.
	switch ((ScalingMethod)nMethod)
	{
	case ScalingMethod_MonoColored://default (Mono colored)
		{
			tmpPixmap = tmpPixmap.scaled(newSize,Qt::KeepAspectRatio,filterMode);
			break;
		}
	case ScalingMethod_SmoothGreyColored://Smooth (Grey colored)
		{
			tmpPixmap = tmpPixmap.scaled(newSize,Qt::KeepAspectRatio,filterMode);
			break;
		}
	case ScalingMethod_SmoothMonoColored://Smooth + Convert to Mono colored (threshold = 255)
		{
			tmpPixmap = tmpPixmap.scaled(newSize,Qt::KeepAspectRatio,filterMode);
			QImage tmpImage = tmpPixmap.toImage().convertToFormat(QImage::Format_Mono, Qt::MonoOnly|Qt::ThresholdDither|Qt::AvoidDither);
			tmpPixmap = tmpPixmap.fromImage(tmpImage);
			break;	
		}
	case ScalingMethod_SmoothMonoColoredCustomThreshold://Smooth + Convert to Mono colored (threshold = nColorThreshold)
		{
			tmpPixmap = tmpPixmap.scaled(newSize,Qt::KeepAspectRatio,filterMode);
			QImage tmpImage = tmpPixmap.toImage();//.convertToFormat(QImage::Format_Mono, Qt::MonoOnly|Qt::ThresholdDither|Qt::AvoidDither);
			QImage resultImage = tmpImage;
			resultImage.fill(255);
			for(int x=0; x<tmpImage.width(); x++)
				for(int y=0; y<tmpImage.height(); y++)
					resultImage.setPixel(x, y, qGray(tmpImage.pixel(x, y))>=nColorThreshold?qRgb(255, 255, 255):qRgb(0, 0, 0));
			tmpPixmap = tmpPixmap.fromImage(resultImage);
			//nColorThreshold
			//*and the following code is for converting images to monochrome manually by specifying threshold:*
			//
			//	QImage result = pImage;
			//result.fill(255);
			//for(int x=0; x<pImage.width(); x++)
			//	for(int y=0; y<pImage.height(); y++)
			//		result.setPixel(x, y, qGray(pImage.pixel(x, y))>threshold?qRgb(255, 255, 255):qRgb(0, 0, 0));

			//return result;

			//*The following code for converting images to monochrome by using automatic guessing for threshold:*
			//
			//	QImage result = pImage;
			//QList<int> pixelsValues;
			//for(int x=0; x<pImage.width();x++)
			//{
			//	for(int y=0; y<pImage.height();y++)
			//	{
			//		QRgb rgb = pImage.pixel(x,y);
			//		QColor color(rgb);
			//		pixelsValues<<color.lightness();
			//	}
			//}
			//qSort(pixelsValues);
			//int median2 = (pixelsValues.first() + pixelsValues.last())/2;
			//int threshold = median2;
			//for(int x=0; x<pImage.width(); x++)
			//	for(int y=0; y<pImage.height(); y++)
			//		result.setPixel(x, y, qGray(pImage.pixel(x, y))>threshold?qRgb(255, 255, 255):qRgb(0, 0, 0));
			//qDebug("threshold = %d", threshold);
			//return result;

			//*and the following code is for converting images to monochrome manually by specifying threshold:*
			//
			//	QImage result = pImage;
			//result.fill(255);
			//for(int x=0; x<pImage.width(); x++)
			//	for(int y=0; y<pImage.height(); y++)
			//		result.setPixel(x, y, qGray(pImage.pixel(x, y))>threshold?qRgb(255, 255, 255):qRgb(0, 0, 0));

			//return result;

		}
	}
	bSaveResult = tmpPixmap.save(strDestination,"PNG");
	return bSaveResult;
}

bool ImageProcessor::ConcatenateDatFiles(const QStringList &sourceImagePaths,const QString &destinationPath, const bool bOverwrite)
{
	if(sourceImagePaths.isEmpty())
		return false;
	QFile fileSource;
	//Do all sources exist?
	for (int i=0;i<sourceImagePaths.count();i++)
	{		
		fileSource.setFileName(sourceImagePaths.at(i));
		if (!fileSource.exists())
			return false;
	}
	QFile fileDest(destinationPath);
	//Check if we can write to the destination
	QFileInfo fileDestInfo(destinationPath);
	if (fileDest.exists() && (bOverwrite == false))
		return false;
	if (!fileDestInfo.absoluteDir().exists())
	{
		if (!QDir().mkdir(fileDestInfo.absolutePath()))
			return false;
	}
	//Let's write the data to the *.cdat file
	if (fileDest.open(QIODevice::WriteOnly))
	{
		QDataStream output(&fileDest);
		quint32 magic = ARGB_FORMAT_HEADER_CONC;
		quint32 nNumberOfFrames = sourceImagePaths.count();
		output << magic << nNumberOfFrames;
		bool bBreakWriteOperation = false;
		for (int i=0;i<sourceImagePaths.count();i++)
		{		
			fileSource.setFileName(sourceImagePaths.at(i));
			if(fileSource.open(QIODevice::ReadOnly))
			{
				quint32 magic;
				quint32 width;
				quint32 height;
				char *readData;
				QDataStream input(&fileSource);
				input >> magic >> width >> height;
				if(magic == ARGB_FORMAT_HEADER)
				{
					int nDataSize = width*height*sizeof(QRgb);
					readData = new char[nDataSize];
					//int nBytesRead = 
						input.readRawData(readData,nDataSize);
					if(i==0)
						output << width << height;
					//int nBytesWritten = 
						output.writeRawData(readData,nDataSize);
					delete[] readData;
				}
				fileSource.close();
			}
			else
			{
				bBreakWriteOperation = true;
				break;
			}
		}
		if(bBreakWriteOperation)
		{
			fileDest.close();
			return false;
		}
		fileDest.close();
		return true;
	}
	return false;
}

int ImageProcessor::getCDATFileNrOfSubFrames(const QString &sourcePath)
{
	if(sourcePath.isEmpty())
		return -1;
	QFile fileSource;
	fileSource.setFileName(sourcePath);
	if (!fileSource.exists())
		return -1;
	QFileInfo fileSourceInfo(sourcePath);
	if ((fileSourceInfo.suffix() == "cdat") || (fileSourceInfo.suffix() == "dat"))
	{
		if(fileSource.open(QIODevice::ReadOnly))
		{
			quint32 img_magic;
			quint32 img_width;
			quint32 img_height;
			quint32 nNumberOfFrames = 0;
			QDataStream input(&fileSource);
			if(fileSourceInfo.suffix() == "cdat")
			{
				input >> img_magic >> nNumberOfFrames >> img_width >> img_height;
				fileSource.close();
				return nNumberOfFrames;
			}
			fileSource.close();
			return 1;
		}
	}
	return -1;
}

QImage ImageProcessor::getImageFromCDATFile(const QString &sourcePath, const int &nIndex)
{
	if(sourcePath.isEmpty())
		return QImage(0,0,QImage::Format_ARGB32);
	QFile fileSource;
	fileSource.setFileName(sourcePath);
	if (!fileSource.exists())
		return QImage(0,0,QImage::Format_ARGB32);
	QFileInfo fileSourceInfo(sourcePath);
	if (fileSourceInfo.suffix() == "dat")
	{
		if(fileSource.open(QIODevice::ReadOnly))
		{
			quint32 img_magic;
			quint32 img_width;
			quint32 img_height;
			char *readData;
			QDataStream input(&fileSource);
			input >> img_magic >> img_width >> img_height;
			if(img_magic == ARGB_FORMAT_HEADER)
			{
				int nDataSize = img_width*img_height*sizeof(QRgb);
				readData = new char[nDataSize];
				int nBytesRead = input.readRawData(readData,nDataSize);				
				if(nBytesRead>0)
				{
					QImage tmpImage((uchar*)readData,(int)img_width, (int)img_height,QImage::Format_ARGB32);
					fileSource.close();
					return tmpImage;
				}	
			}
			fileSource.close();
		}
	}
	else if (fileSourceInfo.suffix() == "cdat")
	{
		if(fileSource.open(QIODevice::ReadOnly))
		{
			quint32 img_magic;
			quint32 img_width;
			quint32 img_height;
			quint32 nNumberOfFrames = 0;
			char *readData;
			QDataStream input(&fileSource);
			input >> img_magic >> nNumberOfFrames >> img_width >> img_height;
			if(nIndex < (int)nNumberOfFrames)
			{
				if(img_magic == ARGB_FORMAT_HEADER_CONC)
				{
					int nDataSize = img_width*img_height*sizeof(QRgb);
					readData = new char[nDataSize];
					input.skipRawData(nIndex * nDataSize);
					int nBytesRead = input.readRawData(readData,nDataSize);				
					if(nBytesRead>0)
					{
						QImage tmpImage((uchar*)readData,(int)img_width, (int)img_height,QImage::Format_ARGB32);
						fileSource.close();
						return tmpImage;
					}	
				}
			}
			fileSource.close();
		}

	}
	return QImage(0,0,QImage::Format_ARGB32);
}

int ImageProcessor::SplitCDatFile(const QString &sourceImagePath,const QString &destinationPath, const QString &destPreFileName, const bool bOverwrite)
{
	if(sourceImagePath.isEmpty())
		return -1;
	QFile fileSource;
	fileSource.setFileName(sourceImagePath);
	if (!fileSource.exists())
		return -1;
	//Create the destination directory
	QFileInfo fileDestInfo(destinationPath);
	if (!fileDestInfo.absoluteDir().exists())
	{
		if (!QDir().mkdir(fileDestInfo.absolutePath()))
			return -1;
	}
	if (fileSource.open(QIODevice::ReadOnly))
	{
		bool bBreakWriteOperation = false;
		quint32 magic_conc;
		quint32 magic = ARGB_FORMAT_HEADER;
		quint32 width;
		quint32 height;
		quint32 nNumberOfFrames = 0;
		QDataStream input(&fileSource);
		input >> magic_conc >> nNumberOfFrames >> width >> height;
		if(magic_conc == ARGB_FORMAT_HEADER_CONC)
		{
			int nDestFileCount = nNumberOfFrames;
			QFile fileDest;
			if(nDestFileCount > 0)
			{
				//Check if we can write to the destination
				if(bOverwrite == false)
				{
					for (int i=0;i<nDestFileCount;i++)
					{		
						fileDest.setFileName(destinationPath + destPreFileName + "_" + i + ".dat");
						if (fileDest.exists())
							return -1;
					}
				}
				int nDataSize = width*height*sizeof(QRgb);
				char *readData;				
				QString tmpFileStr = "";
				for (int i=0;i<(int)nNumberOfFrames;i++)
				{
					tmpFileStr = destinationPath + destPreFileName + "_" + QString::number(i) + ".dat";
					fileDest.setFileName(tmpFileStr);
					if(fileDest.open(QIODevice::WriteOnly))
					{
						QDataStream output(&fileDest);
						readData = new char[nDataSize];
						output << magic << width << height;
						//int nBytesRead = 
							input.readRawData(readData,nDataSize);
						//int nBytesWritten = 
							output.writeRawData(readData,nDataSize);
						fileDest.close();
						delete[] readData;
					}
					else
					{
						bBreakWriteOperation = true;
						break;
					}
				}
			}
		}
		else
		{
			bBreakWriteOperation = true;
		}
		fileSource.close();
		if(bBreakWriteOperation)
			return -1;
		return nNumberOfFrames;
	}
	return -1;
}

bool ImageProcessor::CreateMeanImageFromPngFiles(const QStringList &sourceImagePaths,const QString &destinationPath, const bool bOverwrite)
{
	if(sourceImagePaths.isEmpty())
		return false;
	QFile fileSource;
	for (int i=0;i<sourceImagePaths.count();i++)
	{		
		fileSource.setFileName(sourceImagePaths.at(i));
		if (!fileSource.exists())
			return false;
	}
	QFile fileDest(destinationPath);
	QFileInfo fileDestInfo(destinationPath);
	if (fileDest.exists() && (bOverwrite == false))
		return false;
	if (!fileDestInfo.absoluteDir().exists())
	{
		if (!QDir().mkdir(fileDestInfo.absolutePath()))
			return false;
	}
	QPixmap tmpPixmap;
	QList<QImage> lstSourceImages;
	for (int i=0;i<sourceImagePaths.count();i++)
	{	
		if(!tmpPixmap.load(sourceImagePaths.at(i),"PNG"))
			return false;
		lstSourceImages.append(tmpPixmap.toImage());
	}
	//QImage tmpImage = tmpPixmap.toImage();//.convertToFormat(QImage::Format_Mono, Qt::MonoOnly|Qt::ThresholdDither|Qt::AvoidDither);
	QImage resultImage = lstSourceImages.at(0);
	resultImage.fill(0);
	QColor tmpColor;
	qreal tmpRed = 0.0f;
	qreal tmpGreen = 0.0f;
	qreal tmpBlue = 0.0f;
	int nWidth = lstSourceImages.at(0).width();
	int nHeight = lstSourceImages.at(0).height();
	int nCount = lstSourceImages.count();
	for(int x=0;x<nWidth;x++)
	{
		for(int y=0;y<nHeight;y++)
		{
			for(int z=0;z<nCount;z++)
			{
				tmpColor = QColor(lstSourceImages.at(z).pixel(x,y));
				tmpRed = tmpRed + tmpColor.redF();
				tmpGreen = tmpGreen + tmpColor.greenF();
				tmpBlue = tmpBlue + tmpColor.blueF();
			}
			tmpColor.setRgbF(tmpRed/nCount,tmpGreen/nCount,tmpBlue/nCount);
			resultImage.setPixel(x, y, tmpColor.rgb());
			tmpRed = 0.0f;
			tmpGreen = 0.0f;
			tmpBlue = 0.0f;
		}
	}
	tmpPixmap = tmpPixmap.fromImage(resultImage);
	return tmpPixmap.save(destinationPath,"PNG");
}

//QPixmap ImageProcessor::ChangeHue(QPixmap &p)
//{
	//m_colorMatrix *= QMatrix4x4(
	//	1.164,  0.000,  1.596, -0.8708,
	//	1.164, -0.392, -0.813,  0.5296,
	//	1.164,  2.017,  0.000, -1.081,
	//	0.0,    0.000,  0.000,  1.0000);

	//p.t

	//uniform sampler2D Image;
	//uniform sampler3D RGB2HSI;
	//uniform sampler3D HSI2RGB;

	//uniform float hueShift;
	//uniform float satBoost;

	//varying vec2 texCoord;

	//// Sample the image
	//vec3 rgb = texture2D(Image, texCoord).rgb;
	//// Look up the corresponding HSI value
	//vec3 hsi = texture3D(RGB2HSI, rgb).xyz;

	//// Manipulate hue and saturation
	//hsi.x = fract(hsi.x + hueShift);
	//hsi.y *= satBoost;

	//// Look up the corresponding RGB value
	//gl_FragColor = texture3D(HSI2RGB, hsi);
	
	//uniform sampler2DRect texture;
	//const mat3 rgb2yiq = mat3(0.299, 0.587, 0.114, 0.595716, -0.274453, -0.321263, 0.211456, -0.522591, 0.311135);
	//const mat3 yiq2rgb = mat3(1.0, 0.9563, 0.6210, 1.0, -0.2721, -0.6474, 1.0, -1.1070, 1.7046);
	//uniform float hue;
	//
	//vec3 yColor = rgb2yiq * texture2DRect(texture, gl_TexCoord[0].st).rgb; 

	//float originalHue = atan(yColor.b, yColor.g);
	//float finalHue = originalHue + hue;

	//float chroma = sqrt(yColor.b*yColor.b+yColor.g*yColor.g);

	//vec3 yFinalColor = vec3(yColor.r, chroma * cos(finalHue), chroma * sin(finalHue));
	//gl_FragColor    = vec4(yiq2rgb*yFinalColor, 1.0);
	
	//QImage img = p.toImage().convertToFormat().convertDepth(8);
	//if (img.isNull())
	//{
	//	return p;
	//}
	//for (int i = 0; i < img.numColors(); ++i)
	//{
	//	QRgb rgb = img.color(i);
	//	if (rgb >= 0xFF000000)
	//	{
	//		QColor color(rgb);
	//		int h, s, v;
	//		color.hsv(&h, &s, &v);
	//		s = s * 7 / 10;
	//		v = v * 12 / 10;
	//		if (v > 255) v = 255;
	//		color.setHsv(h, s, v);
	//		img.setColor(i, color.rgb());
	//	}
	//}
	//bofol b = p.convertFromImage(img);
	//return p;
	//return p;
//}
