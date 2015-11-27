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

#include "ogreitem.h"
#include "ogrecameranode.h"
#include <QtCore/QPropertyAnimation>
#include <QTimer>
#include <QTest>
#include <windows.h>

OgreItem::OgreItem(QQuickItem *parent) : QQuickItem(parent), m_timerID(0)
{
	bIsPreInitializing = false;
	bWindowActivated = false;
	m_node = NULL;
	m_camera = NULL;
	cAmbientSceneColor = NULL;
    setFlag(ItemHasContents);
    setSmooth(false);
    bFirstUpdatePaintNode = true;
	m_timerID = startTimer(16);
}

OgreItem::~OgreItem()
{
	if(m_timerID)
	{
		killTimer(m_timerID);
		m_timerID = 0;
	}
	lResources.clear();
	lEntities.clear();
	lSceneNodes.clear();
	lLightSources.clear();
	if(cAmbientSceneColor)
	{
		delete cAmbientSceneColor;
		cAmbientSceneColor = NULL;
	}
	if(Ogre::Root::getSingletonPtr() != NULL)
	{
		Ogre::Root::getSingletonPtr()->shutdown();
		delete Ogre::Root::getSingletonPtr();
	}
}

bool OgreItem::addResourceLocation(const QString &sLocation,const QString &sType)
{
	if (sType == "FileSystem")
	{
		QDir tmpDir;
		tmpDir.setPath(sLocation);
		if (tmpDir.exists() == false)
			return false;
	}
	else if (sType == "Zip")
	{
		QFileInfo tmpFileInfo;
		tmpFileInfo.setFile(sLocation);
		if (tmpFileInfo.exists() == false)
			return false;
	}
	else
	{
		return false;
	}
	sTypeOgreResourcesStructure tmpResource;
	tmpResource.sLocation = sLocation;
	tmpResource.sType = sType;
	lResources.append(tmpResource);
	return true;
}

bool OgreItem::createEntity(const QString &sEntityName, const QString &sEntityMesh)
{
	sEntityStructure tmpEntity;
	tmpEntity.pEntity = NULL;
	tmpEntity.sName = sEntityName;
	tmpEntity.sMesh = sEntityMesh;
	lEntities.append(tmpEntity);
	return true;
}

bool OgreItem::createSceneNode(const QString &sNodeName, const QString &sEntityName, const float &xPos, const float &yPos, const float &zPos)
{
	sSceneNodeStructure tmpSceneNode;
	tmpSceneNode.pSceneNode = NULL;
	tmpSceneNode.sName = sNodeName;
	tmpSceneNode.sEntityName = sEntityName;
	tmpSceneNode.xPos = xPos;
	tmpSceneNode.yPos = yPos;
	tmpSceneNode.zPos = zPos;
	lSceneNodes.append(tmpSceneNode);
	return true;
}

bool OgreItem::createLightSource(const QString &sLightName, const float &xPos, const float &yPos, const float &zPos)
{
	sLightSourceStructure tmpLightSource;
	tmpLightSource.pLightSource = NULL;
	tmpLightSource.sName = sLightName;
	tmpLightSource.xPos = xPos;
	tmpLightSource.yPos = yPos;
	tmpLightSource.zPos = zPos;
	tmpLightSource.bAppended = false;
	lLightSources.append(tmpLightSource);
	return true;
}

QObject *OgreItem::getLightSource(const QString &sLightName)
{
	if(m_node)
	{
		OgreLightNode *tmpLight = m_node->getLightSource(sLightName);
		if(tmpLight)
			return (QObject *)tmpLight;
	}
	return NULL;
}

bool OgreItem::setMaterialName(const QString &sEntityName, const QString &sMaterial)
{
	if(m_node)
	{
		return m_node->setMaterialName(sEntityName,sMaterial);
	}
	return false;
}

bool OgreItem::setAmbientColor(const int &nRed, const int &nGreen, const int &nBlue, const int &nAlpha)
{
	cAmbientSceneColor = new QColor(nRed, nGreen, nBlue, nAlpha);
	return true;
}

QVector3D OgreItem::getObjectBoundingBoxCenter(const QString &sSceneNodeName, const QString &sObjectName)
{
	try
	{
		Ogre::Vector3 tmpVec = Ogre::Root::getSingleton().getSceneManager("mySceneManager")->getSceneNode(sSceneNodeName.toLocal8Bit().constData())->getAttachedObject(sObjectName.toLocal8Bit().constData())->getWorldBoundingBox().getCenter();//getCorner(Ogre::AxisAlignedBox::FAR_LEFT_BOTTOM);//Ogre::CornerEnum::FAR_LEFT_BOTTOM
		return QVector3D(tmpVec.x,tmpVec.y,tmpVec.z);
	}
	catch(std::exception &e) 
	{
		qDebug() << __FUNCTION__ << QString("Error %1").arg(e.what());
		return QVector3D(0.0, 0.0, 0.0);
	}
	catch(...)
	{
		qDebug() << __FUNCTION__ << ("An Error occurred");
		return QVector3D(0.0, 0.0, 0.0);
	}	
}

QVector3D OgreItem::getObjectBoundingBoxSize(const QString &sSceneNodeName, const QString &sObjectName)
{
	try
	{
		Ogre::Vector3 tmpVec = Ogre::Root::getSingleton().getSceneManager("mySceneManager")->getSceneNode(sSceneNodeName.toLocal8Bit().constData())->getAttachedObject(sObjectName.toLocal8Bit().constData())->getBoundingBox().getSize();
		return QVector3D(tmpVec.x,tmpVec.y,tmpVec.z);
	}
	catch(std::exception &e) 
	{
		qDebug() << __FUNCTION__ << QString("Error %1").arg(e.what());
		return QVector3D(0.0, 0.0, 0.0);
	}
	catch(...)
	{
		qDebug() << __FUNCTION__ << ("An Error occurred");
		return QVector3D(0.0, 0.0, 0.0);
	}	
}

void OgreItem::setObjectVisibility(const QString &sSceneNodeName, const QString &sObjectName, const bool &bVisible)
{
	try
	{
		Ogre::Root::getSingleton().getSceneManager("mySceneManager")->getSceneNode(sSceneNodeName.toLocal8Bit().constData())->getAttachedObject(sObjectName.toLocal8Bit().constData())->setVisible(bVisible);
	}
	catch(std::exception &e) 
	{
		qDebug() << __FUNCTION__ << QString("Error %1").arg(e.what());
	}
	catch(...)
	{
		qDebug() << __FUNCTION__ << ("An Error occurred");
	}
}

//bool OgreItem::setObjectAttachement(const QString &sSceneNodeName, const QString &sObjectName, const bool &bAttachment)
//{
//	if(bAttachment == false)
//		Ogre::Root::getSingleton().getSceneManager("mySceneManager")->getSceneNode(sSceneNodeName.toLocal8Bit().constData())->getAttachedObject(sObjectName.toLocal8Bit().constData())->detachFromParent(bAttachment);
//	else
//		Ogre::Root::getSingleton().getSceneManager("mySceneManager")->getSceneNode(sSceneNodeName.toLocal8Bit().constData())->attachObject(sObjectName.toLocal8Bit().constData()));
//	return false;
//}

QSGNode *OgreItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
	if(bIsPreInitializing)
		return NULL;
    if (width() <= 0 || height() <= 0) {
        delete oldNode;
        return 0;
    }
    m_node = static_cast<OgreNode *>(oldNode);
	bool bDoEmit = false;
	bool bWasFirstUpdatePaintNode = false;
    if (!m_node)
    {
        m_node = new OgreNode();
		if(bFirstUpdatePaintNode)
		{
			bFirstUpdatePaintNode = false;
			bWasFirstUpdatePaintNode = true;
			if(lResources.isEmpty() == false)
			{
				m_node->setResourceLocations(lResources);
				lResources.clear();
				m_node->setEntities(lEntities);
				lEntities.clear();
				m_node->setSceneNodes(lSceneNodes);
				lSceneNodes.clear();
				if(cAmbientSceneColor)
				{
					m_node->setAmbientLight(*cAmbientSceneColor);
					delete cAmbientSceneColor;
					cAmbientSceneColor = NULL;
				}
				foreach(sLightSourceStructure lLightSource,lLightSources)
				{
					if(lLightSource.bAppended == false)
					{
						lLightSource.bAppended = m_node->appendLightSource(lLightSource);
					}
				}
				//m_node->setLightSources(lLightSources);
				//lLightSources.clear();
			}
		}
		m_node->setQuickWindow(window());
		bDoEmit = true;
    }
    m_node->setSize(QSize(width(), height()));
    m_node->setAAEnabled(smooth());
	if(bWasFirstUpdatePaintNode)
	{
		if(bDoEmit)
			QTimer::singleShot(1, this, SIGNAL(ogreNodePreInitialize()));//Thread safety
		bIsPreInitializing = true;
		m_node->preInit();
		bIsPreInitializing = false;
	}
    m_node->update();
    m_camera = static_cast<QObject *>(m_node->camera());
	if(bDoEmit)
		QTimer::singleShot(1, this, SIGNAL(ogreNodeInitialized()));//Thread safety

	if (bWasFirstUpdatePaintNode)
	{
		//Ogre::Root *m_root;
		//Ogre::Camera *m_camera;
		//Ogre::SceneManager *m_sceneManager;
		//Ogre::RenderTexture *m_renderTexture;
		//Ogre::Viewport *m_viewport;
		//Ogre::TexturePtr rtt_texture;
		//Ogre::RenderWindow *m_window;
		
		//QEvent event(QEvent::FocusIn);
		//event.
		//window()->sendEvent(this, &event);//QQuickItem *item, QEvent *e);

		if (bWindowActivated == false)
		{
			bWindowActivated = true;
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 100, 200, 0, 0);
			mouse_event(MOUSEEVENTF_RIGHTUP, 100, 200, 0, 0);	
		}
	}
    return m_node;
}

void OgreItem::timerEvent(QTimerEvent *)
{
    update();
}
