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


#ifndef RETINOTOPYMAPPERWINDOW_H
#define RETINOTOPYMAPPERWINDOW_H

#include "openglwindow.h"
#include "retinotopymapper.h"

class RetinotopyMapper;

class RetinotopyMapperWindow : public OpenGLWindow
{
public:
	RetinotopyMapperWindow(RetinotopyMapper *parent = NULL);
	~RetinotopyMapperWindow();

	QScreen *grabScreenUnderMouseCursor();

	void initialize();
	//void render();
	void render(QPainter *stimuliPainter);
	void postSwapBuffers();

protected:
	bool drawPolar();
	bool drawMovingBar();
	bool drawEccentricity();
	bool drawMovingDots();
	bool drawFixation();
	bool drawCustom();

private:
	QImage fractalFillCheckeredImage(float fWidth, float fHeigth, float fSize, int nflickr);
	bool doOutputTriggerFrame(const ExperimentStructuresNameSpace::ExperimentStructureState &tmpExpStrState,const cExperimentStructure &tmpExpStr, const cBlockStructure &tmpBlockStrc, const QPixmap &pix2Output);

	RetinotopyMapper *parentRetinotopyMapper;

	QImage mirrorImage;
	QPainter activationPainter;
	QPainter imgPainter;
	
	bool bCDATFileReadyToWrite;
	QFile *outputFile;
	QDataStream *CDatOutputStream;
	quint32 OutputImageItemCount;
	quint32 OutputImageWidth;
	quint32 OutputImageHeight;

	bool bRenderStimuli;
	bool bFistRenderCall;
	int nStimFrameHeight;
	int nStimFrameWidth;
	int nLastOutputTriggerFrameNumber;
};

#endif // RETINOTOPYMAPPERWINDOW_H
