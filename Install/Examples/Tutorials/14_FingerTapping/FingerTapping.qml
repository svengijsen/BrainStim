import QtQuick 2.0
import StimulGL.Modules 1.0

Rectangle
{
	id: page
	objectName: "rootObjectName"	
	color: "black"
	
	property int nFlickrFrequency: 2
	property int nFlickrFrequencyTextXOffset: 200

	Column
	{
		anchors.top: parent.top
		anchors.horizontalCenter: parent.horizontalCenter
		spacing: 10
		DebugMode{}//Present in DebugMode.qml!
	}
	
	Text
	{
		id: centerText
		anchors.centerIn: parent
		text: "+"
		font.bold: true
		font.family: "Helvetica"
		font.pointSize: 30
		color: "white"
		renderType: Text.NativeRendering
	}
	
	Text
	{
		id: flickrFingerTappingText
		anchors.verticalCenter: centerText.verticalCenter
		text: "R|L"
		font.bold: true
		font.family: "Helvetica"
		font.pointSize: 30
		color: "red"
		renderType: Text.NativeRendering
		visible: false
	}
	
	SequentialAnimation 
	{
		id: changeFlickrFingerTappingText
		running: false
		
		SequentialAnimation {
		     PropertyAction { target: flickrFingerTappingText; property: "visible"; value: "true" }
		     PropertyAction { target: centerImage; property: "visible"; value: "true" }
		     PauseAnimation { duration: 1000 / (2*nFlickrFrequency) }
		     PropertyAction { target: flickrFingerTappingText; property: "visible"; value: "false" }
		     PropertyAction { target: centerImage; property: "visible"; value: "false" }
		     PauseAnimation { duration: 1000 / (2*nFlickrFrequency) }
		 }
		
		loops: Animation.Infinite
	}	
	
	Image
	{
		id: centerImage
		anchors.bottom: centerText.top
		anchors.horizontalCenter: centerText.horizontalCenter
		source: ""
	}

	Item
	{
		id: functionsID
		objectName: "functions"
		function setCenterText(sText)
		{
			centerText.text = sText;			
			return true;
		}	
	
		function setImageFile(sImageFilePath)
		{
			centerImage.source = sImageFilePath;		
			return true;
		}	
		
		function setFingerFlickrAnimation(sText)
		{	
			if(sText == "left")
			{
				//console.log("move left");
				changeFlickrFingerTappingText.start();
				flickrFingerTappingText.x = centerText.x - nFlickrFrequencyTextXOffset;
				flickrFingerTappingText.text = "L";			
			}
			else if(sText == "right")
			{
				//console.log("move right");
				changeFlickrFingerTappingText.start();
				flickrFingerTappingText.x = centerText.x + nFlickrFrequencyTextXOffset;
				flickrFingerTappingText.text = "R";		
			}
			else if(sText == "")
			{
				//console.log("stop");
				flickrFingerTappingText.visible = false;
				centerImage.visible = true;
				changeFlickrFingerTappingText.stop();
				flickrFingerTappingText.x = centerText.x;
				flickrFingerTappingText.text = "L|R";				
			}
			else
			{
				return false;
			}
			return true;
		}
		
	}
	
//	Item 
//	{
//		anchors.fill: parent
//		focus: true
//		Keys.onPressed: 
//		{
//			if (event.key == Qt.Key_Left) 
//			{
//				functionsID.setFingerFlickrAnimation("left");
//			}
//			else if (event.key == Qt.Key_Right) 
//			{
//				functionsID.setFingerFlickrAnimation("right");
//			}
//			else if (event.key == Qt.Key_Up) 
//			{
//				functionsID.setFingerFlickrAnimation("");
//			}
//		}
//	}	
}
