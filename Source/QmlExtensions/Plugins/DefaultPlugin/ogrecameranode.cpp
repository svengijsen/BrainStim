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

#include "ogrecameranode.h"
#include <OgreRoot.h>
#include <OgreSceneNode.h>
#include <OgreCamera.h>
#include <QDebug>

OgreCameraNode::OgreCameraNode(Ogre::Camera *cam, QObject *parent) :
    QObject(parent),
    m_camera(cam),
    m_yaw(0),
    m_pitch(0),
    m_zoom(1)
{
    m_node = Ogre::Root::getSingleton().getSceneManager("mySceneManager")->getRootSceneNode()->createChildSceneNode();
    m_node->attachObject(cam);
	m_position = Ogre::Vector3(0, 0, 300);
    cam->move(m_position);
}

void OgreCameraNode::updateRotation()
{
    m_node->resetOrientation();
    m_node->yaw(Ogre::Radian(Ogre::Degree(m_yaw)));
    m_node->pitch(Ogre::Radian(Ogre::Degree(m_pitch)));
}

void OgreCameraNode::setZoom(qreal z)
{
    m_zoom = z;
    m_node->resetOrientation();
    m_camera->setPosition(m_position * (1 / m_zoom));
    updateRotation();
}

void OgreCameraNode::setPosition(QVector3D p)
{ 
	m_position.x = p.x(); 
	m_position.y = p.y(); 
	m_position.z = p.z();
	//m_node->resetOrientation();
	m_camera->setPosition(m_position * (1 / m_zoom));
	updateRotation();
}

bool OgreCameraNode::setAutoTracking(const bool &bEnable, const QString &sSceneNodeName, const QVector3D &vecOffset)
{
	if(bEnable == false)
	{
		m_camera->setAutoTracking(false);
		return true;
	}
	if(sSceneNodeName.isEmpty() == false)
	{
		try
		{
			Ogre::SceneNode *tmpSceneNode = Ogre::Root::getSingleton().getSceneManager("mySceneManager")->getSceneNode(sSceneNodeName.toLocal8Bit().constData());
			if(tmpSceneNode)
			{			
				m_camera->setAutoTracking(true, tmpSceneNode,Ogre::Vector3(vecOffset.x(),vecOffset.y(),vecOffset.z()));
				return true;
			}
		}
		catch(std::exception &e) 
		{
			qDebug() << __FUNCTION__ << QString("Error %1").arg(e.what());
			return false;
		}
		catch(...)
		{
			qDebug() << __FUNCTION__ << ("An Error occurred");
			return false;
		}	
	}
	return false;
}
