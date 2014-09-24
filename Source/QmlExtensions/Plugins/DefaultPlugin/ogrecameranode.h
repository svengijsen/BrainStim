//DefaultQMLPlugin
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

#ifndef OgreCameraNode_H
#define OgreCameraNode_H

#include <QObject>
#include <QVector3D>
#include "OgreVector3.h"

namespace Ogre 
{
class SceneNode;
class Camera;
}

//!  The OgreCameraNode class. 
/*!
  The OgreCameraNode Class implements access to the camera object from the OgreItem class.
*/
class OgreCameraNode : public QObject
{
    Q_OBJECT
	//! \brief yaw property.
	/*!  Use this property to change the camera's yaw property.
    */
    Q_PROPERTY(qreal yaw READ yaw WRITE setYaw)
	//! \brief pitch property.
	/*!  Use this property to change the camera's pitch property.
    */
    Q_PROPERTY(qreal pitch READ pitch WRITE setPitch)
	//! \brief zoom property.
	/*!  Use this property to change the camera's zoom property.
    */
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom)
	//! \brief position property.
	/*!  Use this property to change the camera's position property.
    */
	Q_PROPERTY(QVector3D position READ position WRITE setPosition)
public:
    explicit OgreCameraNode(Ogre::Camera *cam, QObject *parent = 0);

    Ogre::SceneNode *sceneNode() const
    { return m_node; }
    Ogre::Camera *camera() const
    { return m_camera; }

    qreal yaw() const
    { return m_yaw; }
    qreal pitch() const
    { return m_pitch; }
    qreal zoom() const
    { return m_zoom; }
    void setYaw(qreal y)
    { m_yaw = y; updateRotation(); }
    void setPitch(qreal p)
    { m_pitch = p; updateRotation(); }
    void setZoom(qreal z);
	QVector3D position() const
	{ return QVector3D(m_position.x,m_position.y,m_position.z); }
	void setPosition(QVector3D p);

public slots:
	//! \brief setAutoTracking slot.
	/*!  This function configures the camera auto-tracking feature that automatically tracks a scene node in the 3D scene.
	 * @remarks
	 * If you enable auto-tracking, this Camera will automatically rotate to look at the target SceneNode every frame, 
	 * no matter how it or SceneNode move. This is handy if you want a Camera to be focused on a single object or group of objects. 
	 * Note that by default the Camera looks at the origin of the SceneNode, if you want to tweak this, e.g. if the object which is
	 * attached to this target node is quite big and you want to point the camera at a specific point on it, provide a vector in the 
	 * 'offset' parameter and the camera's target point will be adjusted.
	 * @param bEnable a boolean value determining whether the auto-tracking feature should be enabled. 
	 * If false the camera will cease tracking and will remain in it's current orientation.
	 * @param sSceneNodeName a string containing name of the scene node to track
	 * @param vecOffset a QVector3D structure (optional, default = QVector3D(0.0,0.0,0.0)) containing 
	 * a point the camera targets in local space of the scene node (instead of the origin of the scene node). Good for fine tuning the look at point.
	 * @return a boolean value determining whether the function executed successfully.
	 */	
	bool setAutoTracking(const bool &bEnable, const QString &sSceneNodeName = "", const QVector3D &vecOffset = QVector3D(0,0,0));
	
private:
    void updateRotation();

    Ogre::SceneNode *m_node;
    Ogre::Camera *m_camera;

    qreal m_yaw;
    qreal m_pitch;
    qreal m_zoom;
	Ogre::Vector3 m_position;
};

#endif // OgreCameraNode_H
