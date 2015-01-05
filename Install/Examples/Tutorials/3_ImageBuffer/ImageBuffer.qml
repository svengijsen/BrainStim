import QtQuick 2.0
import "../Common/QML/"

Rectangle
{
	id: page
	width: 500; height: 500
	color: "#0000FF00" //Make it transparant
	//border.width: 10
	//border.color: "#e04343" //or use text like "lightgray"

	Text
	{
		id: someText
		x: 11
		y: 414
		width: 480
		height: 76
		text: "Some Text..."
		wrapMode: Text.WrapAtWordBoundaryOrAnywhere
		font.bold: true
		verticalAlignment: Text.AlignTop
		horizontalAlignment: Text.AlignLeft
		font.family: "Helvetica"
		font.pointSize: 12
		color: "red"
		renderType: Text.NativeRendering
	}

	Item
	{
		id: functionsID
		objectName: "functions"
		function setImageFunction(path)
		{
			//console.log("*** setImageFunction() Got message():", path)//only for debugging writes to stimulGL output file
			var tmpString = new String(path)
			if(!tmpString.match("fixation"))
			{
				image1.visible = true;
				image1.source = "image://DefaultImageBuffer/" + path
				someText.text = image1.source.toString().substring(image1.source.toString().lastIndexOf("/")+1)
			}
			else
			{
				image1.visible = false;
				someText.text = "fixation"
			}
			return true;
		}
	}

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
		id: text1
		color: "#ff0000"
		text: qsTr("+")
		styleColor: "#ff0000"
		font.bold: true
		anchors.fill: parent
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
		font.pixelSize: 30
		renderType: Text.NativeRendering
	}

	Image
	{
		id: image1
		x: 56
		y: 82
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		fillMode: Image.PreserveAspectFit
		//        sourceSize.width: 459
		//        sourceSize.height: 373
		//        smooth: true
		//        fillMode: Image.PreserveAspectFit
		//        asynchronous: true
		source: ""
	}
}
