import QtQuick 2.0
import BrainStim.Modules 1.0

Rectangle 
{
	color: "grey"
	
	Column
	{
		x: 16;y: 27;width: 87;height: 93;spacing: 10
		DebugMode{}
	}	

	Text 
	{
		id: txtInfo
		text: qsTr("Default text")
		anchors.centerIn: parent
		font.family: "Helvetica"
		font.pointSize: 20
		font.bold: true
		renderType: Text.NativeRendering
	}
	
	Item
	{
		id: functionsID
		objectName: "functions"
		function setTextFunction(sText)
		{
			txtInfo.text = sText;
			return true;
		}
	}	
}