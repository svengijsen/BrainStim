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

#ifndef OGRENODE_H
#define OGRENODE_H

#include "Ogre.h"
#include <QtQuick/QSGGeometryNode>
#include <QtQuick/QSGTextureMaterial>
#include <QtQuick/QSGOpaqueTextureMaterial>
#include <QtQuick/QQuickWindow>
#include <QMutex>

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define OGRE_PLUGIN_DIR STRINGIFY(OGRE_PLUGIN_VAR)

namespace Ogre {
	class Root;
	class Camera;
	class SceneManager;
	class RenderTexture;
	class Viewport;
	class RenderTarget;
}

class OgreCameraNode;
class OgreLightNode;

typedef struct strcOgreResources
{
	QString sType;
	QString sLocation;
} sTypeOgreResourcesStructure;

typedef struct strcEntity
{
	Ogre::Entity* pEntity;
	QString sName;
	QString sMesh;
} sEntityStructure;

typedef struct strcSceneNode
{
	Ogre::SceneNode* pSceneNode;
	QString sName;
	QString sEntityName;
	float xPos;
	float yPos;
	float zPos;
} sSceneNodeStructure;

typedef struct strcLightSource
{
	OgreLightNode* pLightSource;
	QString sName;
	float xPos;
	float yPos;
	float zPos;
	bool bAppended;
	strcLightSource()
	{
		pLightSource = NULL;
		sName = "";
		xPos = 0.0;
		yPos = 0.0;
		zPos = 0.0;
		bAppended = false;
	}
} sLightSourceStructure;

class OgreNode : public QSGGeometryNode
{

public:
    OgreNode();
    ~OgreNode();

    void setSize(const QSize &size);
    QSize size() const { return m_size; }
    void setQuickWindow(QQuickWindow *window);
	void setAAEnabled(bool enable);
    OgreCameraNode *camera() const { return m_cameraObject; }
    void update();
    void updateFBO();
    void init();
	void preInit();
    GLuint getOgreFBO();
    void saveOgreState();
    void restoreOgreState();
    void preprocess();

	bool setResourceLocations(const QList<sTypeOgreResourcesStructure> lResources);
	bool setEntities(const QList<sEntityStructure> lEntities);
	bool setSceneNodes(const QList<sSceneNodeStructure> lSceneNodes);
	bool setAmbientLight(const QColor &cColor);
	bool setLightSources(const QList<sLightSourceStructure> lLightSources);
	bool appendLightSource(sLightSourceStructure &lLightSource);
	OgreLightNode *getLightSource(const QString &sLightName);

	public slots:
		bool setMaterialName(const QString &sEntityName, const QString &sMaterial);

private:
	bool configureUserSettings();

	QList<sTypeOgreResourcesStructure> lBufferedResources;	
	QList<sEntityStructure> lBufferedEntities;
	QList<sSceneNodeStructure> lBufferedSceneNodes;
	QList<sLightSourceStructure> lBufferedLightSources;
	QColor *cAmbientSceneColor;

    QSGTextureMaterial m_material;
    QSGOpaqueTextureMaterial m_materialO;
    QSGGeometry m_geometry;
    QSGTexture *m_texture;
    QQuickWindow *m_quickWindow;
    /** Pointer to QOpenGLContext to be used by Ogre. */
    QOpenGLContext* m_ogreContext;
    /** Pointer to QOpenGLContext to be restored after Ogre context. */
    QOpenGLContext* m_qtContext;

    int m_samples;
    bool m_AAEnabled;
    QSize m_size;

    Ogre::Root *m_root;
    Ogre::Camera *m_camera;
    Ogre::SceneManager *m_sceneManager;
    Ogre::RenderTexture *m_renderTexture;
    Ogre::Viewport *m_viewport;
    Ogre::TexturePtr rtt_texture;
    Ogre::RenderWindow *m_window;

    GLuint m_ogreFBO;
    OgreCameraNode *m_cameraObject;

    bool m_initialized;
    bool m_dirtyFBO;
};

#endif // OGRENODE_H
