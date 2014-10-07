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

#include <RenderSystems/GL/OgreGLTexture.h>
#include <RenderSystems/GL/OgreGLFrameBufferObject.h>
#include <RenderSystems/GL/OgreGLFBORenderTexture.h>

#include "ogrenode.h"
#include "ogrecameranode.h"
#include "ogrelightnode.h"
#include <Ogre.h>
#include "defines.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

static QString appPath()
{
    QString path = QCoreApplication::applicationDirPath();
    QDir dir(path);
#ifdef Q_WS_MAC
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
#elif defined(Q_WS_WIN)
    dir.cdUp();
#endif
    return dir.absolutePath();
}

OgreNode::OgreNode() : QSGGeometryNode()
    , m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
    , m_texture(0)
    , m_samples(0)
    , m_quickWindow(0)
    , m_ogreContext(0)
    , m_qtContext(0)
    , m_AAEnabled(false)
    , m_renderTexture(0)
    , m_ogreFBO(0)
    , m_initialized(false)
    , m_dirtyFBO(false)
	, m_root(0)
	, m_camera(0)
	, cAmbientSceneColor(NULL)
{
    setMaterial(&m_material);
    setOpaqueMaterial(&m_materialO);
    setGeometry(&m_geometry);
    setFlag(UsePreprocess);
}

OgreNode::~OgreNode()
{
	if(cAmbientSceneColor)
	{
		delete cAmbientSceneColor;
		cAmbientSceneColor = NULL;
	}
    if (m_renderTexture) 
	{
        m_renderTexture->removeAllViewports();		
    }
	//if(m_cameraObject)
	//{
	//	delete m_cameraObject;
	//	m_cameraObject = NULL;
	//}
	//m_root = Ogre::Root::getSingleton().getSingletonPtr();
 //   if (m_root.get()) 
	//if (m_root)
	//{
 //       //m_root->detachRenderTarget(m_renderTexture);
 //       if (m_sceneManager) 
	//	{
	//		m_sceneManager->destroyAllCameras();
	//		m_sceneManager->destroyAllEntities();
	//		m_sceneManager->destroyAllLights();
	//		m_sceneManager->destroyAllManualObjects();
	//		m_sceneManager->getRootSceneNode()->removeAndDestroyAllChildren();
	//		m_sceneManager->clearScene();
	//		//Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	//		m_root->destroySceneManager(m_sceneManager);
	//		if(m_window)
	//		{
	//			m_root->detachRenderTarget(m_window);
	//			m_root->destroyRenderTarget(m_window);			
	//		}
	//		m_root->destroyRenderTarget(m_renderTexture);
	//		m_renderTexture = NULL;
 //       }
	//	m_root->destroyAllRenderQueueInvocationSequences();
	//	//return;
	//	//m_root = NULL;
 //   }

	//if(m_ogreContext)
	//{
	//	delete m_ogreContext;
	//	m_ogreContext = NULL;
	//}
	//if (m_root.get()) 
	//{
	//	delete m_root;
	//	m_root.reset();
	//}
}

void OgreNode::saveOgreState()
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    ctx->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    ctx->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    ctx->functions()->glBindRenderbuffer(GL_RENDERBUFFER, 0);
    ctx->functions()->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

    ctx->doneCurrent();
    m_qtContext->makeCurrent(m_quickWindow);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
}

void OgreNode::restoreOgreState()
{
    glPopAttrib();

    m_qtContext = QOpenGLContext::currentContext();
    m_qtContext->functions()->glUseProgram(0);
    m_qtContext->doneCurrent();

    m_ogreContext->makeCurrent(m_quickWindow);
    m_ogreContext->functions()->glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_ogreFBO);
}

GLuint OgreNode::getOgreFBO()
{
    if (!m_renderTexture)
        return 0;
    Ogre::GLFrameBufferObject *ogreFbo = 0;
    m_renderTexture->getCustomAttribute("FBO", &ogreFbo);
    Ogre::GLFBOManager *manager = ogreFbo->getManager();
    manager->bind(m_renderTexture);
    GLint id;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &id);
    return id;
}

void OgreNode::preprocess()
{
    restoreOgreState();
	m_renderTexture->update(true);  
    saveOgreState();
}

void OgreNode::setQuickWindow(QQuickWindow *window)
{
    m_quickWindow = window;
    // create a new shared OpenGL context to be used exclusively by Ogre
    m_ogreContext = new QOpenGLContext();
    m_ogreContext->setFormat(m_quickWindow->requestedFormat());
    m_ogreContext->setShareContext(QOpenGLContext::currentContext());
    m_ogreContext->create();
}

void OgreNode::update()
{
    restoreOgreState();
    if (!m_initialized)
        init();
    if (m_dirtyFBO) 
	{
        updateFBO();
        m_ogreFBO = getOgreFBO();
        m_dirtyFBO = false;
    }
    saveOgreState();
}

void OgreNode::updateFBO()
{
    if (m_renderTexture)
        Ogre::TextureManager::getSingleton().remove("RttTex");
    rtt_texture = Ogre::TextureManager::getSingleton().createManual("RttTex",
                                                                    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                    Ogre::TEX_TYPE_2D,
                                                                    m_size.width(),
                                                                    m_size.height(),
                                                                    0,
                                                                    Ogre::PF_R8G8B8A8,
                                                                    Ogre::TU_RENDERTARGET, 0, false,
                                                                    m_AAEnabled ? m_samples : 0);
    m_renderTexture = rtt_texture->getBuffer()->getRenderTarget();
    m_renderTexture->addViewport(m_camera);
    m_renderTexture->getViewport(0)->setClearEveryFrame(true);
    m_renderTexture->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
    m_renderTexture->getViewport(0)->setOverlaysEnabled(false);
    Ogre::Real aspectRatio = Ogre::Real(m_size.width()) / Ogre::Real(m_size.height());
    m_camera->setAspectRatio(aspectRatio);
    QSGGeometry::updateTexturedRectGeometry(&m_geometry,
                                            QRectF(0, 0, m_size.width(), m_size.height()),
                                            QRectF(0, 0, 1, 1));
    Ogre::GLTexture *nativeTexture = static_cast<Ogre::GLTexture *>(rtt_texture.get());
    delete m_texture;
    m_texture = m_quickWindow->createTextureFromId(nativeTexture->getGLID(), m_size);
    m_material.setTexture(m_texture);
    m_materialO.setTexture(m_texture);
}

void OgreNode::setSize(const QSize &size)
{
    if (size == m_size)
        return;
    m_size = size;
    m_dirtyFBO = true;
    markDirty(DirtyGeometry);
}

void OgreNode::setAAEnabled(bool enable)
{
    if (m_AAEnabled == enable)
        return;
    m_AAEnabled = enable;
    m_dirtyFBO = true;
    markDirty(DirtyMaterial);
}

bool OgreNode::setResourceLocations(const QList<sTypeOgreResourcesStructure> lResources)
{
	lBufferedResources = lResources;
	return true;
}

bool OgreNode::setEntities(const QList<sEntityStructure> lEntities)
{
	lBufferedEntities = lEntities;
	return true;
}

bool OgreNode::setSceneNodes(const QList<sSceneNodeStructure> lSceneNodes)
{
	lBufferedSceneNodes = lSceneNodes;
	return true;
}

bool OgreNode::setAmbientLight(const QColor &cColor)
{
	cAmbientSceneColor = new QColor(cColor);
	return true;
}

bool OgreNode::setLightSources(const QList<sLightSourceStructure> lLightSources)
{
	lBufferedLightSources = lLightSources;
	return true;
}

bool OgreNode::appendLightSource(sLightSourceStructure &lLightSource)
{
	lBufferedLightSources.append(lLightSource);
	return true;
}

OgreLightNode *OgreNode::getLightSource(const QString &sLightName)
{
	foreach(sLightSourceStructure tmpLightSource,lBufferedLightSources)
	{
		if ((tmpLightSource.sName.toLower() == sLightName.toLower()) && tmpLightSource.bAppended)
		{
			return tmpLightSource.pLightSource;
		}
	}
	return NULL;
}

bool OgreNode::configureUserSettings()
{
	int i,j;
	int nFails = 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(lBufferedResources.isEmpty() == false)
	{
		for(i=0;i<lBufferedResources.count();i++)
		{
			if((lBufferedResources.at(i).sType == "FileSystem") || (lBufferedResources.at(i).sType == "Zip"))
			{
				try
				{
					Ogre::ResourceGroupManager::getSingleton().addResourceLocation(lBufferedResources.at(i).sLocation.toLatin1().data(), lBufferedResources.at(i).sType.toLatin1().data());					
				}
				catch(std::exception &e) 
				{
					qDebug() << __FUNCTION__ << QString("Error while adding a Ogre3D Resource %1").arg(e.what());
					nFails++;
				}
				catch(...)
				{
					qDebug() << __FUNCTION__ << ("Error while adding a Ogre3D Resource.");
					nFails++;
				}
			}
			else
			{
				nFails++;
			}
		}
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(lBufferedEntities.isEmpty() == false)
	{
		for(i=0;i<lBufferedEntities.count();i++)
		{
			try
			{
				lBufferedEntities[i].pEntity = m_sceneManager->createEntity((Ogre::String)lBufferedEntities.at(i).sName.toLocal8Bit().constData(), (Ogre::String)lBufferedEntities.at(i).sMesh.toLocal8Bit().constData());
			}
			catch(std::exception &e) 
			{
				qDebug() << __FUNCTION__ << QString("Error while adding a Ogre3D Entity %1").arg(e.what());
				nFails++;
			}
			catch(...)
			{
				qDebug() << __FUNCTION__ << ("Error while adding a Ogre3D Entity.");
				nFails++;
			}			
		}
		for(i=0;i<lBufferedSceneNodes.count();i++)
		{
			try
			{
				lBufferedSceneNodes[i].pSceneNode = m_sceneManager->getRootSceneNode()->createChildSceneNode(lBufferedSceneNodes[i].sName.toLocal8Bit().constData(), Ogre::Vector3(lBufferedSceneNodes[i].xPos, lBufferedSceneNodes[i].yPos, lBufferedSceneNodes[i].zPos));
				if((lBufferedSceneNodes[i].pSceneNode) && (lBufferedSceneNodes[i].sEntityName.isEmpty() == false))
				{
					for(j=0;j<lBufferedEntities.count();j++)
					{
						if(lBufferedEntities[i].sName == lBufferedSceneNodes[i].sEntityName)
						{
							lBufferedSceneNodes[i].pSceneNode->attachObject(lBufferedEntities[i].pEntity);
							///m_sceneManager->getRootSceneNode()->attachObject(ogreHead);
							break;
						}
					}				
				}				
			}
			catch(std::exception &e) 
			{
				qDebug() << __FUNCTION__ << QString("Error while adding a Ogre3D Scene Node %1").arg(e.what());
				nFails++;
			}
			catch(...)
			{
				qDebug() << __FUNCTION__ << ("Error while adding a Ogre3D Scene Node.");
				nFails++;
			}	
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for(i=0;i<lBufferedLightSources.count();i++)
	{
		try
		{
			lBufferedLightSources[i].pLightSource = new OgreLightNode(m_sceneManager->createLight(lBufferedLightSources[i].sName.toLocal8Bit().constData()));
			//lBufferedLightSources[i].pLightSource->setType(Ogre::Light::LT_POINT);//LT_DIRECTIONAL);
			lBufferedLightSources[i].pLightSource->setPosition(lBufferedLightSources[i].xPos, lBufferedLightSources[i].yPos, lBufferedLightSources[i].zPos);
			lBufferedLightSources[i].bAppended = true;
		}
		catch(std::exception &e) 
		{
			qDebug() << __FUNCTION__ << QString("Error while adding a Ogre3D Light Source %1").arg(e.what());
			nFails++;
		}
		catch(...)
		{
			qDebug() << __FUNCTION__ << ("Error while adding a Ogre3D Light Source.");
			nFails++;
		}	
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(cAmbientSceneColor)
	{
		try
		{
			m_sceneManager->setAmbientLight(Ogre::ColourValue(cAmbientSceneColor->redF(), cAmbientSceneColor->greenF(), cAmbientSceneColor->blueF(), cAmbientSceneColor->alphaF()));
		}
		catch(std::exception &e) 
		{
			qDebug() << __FUNCTION__ << QString("Error while setting the Ambient Scene Light %1").arg(e.what());
			nFails++;
		}
		catch(...)
		{
			qDebug() << __FUNCTION__ << ("Error while setting the Ambient Scene Light.");
			nFails++;
		}	
	}

	if(nFails > 0)
		return false;
	return true;
}

void OgreNode::preInit()
{
	restoreOgreState();
	if (!m_initialized)
		init();
	if (m_dirtyFBO) 
	{
		updateFBO();
		m_ogreFBO = getOgreFBO();
		m_dirtyFBO = false;
	}
	saveOgreState();
}

void OgreNode::init()
{
    const QOpenGLContext *ctx = QOpenGLContext::currentContext();
    QSurfaceFormat format = ctx->format();
    m_samples = format.samples();
	// STEP 1/ First, we will need to create the Ogre::Root object.
	// It is an object that must be created to use ogre correctly, and delete once we are finished using Ogre.
	// This is the name of an optional textual configuration file for the render system.
	// I won't use it.
	Ogre::String lConfigFileName = "";
	// This is the name of an optional textual configuration file, which lists the available plugins.
	// I won't use it.
	Ogre::String lPluginsFileName = "";
	// This is the name of the log file. A log file is a file in which you can write things during the program execution.
	// Ogre use it to display general informations about the render system.
	// You are not obliged to generate one, and ogre can even transmit the log data to you own class if you want.
	// Here we only ask the root to create the file.
	Ogre::String lLogFileName = "Ogre.log";
	// I create the root and I wrap it in an auto_ptr so that it will be automatically released.
	// Now I can even do "throw std::bad_alloc("bad alloc");", the program will release the root smoothly.
	//std::auto_ptr<Ogre::Root> lRoot(new Ogre::Root);//(lConfigFileName, lPluginsFileName, lLogFileName));
	//m_root = Ogre::Root::getSingleton().getSingletonPtr();

//#ifdef QT_NO_DEBUG
// #pragma comment( lib, "CEGUIBase.lib" )
// #pragma comment( lib, "OgreGUIRenderer.lib" )
// #pragma comment( lib, "OIS.lib" )
// #pragma comment( lib, "OgreMain.lib" )
//#else
//	OgreMain.lib
//		RenderSystem_GL.lib
 //#pragma comment( lib, "C:/Qt/Ogre/OGRE-SDK-1.8.2-vc100-x86-28.05.2013/lib/debug/OgreMain_d.lib" )
 //#pragma comment( lib, "C:/Qt/Ogre/OGRE-SDK-1.8.2-vc100-x86-28.05.2013/lib/debug/opt/RenderSystem_GL_d.lib" )
	//$(OGRE_HOME)/lib/RelWithDebInfo;$(OGRE_HOME)/lib/RelWithDebInfo/opt;
//#endif
		
	//m_root = Ogre::Root::getSingletonPtr();
	//if(m_root == NULL)
	//{
	m_root = new Ogre::Root(lPluginsFileName,lConfigFileName,lLogFileName);
	//}
	//else
	//{
		//m_root->shutdown();
		//delete m_root;
		//m_root = NULL;
		//m_root = new Ogre::Root(lPluginsFileName,lConfigFileName,lLogFileName);
	//}
	//m_root->reset(new Ogre::Root());//("plugins.cfg","ogre.cfg","Ogre.log");
	//m_root = new Ogre::Root(lPluginsFileName,lConfigFileName,lLogFileName);
	
	QString glPlugin;
	//QString glPlugin = QLatin1String(OGRE_PLUGIN_DIR);
	//glPlugin.remove("\"");
	//glPlugin = MainAppInfo::qmlExtensionsPluginDirPath() + "/" + OGRE3DITEM_PLUGINFOLDER_NAME;//"E:/Projects/BrainStim/Install/qml/plugins/Win32";
	glPlugin = MainAppInfo::appDirPath();//qmlExtensionsPluginDirPath() + "/" + OGRE3DITEM_PLUGINFOLDER_NAME;//"E:/Projects/BrainStim/Install/qml/plugins/Win32";
//#ifdef DEBUG_PLUGIN
#ifndef QT_NO_DEBUG
	glPlugin += QLatin1String("/RenderSystem_GL_d");
#else
	glPlugin += QLatin1String("/RenderSystem_GL");
#endif
	m_root->loadPlugin(glPlugin.toLatin1().constData());
	Ogre::RenderSystem *renderSystem = m_root->getRenderSystemByName("OpenGL Rendering Subsystem");
	m_root->setRenderSystem(renderSystem);
	m_root->initialise(false);
	Ogre::NameValuePairList params;
    params["externalGLControl"] = "true";
    params["currentGLContext"] = "true";
	params["border"] = "none";
    //Finally create our window.
    m_window = m_root->createRenderWindow("OgreWindow", 1, 1, false, &params);
    m_window->setVisible(false);
    m_window->update(false);
    // Setup scene
    m_sceneManager = m_root->createSceneManager(Ogre::ST_GENERIC, "mySceneManager");
    m_camera = m_sceneManager->createCamera("myCamera");
    m_camera->setNearClipDistance(0.1);
    m_camera->setFarClipDistance(99999);
    m_camera->setAspectRatio(Ogre::Real(m_size.width()) / Ogre::Real(m_size.height()));
    // Setup content...
	bool bResult = configureUserSettings();
    // Set a sky dome
    //m_sceneManager->setSkyBox(true, "SpaceSkyBox", 10000);
    // setup some basic lighting for our scene
    //m_sceneManager->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
    //m_sceneManager->createLight("myLight")->setPosition(20, 80, 50);	
    // Setup the camera
    m_cameraObject = new OgreCameraNode(m_camera);
    //m_cameraObject->camera()->setAutoTracking(true, m_sceneManager->getRootSceneNode());
	m_initialized = true;
}

bool OgreNode::setMaterialName(const QString &sEntityName, const QString &sMaterial)
{
	if(sEntityName.isEmpty() == false)
	{
		try
		{
			//Ogre::SceneNode *tmpSceneNode = Ogre::Root::getSingleton().getSceneManager("mySceneManager")->getSceneNode(sSceneNodeName.toLocal8Bit().constData());
			Ogre::Entity *tmpEntity = Ogre::Root::getSingleton().getSceneManager("mySceneManager")->getEntity(sEntityName.toLocal8Bit().constData());
			if(tmpEntity)
			{			
				tmpEntity->setMaterialName(sMaterial.toLocal8Bit().constData());
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