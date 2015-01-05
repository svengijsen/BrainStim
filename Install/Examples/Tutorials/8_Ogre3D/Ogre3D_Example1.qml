import QtQuick 2.0
import StimulGL_QMLExtensions 1.2 as MyStimulGLExtensions // import types from the plugin

Rectangle 
{
	id: page
	width: 640; height: 480
	color: "#FF00FF"
	
	MyStimulGLExtensions.OgreItem 
	{
		id: id_ogreitem
		width: 1680; height: 1050
		Component.onCompleted: 
		{
			console.log(id_ogreitem.addResourceLocation("E:/projects/StimulGL/Install/examples/Tutorials/Common/Meshes/ogre","FileSystem"));
			console.log(id_ogreitem.createEntity("EntityHead1", "ogrehead.mesh"));
			console.log(id_ogreitem.createSceneNode("SceneNodeHead1", "EntityHead1",0,0,0));
		}		
	}

	MouseArea 
	{
		anchors.fill: parent
		acceptedButtons: Qt.LeftButton | Qt.RightButton
		property int prevX: -1
		property int prevY: -1
		onPositionChanged: 
		{
			if (pressedButtons & Qt.LeftButton) 
			{
				if (prevX > -1)
					id_ogreitem.camera.yaw -= (mouse.x - prevX) / 2
				if (prevY > -1)
					id_ogreitem.camera.pitch -= (mouse.y - prevY) / 2
				prevX = mouse.x
				prevY = mouse.y
			}
			if (pressedButtons & Qt.RightButton) 
			{
				if (prevY > -1)
					id_ogreitem.camera.zoom = Math.min(6, Math.max(0.1, id_ogreitem.camera.zoom - (mouse.y - prevY) / 100));
				prevY = mouse.y
			}
		}
		onReleased: 
		{ 
			prevX = -1 
			prevY = -1 
		}		
	}
}
