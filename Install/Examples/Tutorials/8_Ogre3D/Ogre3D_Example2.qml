import QtQuick 2.0
import BrainStim_QMLExtensions 1.0 as MyBrainStimExtensions // import types from the plugin
import "../Common/QML/"

Rectangle 
{
	id: page
	property bool bOgre3DItemLoaded: false
	//width: 1680
	//height: 1050
	color: "black"

	Rectangle 
	{
		id: id_OgreViewPort
		property alias ogreLoader: id_OgreComponentLoader
		property alias fixation: id_Fixation
		anchors.fill: parent
		color: "black"
		
		Loader 
		{ 
			id: id_OgreComponentLoader
			property alias resourceLocations: id_resourceLocations
			property alias mesh3DModels: id_mesh3DModels			
			ListModel 
			{
				id: id_resourceLocations			
			}
			ListModel 
			{
				id: id_mesh3DModels
				property var currentNode: ""
				property var currentEntity: ""
			}
			anchors.fill: parent		
			onStatusChanged: 
			{
				if (id_OgreViewPort.ogreLoader.status == Loader.Ready) 
				{
					console.log("OGRE3D Loaded!");
					page.bOgre3DItemLoaded = true;
				}
			}
		}
		
		Rectangle 
		{
			id: id_Fixation
			anchors.fill: parent
			color: parent.color
			visible: false
			
			Text
			{
				id: id_FixationText
				anchors.centerIn: parent
				text: "+"
				font.bold: true
				font.family: "Helvetica"
				font.pointSize: 26
				color: "white"
				visible: true
				renderType: Text.NativeRendering
			}	
		}		
	}

	Rectangle
	{
		anchors.top: id_OgreViewPort.top
		color: "#00000000"
		width: 300
		height: 40
		DebugMode{}//Present in DebugMode.qml!
	}	

	SequentialAnimation
	{
		id: seqAnimationYaw
		property real startFaceAngle: 0;
		property real animationAngleSpan: 10;
		property real animationDuration: 1000;		
		running: false
		NumberAnimation 
		{ 
			id: id_OgreNumberAnimation
			target: id_OgreViewPort.ogreLoader.item
			property: "camera.yaw"
			from: seqAnimationYaw.startFaceAngle
			to: seqAnimationYaw.startFaceAngle + seqAnimationYaw.animationAngleSpan
			duration: seqAnimationYaw.animationDuration 
		}
		ScriptAction 
		{ 
			script: 
			{
				//id_OgreViewPort.ogreLoader.item.setObjectVisibility(id_OgreViewPort.ogreLoader.mesh3DModels.currentNode,id_OgreViewPort.ogreLoader.mesh3DModels.currentEntity,false);
				id_OgreViewPort.fixation.visible = true;
			}
		}		
	}

	Component 
	{
		id: id_OgreComponent
		MyBrainStimExtensions.OgreItem 
		{
			id: id_ogreitem
			property int nCamDistance: 15
			property int nObjectDistance: 15
			
			Component.onCompleted: 
			{
				var i = 0;
				for(i=0;i<id_OgreViewPort.ogreLoader.resourceLocations.count;i++)
				{
					console.log("resourceLocations.count= " + id_OgreViewPort.ogreLoader.resourceLocations.count);
					console.log(addResourceLocation(id_OgreViewPort.ogreLoader.resourceLocations.get(i).path,id_OgreViewPort.ogreLoader.resourceLocations.get(i).type));
				}
				console.log("setAmbientColor() returned: " + setAmbientColor(76,76,76,255));
				for(i=0;i<id_OgreViewPort.ogreLoader.mesh3DModels.count;i++)
				{
					console.log("createEntity returned: " + createEntity(id_OgreViewPort.ogreLoader.mesh3DModels.get(i).entity, id_OgreViewPort.ogreLoader.mesh3DModels.get(i).mesh));
					console.log("createSceneNode returned: " + createSceneNode(id_OgreViewPort.ogreLoader.mesh3DModels.get(i).node, id_OgreViewPort.ogreLoader.mesh3DModels.get(i).entity,0,(i)*nObjectDistance,0));
					//console.log("createLightSource() returned: " + createLightSource("sLightName" + i, 0, 0, nCamDistance * -5));//(i)*nObjectDistance, nCamDistance * 5));//0, 0, 0));	
				}				
				console.log("createLightSource() returned: " + createLightSource("sLightName0", 0, 0, nCamDistance * 250));//0, 0, nCamDistance * 5));//0, 0, 0));			
			}
			onOgreNodeInitialized:
			{
				console.log("onOgreNodeInitialized- BEGIN");
				//for(var i=0;i<id_OgreViewPort.ogreLoader.mesh3DModels.count;i++)
				//{
				//	id_ogreitem.setObjectVisibility(id_OgreViewPort.ogreLoader.mesh3DModels.get(i).node,id_OgreViewPort.ogreLoader.mesh3DModels.get(i).entity,false);	
				//}
				id_OgreViewPort.fixation.visible = true;
				if(id_OgreViewPort.ogreLoader.mesh3DModels.count > 0)
					functionsID.trackObject(id_OgreViewPort.ogreLoader.mesh3DModels.get(0).node,id_OgreViewPort.ogreLoader.mesh3DModels.get(0).entity);
				console.log("onOgreNodeInitialized- END");
			}
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
					id_OgreViewPort.ogreLoader.item.camera.yaw -= (mouse.x - prevX) / 2
				if (prevY > -1)
					id_OgreViewPort.ogreLoader.item.camera.pitch -= (mouse.y - prevY) / 2
				prevX = mouse.x
				prevY = mouse.y
			}
			if (pressedButtons & Qt.RightButton) 
			{
				if (prevY > -1)
					id_OgreViewPort.ogreLoader.item.camera.zoom = Math.min(6, Math.max(0.1, id_OgreViewPort.ogreLoader.item.camera.zoom - (mouse.y - prevY) / 100));
				prevY = mouse.y
			}
		}
		onReleased: 
		{ 
			prevX = -1 
			prevY = -1 
			//toptoolbartext.text = "camera position:%1".arg(id_OgreViewPort.ogreLoader.item.camera.position);
		}		
	}
	
//	Item
//	{
//		id: keyHandler
//		anchors.fill: parent
//		focus: true
//		Keys.onPressed:
//		{
//			if (event.key === Qt.Key_X)
//			{
//				if (event.modifiers & Qt.ShiftModifier) {
//					console.log("Shift + Key_X pressed");
//					id_OgreViewPort.ogreLoader.item.camera.position.x = id_OgreViewPort.ogreLoader.item.camera.position.x - 100;
//				} else {
//					console.log("Key_X pressed");
//					id_OgreViewPort.ogreLoader.item.camera.position.x = id_OgreViewPort.ogreLoader.item.camera.position.x + 100;
//				}
//				toptoolbartext.text = "camera position:%1".arg(id_OgreViewPort.ogreLoader.item.camera.position);
//				event.accepted = true;
//			}
//			else if (event.key === Qt.Key_Y)
//			{
//				if (event.modifiers & Qt.ShiftModifier) {
//					console.log("Shift + Key_Y pressed");
//					id_OgreViewPort.ogreLoader.item.camera.position.y = id_OgreViewPort.ogreLoader.item.camera.position.y - 100;
//				} else {
//					console.log("Key_Y pressed");
//					id_OgreViewPort.ogreLoader.item.camera.position.y = id_OgreViewPort.ogreLoader.item.camera.position.y + 100;					
//				}				
//				toptoolbartext.text = "camera position:%1".arg(id_OgreViewPort.ogreLoader.item.camera.position);
//				event.accepted = true;
//			}
//			else if (event.key === Qt.Key_Z)
//			{
//				if (event.modifiers & Qt.ShiftModifier) {
//					console.log("Shift + Key_Z pressed");
//					id_OgreViewPort.ogreLoader.item.camera.position.z = id_OgreViewPort.ogreLoader.item.camera.position.z - 100;
//				} else {
//					console.log("Key_Z pressed");
//					id_OgreViewPort.ogreLoader.item.camera.position.z = id_OgreViewPort.ogreLoader.item.camera.position.z + 100;					
//				}				
//				toptoolbartext.text = "camera position:%1".arg(id_OgreViewPort.ogreLoader.item.camera.position);
//				event.accepted = true;
//			}
//			else if (event.key === Qt.Key_1)
//			{
//				functionsID.trackObject("SceneNodeNormalHead1","EntityHeadNormal1");	
//				event.accepted = true;
//			}								
//			else if (event.key === Qt.Key_2)
//			{
//				functionsID.trackObject("SceneNodeNormalHead2","EntityHeadNormal2");	
//				event.accepted = true;
//			}
//			else if (event.key === Qt.Key_3)
//			{
//				functionsID.trackObject("SceneNodeNormalHead3","EntityHeadNormal3");	
//				event.accepted = true;
//			}
//			else if (event.key === Qt.Key_4)
//			{
//				functionsID.trackObject("SceneNodeNormalHead4","EntityHeadNormal4");	
//				event.accepted = true;
//			}								
//			else if (event.key === Qt.Key_5)
//			{
//				functionsID.trackObject("SceneNodeNormalHead5","EntityHeadNormal5");	
//				event.accepted = true;
//			}
//			else if (event.key === Qt.Key_6)
//			{
//				functionsID.trackObject("SceneNodeNormalHead6","EntityHeadNormal6");	
//				event.accepted = true;
//			}
//			else if (event.key === Qt.Key_7)
//			{
//				functionsID.trackObject("SceneNodeNormalHead7","EntityHeadNormal7");	
//				event.accepted = true;
//			}								
//			else if (event.key === Qt.Key_8)
//			{
//				functionsID.trackObject("SceneNodeNormalHead8","EntityHeadNormal8");	
//				event.accepted = true;
//			}
//			else if (event.key === Qt.Key_9)
//			{
//				functionsID.trackObject("SceneNodeNormalHead9","EntityHeadNormal9");	
//				event.accepted = true;
//			}
			//else if (event.key === Qt.Key_I)
			//{
				//seqAnimationYaw.restart();
				//functionsID.set3DMeshAnimation("SceneNodeNormalHead9",40,50,2000);
				//event.accepted = true;
			//}
//			else if (event.key === Qt.Key_C)
//			{
//				if(page.bOgre3DItemLoaded == false)
//				{
					//Qt.createComponent(id_OgreComponent);
//					functionsID.addOgreResourceLocation("E:\\projects\\experiments\\.....l","FileSystem");
//					functionsID.addOgreMesh3DModel("SceneNodeNormalHead1","EntityHeadNormal1","face_01n.mesh");
//					id_OgreViewPort.ogreLoader.sourceComponent = id_OgreComponent;			
//					event.accepted = true;
//				}
//			}
//		}
//	}	

	Item
	{
		id: functionsID
		objectName: "functions"
		function trackObject(sceneNodeName,entityName)
		{
			id_OgreViewPort.ogreLoader.mesh3DModels.currentNode = sceneNodeName;
			id_OgreViewPort.ogreLoader.mesh3DModels.currentEntity = entityName;
			var tmpVectorCenter = id_OgreViewPort.ogreLoader.item.getObjectBoundingBoxCenter(sceneNodeName,entityName);
			var tmpVectorSize = id_OgreViewPort.ogreLoader.item.getObjectBoundingBoxSize(sceneNodeName,entityName);
			tmpVectorCenter.z = tmpVectorCenter.z + id_OgreViewPort.ogreLoader.item.nCamDistance;
			id_OgreViewPort.ogreLoader.item.camera.position = tmpVectorCenter;
			id_OgreViewPort.ogreLoader.item.camera.setAutoTracking(true,sceneNodeName, Qt.vector3d(0,tmpVectorSize.y/2,0));	
			return true;
		}
		function set3DMeshAnimation(nodeName,entityName,startFaceAngle,animationAngleSpan,animationDuration)
		{
			if(nodeName == "initialize")
			{
				if(page.bOgre3DItemLoaded == false)
					id_OgreViewPort.ogreLoader.sourceComponent = id_OgreComponent;				
			}
			else
			{
				if(id_OgreViewPort.ogreLoader.mesh3DModels.count > 0)
				{
					//id_OgreViewPort.ogreLoader.item.setObjectVisibility(nodeName,entityName,false); 
					
					trackObject(nodeName,entityName);					
					seqAnimationYaw.startFaceAngle = startFaceAngle;
					seqAnimationYaw.animationAngleSpan = animationAngleSpan;
					seqAnimationYaw.animationDuration = animationDuration;				
					id_OgreViewPort.fixation.visible = false;
					seqAnimationYaw.restart();	
					
					//id_OgreViewPort.ogreLoader.item.setObjectVisibility(nodeName,entityName,true); 
				}
			}
			return true;
		}
		function addOgreResourceLocation(sPath,sType)
		{		
			id_OgreViewPort.ogreLoader.resourceLocations.append({"path":sPath, "type":sType});
			console.log("addOgreResourceLocation() executed.");
			return true;
		}
		function addOgreMesh3DModel(sSceneName,sEntityName,sMeshFileName)
		{	
			var lastPositionIndex = -1;
			id_OgreViewPort.ogreLoader.mesh3DModels.append({"node":sSceneName, "entity":sEntityName, "mesh":sMeshFileName});
			console.log("addOgreMesh3DModel() executed(" + "scene:" + sSceneName + ", entity:" + sEntityName + ", mesh:" + sMeshFileName + ").");
			lastPositionIndex = (id_OgreViewPort.ogreLoader.mesh3DModels.count) - 1;
			return lastPositionIndex;  //Return the index
		}
//		function setOgreAmbientSceneLight(fRed,fGreen, fBlue, fAlpha)
//		{
//			return id_OgreViewPort.ogreLoader.item.setAmbientColor(fRed,fGreen, fBlue, fAlpha);
//		}
//		function addOgreLightSource(sLightName, fXPos, fYPos, fZPos)
//		{			
//			return id_OgreViewPort.ogreLoader.item.createLightSource(sLightName, fXPos, fYPos, fZPos);
//		}
	}	
}
