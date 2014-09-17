import QtQuick 2.0

Rectangle 
{
	id: debugModeFrame
	//property alias text: textItem.text
	property int nLastLoopValue: -1
	width: 300; height: 30
	color: "#00ffffff"
	visible: false

	Text 
	{
		id: textItem
		anchors.centerIn: parent
		font.pointSize: 14
		color: "#c025a4"
		text: "Block 0, Trial 0, Trigger 0"	
		renderType: Text.NativeRendering
	}

	Item 
	{
		id: functionsID
		objectName: "DebugMode_Functions"
		function updateExperimentState(block,trial,trigger) 
		{
			//console.log("Got message(myQmlFunction):", msg)
			//if(loop >= 0)
			//{
			//	nLastLoopValue = loop;
			//}
			//if(nLastLoopValue!= -1)
			//	textItem.text =  "Block " + block + ", Trial " + trial + ", Trigger " + trigger + ", Loop " + nLastLoopValue;
			//else
				textItem.text =  "Block " + block + ", Trial " + trial + ", Trigger " + trigger;
			return true //"some return value"
		}
		function enableDebugMode()
		{
			debugModeFrame.visible = true;
			return true
		}
	}
}
