import QtQuick 2.0
//import "../../Common/QML/"
import BrainStim_QMLExtensions 1.0

Rectangle
{
	id: rootRect
	color: "#000000" //Make it black
	
	Column
	{
		x: 56
		y: 23
		width: 311
		height: 39
		spacing: 10
		DebugMode {}//Present in DebugMode.qml!
	}

	Text
	{
		id: imageSourceInfo
		text: "<undefined>"
		anchors.bottom: parent.bottom 
		anchors.horizontalCenter: parent.horizontalCenter
		font.pointSize: 20
		color: "white"
		renderType: Text.NativeRendering
	}
	
	Text
	{
		id: fixationText
		color: "red"
		text: "+"
		font.bold: true
		anchors.centerIn: parent
		font.pointSize: 20
		renderType: Text.NativeRendering
	}
	
	Image
	{
		id: imageElement
		anchors.centerIn: parent
		fillMode: Image.PreserveAspectFit
		source: ""
	}

	Item
	{
		id: functionsID
		objectName: "functions"
		function setImageFunction(path)
		{
			console.log("*** setImageFunction() Got message():", path)
			if(path!="fixation")
			{
				imageElement.visible = true;
				imageElement.source = "image://DefaultImageBuffer/" + path;
				imageSourceInfo.text = imageElement.source.toString().substring(imageElement.source.toString().lastIndexOf("/")+1)
			}
			else
			{
				imageElement.visible = false;
				imageSourceInfo.text = "fixation"
			}
			return true;
		}
	}
}
