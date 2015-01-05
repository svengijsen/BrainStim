import QtQuick 2.0
import "../Common/QML/"

Rectangle 
{
	width: 720
	height: 720
	color: "green"
	
	Column //If used from within a Experimetn File then this Column section is used to show the Experiment Structure
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
			//console.log("setTextFunction() Got parameters:", sText)//only for debugging writes to stimulGL output file
			txtInfo.text = sText;
			return true;
		}
	}	
}