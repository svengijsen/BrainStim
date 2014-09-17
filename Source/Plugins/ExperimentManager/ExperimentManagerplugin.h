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


//This file inherits the Interface description, please don't change

#ifndef ExperimentManagerPLUGIN_H
#define ExperimentManagerPLUGIN_H

#include <QObject>
#include <QtWidgets>
#include <QString>
#include <Qlibrary>
#include <QMap>

#include "../../BrainStim/plugininterface.h"
#include "ExperimentManager_dialog.h"
#include "ExperimentManager.h"
#include "ImageProcessor.h"
#include "prtformatmanager.h"
#include "TriggerTimer.h"
#include "ExperimentStructures.h"
#include "RetinotopyMapper.h"
#include "QML2Viewer.h"
#include "defines.h"

#define DOCUMENT_CREATEDOCKWINDOWS_SLOT_SIGNATURE	"createDockWindows"

class ExperimentManagerPlugin : public QObject, ExtensionInterface
{
    Q_OBJECT
	Q_PLUGIN_METADATA(IID "BrainStim.Plugins.Extensions" "ExperimentManager.json")
    Q_INTERFACES(ExtensionInterface)

signals:
	void DocumentHasChanged(QString sCanoFilePath, bool bHasChanged);
	void DocumentIsClosing(QString sCanoFilePath, bool bShouldAutoSaveChanges);

public:
	ExperimentManagerPlugin(QObject *parent = 0);
	~ExperimentManagerPlugin();

	int ConfigureScriptEngine(QScriptEngine &engine);
	QString GetMinimalMainProgramVersion() {return PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION;};	

private:	
	bool ExecuteContent(const GlobalApplicationInformation::DocContentInfoStructure &docStruct);
	
	struct strcControlObjects 
	{
		ExperimentManager *pExpManager;
		QWidget *pVisualEditor;
		strcControlObjects()
		{
			pExpManager = NULL;
			pVisualEditor = NULL;
		}
	};

	QMap<QString, strcControlObjects> mapCanPathtoControlObjStrc;
	ExperimentManager *ExperimentManagerObject; 
	ExperimentManager_Dialog *ExperimentManagerDiagObject;
	TriggerTimer *TriggerTimerObject;
	ExperimentTimer *ExperimentTimerObject;
	RandomGenerator *RandomGeneratorObject;
	cExperimentStructure *cExperimentStructureObject;
	cBlockStructure *cBlockStructureObject;
	cObjectStructure *cObjectStructureObject;
	cLoopStructure *cLoopStructureObject;
	cMethodStructure *cMethodStructureObject;
	cMethodConnectionStructure *cMethodConnectionStructureObject;
	ImageProcessor *ImageProcessorObject;
	PrtFormatManager *PrtFormatManagerObject;
	RetinotopyMapper *RetinotopyMapperObject;
	QML2Viewer *Qml2ViewerObject;

public slots:
    bool ShowGUI();
	bool IsCompatible() {return PluginInterface::IsCompatible();};
	QStringList GetAdditionalFileExtensions() {return (QStringList() << "Experiment files (*.exml);;" << "QML documents (*.qml);;");};
	QStringList GetAdditionalFileSlotHandlers() {return (QStringList() << "exml|ExperimentManagerEXMLDocumentHandler(const QString &, const QString &)|ExperimentManagerEXMLNewDocumentHandler()" << "qml|ExperimentManagerQMLDocumentHandler(const QString &, const QString &)|ExperimentManagerNewQMLDocumentHandler()" << "xdef");};
	bool ExperimentManagerEXMLDocumentHandler(const QString &docContent, const QString &strHomePath);
	bool ExperimentManagerQMLDocumentHandler(const QString &docContent, const QString &strHomePath);
	QObject *GetScriptMetaObject(int nIndex);
	int GetAdditionalFileTypeStyle(QString strExtension);
	QString GetAdditionalFileTypeApiName(QString strExtension);
	QWidget *GetAdditionalFileTypeEditor(QString strExtension, QString strCanonicalFilePath);
	void InitializeAdditionalFileTypeEditor(QString strCanonicalFilePath);
	//bool RemAdditionalFileTypeEditor(QString strExtension, QString strCanonicalFilePath);
	bool LoadAdditionalFile(QString strFilePath);
	//bool SaveAdditionalFile(QString strFilePath);
	bool RemoveAdditionalFile(QString strFilePath);
};
#endif//ExperimentManagerPLUGIN_H
