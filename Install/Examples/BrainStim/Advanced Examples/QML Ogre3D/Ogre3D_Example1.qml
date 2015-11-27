import QtQuick 2.0
import BrainStim_QMLExtensions 1.0

Rectangle 
{
	id: page
	color: "#222222"
	
	Column
	{
		x: 16;y: 27;width: 87;height: 93;spacing: 10
		DebugMode{}
	}

	Text 
	{
		id: txtFixation
		text: "+"
		color: "white"
		anchors.centerIn: parent
		font.family: "Helvetica"
		font.pointSize: 20
		font.bold: true
		renderType: Text.NativeRendering
	}	

	OgreItem 
	{
		id: id_ogreitem
		visible: false
		width: parent.width/2;
		height: parent.height/2;
		anchors.centerIn: page
		Component.onCompleted: 
		{
			console.log("current working directory: " + BrainStimAPI.getCurrentDirectory());
			console.log("addResourceLocation: " + id_ogreitem.addResourceLocation(BrainStimAPI.getCurrentDirectory() + "/../../Common/Meshes/ogre","FileSystem"));
			console.log("createEntity: " + id_ogreitem.createEntity("EntityHead1", "ogrehead.mesh"));
			console.log("createSceneNode: " + id_ogreitem.createSceneNode("SceneNodeHead1", "EntityHead1",0,0,0));
		}	
	}
	
	Item
	{
		id: functionsID
		objectName: "functions"
		function showOgre3DItem(bShow)
		{
			id_ogreitem.visible = bShow;
			console.log("showOgre3DItem() Got parameter:" + bShow);
			return;
		}
	}
	
	Item 
	{
		anchors.fill: parent
		focus: true
		Keys.onPressed: 
		{
			if (event.key == Qt.Key_S) 
			{
				functionsID.showOgre3DItem(true);
				event.accepted = true;
			}
			else if (event.key == Qt.Key_H) 
			{
				functionsID.showOgre3DItem(false);
				event.accepted = true;
			}
		}
	}

	MouseArea 
	{
		id: buttonMouseArea
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
