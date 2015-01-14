//DefaultQMLPlugin
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

#ifndef OGREITEM_H
#define OGREITEM_H

#include <QtQuick/QQuickItem>
#include <QtCore/QPropertyAnimation>
#include "ogrenode.h"

class OgreCameraNode;

//!  The OgreItem class. 
/*!
  The OgreItem Class implement the Ogre3D SDK and makes the Ogre3D API accessible from within the QML engine (see http://www.ogre3d.org/).
*/
class OgreItem : public QQuickItem
{
    Q_OBJECT
	//! \brief camera property.
	/*!  Use this property to access the OgreCameraNode object.
    */
    Q_PROPERTY(QObject *camera READ camera)

signals:
	//! \brief ogreNodeInitialized() signal.
	/*!  This signal is emitted just after the Ogre3D engine is initialized.
	 */
	void ogreNodeInitialized();
	//! \brief ogreNodePreInitialize() signal.
	/*!  This signal is emitted just before the Ogre3D engine is initialized.
	 */
	void ogreNodePreInitialize();

public:
    OgreItem(QQuickItem *parent = 0);
	~OgreItem();
    QObject *camera() const { return m_camera; }

public slots:
	//! \brief addResourceLocation slot.
	/*!  This function can add/append a specific resource location to the Ogre3D engine, just before it initialized.
	 * @param sLocation a string containing the path to the resource location
	 * @param sType a string containing the type of the resource location this can be "FileSystem" or "Zip"
	 * @return a boolean value determining whether the function executed successfully.
	 */	
	bool addResourceLocation(const QString &sLocation,const QString &sType);
	//! \brief createEntity slot.
	/*!  This function creates an 3D entity from a mesh (*.mesh) file and sets its name.
	 * @param sEntityName a string containing the name for the mesh, this name should be unique for all the meshes in the 3D scene.
	 * @param sEntityMesh a string containing the filename of the mesh (*.mesh) file that is available in one of the pre-defined resource locations.
	 * @return a boolean value determining whether the function executed successfully.
	 */	
	bool createEntity(const QString &sEntityName, const QString &sEntityMesh);
	//! \brief createSceneNode slot.
	/*!  This function creates and places a scene node in the 3D Scene at a custom (x,y,z) position.
	 * @param sNodeName a string value that contains the name for the scene node
	 * @param sEntityName a string value that contains the name of the entity used for creating this scene node
	 * @param xPos a float value that contains the x-position of the scene node in the 3D scene world coordinate system
	 * @param yPos a float value that contains the y-position of the scene node in the 3D scene world coordinate system
	 * @param zPos a float value that contains the z-position of the scene node in the 3D scene world coordinate system
	 * @return a boolean value determining whether the function executed successfully.
	 */
	bool createSceneNode(const QString &sNodeName, const QString &sEntityName, const float &xPos, const float &yPos, const float &zPos);
	//! \brief createLightSource slot.
	/*!  This function creates a light source node for the 3D scene.
	 * @param sLightName a string value that contains the name for the light node
	 * @param xPos a float value that contains the x-position of the light node in the 3D scene world coordinate system
	 * @param yPos a float value that contains the y-position of the light node in the 3D scene world coordinate system
	 * @param zPos a float value that contains the z-position of the light node in the 3D scene world coordinate system
	 * @return a boolean value determining whether the function executed successfully.
	 */
	bool createLightSource(const QString &sLightName, const float &xPos, const float &yPos, const float &zPos);
	//! \brief getLightSource slot.
	/*!  This function returns a pointer to an QObject (OgreLightNode *) containing a specific light source defined by its name.
	 * @param sLightName a string value that contains the name of the light source
	 * @return a pointer to an QObject (OgreLightNode *) containing a specific light source, in case of a failure this function return a NULL value.
	 */
	QObject *getLightSource(const QString &sLightName);
	//! \brief setMaterialName slot.
	/*!  This function sets the material of an entity from the scene.
	 * @param sEntityName a string value that contains the name of the scene entity
	 * @param sMaterial a string value that contains the material to use. This name should be the same as the (*.material) file 
	 * which is present in the same directory as the (*.mesh) file and defines the material with provided material name.
	 * @return a boolean value determining whether the function executed successfully.
	 */
	bool setMaterialName(const QString &sEntityName, const QString &sMaterial);
	//! \brief setAmbientColor slot.
	/*!  This function set the ambient color for the 3D scene.
	 * @param nRed a integer value containing the level of the red component for the ambient color
	 * @param nGreen a integer value containing the level of the green component for the ambient color
	 * @param nBlue a integer value containing the level of the blue component for the ambient color
	 * @param nAlpha a integer value (optional, default = 255) containing the level of the alpha component for the ambient color
	 * @return a boolean value determining whether the function executed successfully.
	 */
	bool setAmbientColor(const int &nRed, const int &nGreen, const int &nBlue, const int &nAlpha = 255);
	//! \brief getObjectBoundingBoxCenter slot.
	/*!  This function returns a 3D vector containing the center position of a bounding box from a specific object attached to a scene node.
	 * @param sSceneNodeName a string value that contains the name of the scene node
	 * @param sObjectName a string value that contains the name of the object
	 * @return a QVector3D structure that contains the requested result (x,y,z), in case of a failure this function writes an error code.
	 */
	QVector3D getObjectBoundingBoxCenter(const QString &sSceneNodeName, const QString &sObjectName);
	//! \brief getObjectBoundingBoxSize slot.
	/*!  This function returns a 3D vector containing the size of a bounding box from a specific object attached to a scene node.
	 * @param sSceneNodeName a string value that contains the name of the scene node
	 * @param sObjectName a string value that contains the name of the object
	 * @return a QVector3D structure that contains the requested result (x,y,z), in case of a failure this function writes an error code.
	 */
	QVector3D getObjectBoundingBoxSize(const QString &sSceneNodeName, const QString &sObjectName);
	//! \brief setObjectVisibility slot.
	/*!  This function sets the visibility of a object that is attached to a scene node.
	 * @param sSceneNodeName a string value that contains the name of the scene node
	 * @param sObjectName a string value that contains the name of the object
	 * @param bVisible a boolean value that determines whether the object should be visible
	 */
	void setObjectVisibility(const QString &sSceneNodeName, const QString &sObjectName, const bool &bVisible);
	//bool setObjectAttachement(const QString &sSceneNodeName, const QString &sObjectName, const bool &bAttachment);
	//void startRenderLoop();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    void timerEvent(QTimerEvent *);

private:
	bool bIsPreInitializing;
	bool bFirstUpdatePaintNode;
    int m_timerID;
    QObject *m_camera;
	OgreNode *m_node;
	QList<sTypeOgreResourcesStructure> lResources;
	QList<sEntityStructure> lEntities;
	QList<sSceneNodeStructure> lSceneNodes;
	QList<sLightSourceStructure> lLightSources;
	QColor *cAmbientSceneColor;		
};

#endif // OGREITEM_H
