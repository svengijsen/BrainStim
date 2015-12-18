import QtQuick 2.0;
import BrainStim_QMLExtensions 1.0;

Rectangle 
{
	width: 800
	height: 800
	
	Text 
	{
		id: textID;
		text: "Hello World!";
		anchors.centerIn: parent;
		font.family: "Helvetica";
		font.pointSize: 40;
		font.bold: true;
		renderType: Text.NativeRendering;
	}
	MouseArea 
	{
		anchors.fill: parent;
		onClicked: 
		{
			Qt.quit();
		}
	}
	
	Item
	{
		id:functionsID
		objectName: "functions"
		function settext(text)
		{
			textID.text = text;
		}
	}
}