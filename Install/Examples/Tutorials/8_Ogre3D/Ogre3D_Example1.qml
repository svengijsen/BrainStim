import QtQuick 2.0
import BrainStim_QMLExtensions 1.0 as MyBrainStimExtensions // import types from the plugin
import BrainStim.Interface 1.0;

Rectangle 
{
	id: page
	color: "#FF00FF"
	
	MyBrainStimExtensions.OgreItem 
	{
		id: id_ogreitem
		width: parent.width/2;
		height: parent.height/2;
		anchors.centerIn: page
		Component.onCompleted: 
		{
			console.log("current working directory: " + BrainStimAPI.getCurrentDirectory());
			console.log("addResourceLocation: " + id_ogreitem.addResourceLocation(BrainStimAPI.getCurrentDirectory() + "/../Common/Meshes/ogre","FileSystem"));
			console.log("createEntity: " + id_ogreitem.createEntity("EntityHead1", "ogrehead.mesh"));
			console.log("createSceneNode: " + id_ogreitem.createSceneNode("SceneNodeHead1", "EntityHead1",0,0,0));
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
