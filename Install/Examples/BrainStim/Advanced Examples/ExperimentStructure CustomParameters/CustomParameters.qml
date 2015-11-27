import QtQuick 2.0
import BrainStim_QMLExtensions 1.0

Rectangle 
{
	color: "green"
	
	Column
	{
		x: 16;y: 27;width: 87;height: 93;spacing: 10
		DebugMode{}
	}	

	Text 
	{
		id: txtInfo1
		text: qsTr("Default text1")
		x: parent.width/4
		anchors.verticalCenter: parent.verticalCenter
		font.family: "Helvetica"
		font.pointSize: 20
		font.bold: true
		renderType: Text.NativeRendering
	}

	Text 
	{
		id: txtInfo2
		text: qsTr("Default text2")
		anchors.top: txtInfo1.bottom
		anchors.left: txtInfo1.left
		font.family: "Helvetica"
		font.pointSize: 20
		font.bold: true
		renderType: Text.NativeRendering
	}

	Text 
	{
		id: txtInfo3
		text: qsTr("Default text3")
		anchors.top: txtInfo2.bottom
		anchors.left : txtInfo2.left
		font.family: "Helvetica"
		font.pointSize: 20
		font.bold: true
		renderType: Text.NativeRendering
	}	
	
	Item
	{
		id: functionsID
		objectName: "functions"
		function setTextFunction(sText1,sText2,sText3)
		{
			console.log("setTextFunction() Got parameters:" + sText1 + ", " + sText2 + ", " + sText3);
			txtInfo1.text = sText1;
			txtInfo2.text = sText2;
			txtInfo3.text = sText3;
			return true;
		}
	}	
}