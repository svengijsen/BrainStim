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

#include "OgreLightNode.h"
#include <OgreRoot.h>
#include <OgreSceneNode.h>
#include <OgreLight.h>
#include <QDebug>

OgreLightNode::OgreLightNode(Ogre::Light *light, QObject *parent) : QObject(parent), m_light(light),m_yaw(0),m_pitch(0)
{
    m_node = Ogre::Root::getSingleton().getSceneManager("mySceneManager")->getRootSceneNode()->createChildSceneNode();
    m_node->attachObject(light);
	m_position = Ogre::Vector3(0, 0, 300);
    light->setPosition(m_position);
}

void OgreLightNode::updateRotation()
{
	m_node->resetOrientation();
	m_node->yaw(Ogre::Radian(Ogre::Degree(m_yaw)));
	m_node->pitch(Ogre::Radian(Ogre::Degree(m_pitch)));
}

QVector3D OgreLightNode::position() const
{ 
	return QVector3D(m_position.x,m_position.y,m_position.z); 
}

void OgreLightNode::setPosition(const float &x, const float &y, const float &z)
{
	m_position.x = x; 
	m_position.y = y; 
	m_position.z = z;
	m_light->setPosition(m_position);
	updateRotation();
}

void OgreLightNode::setPosition(QVector3D p)
{ 
	m_position.x = p.x(); 
	m_position.y = p.y(); 
	m_position.z = p.z();
	//m_node->resetOrientation();
	m_light->setPosition(m_position);// * (1 / m_zoom));
	updateRotation();	
}