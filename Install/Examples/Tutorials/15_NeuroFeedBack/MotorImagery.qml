import QtQuick 2.0
import StimulGL.Modules 1.0

Rectangle
{
	id: page
	objectName: "rootObjectName"	
	color: "black" //Make it transparant

	Column
	{
		x: 200
		y: 0
		width: 311
		height: 39
		spacing: 10
		DebugMode{}//Present in DebugMode.qml!
	}

	CustomProgressBar 
	{
		id: progress
		x: page.width / 2 - width/2
		y: page.height / 4
		//anchors.centerIn: progressSpinner
		width: page.height / 5
		height: page.height / 2
		color: "yellow"
		value: 0.8
	}

	Item
	{
		id: functionsID
		objectName: "functions"
		function setTextAndBarLevelFunction(textString, nValue)
		{
			if(nValue < 0 || nValue > 1)
				nValue = 0;
			if(textString === "Feedback")
			{
				progress.value = nValue;
				progress.color = "green";
			}
			else if(textString === "Rest")
			{
				progress.value = nValue;
				progress.color = "yellow";
			}
			else if(textString === "Update")
			{
				progress.value = nValue;
			}
			console.log(textString, " ", nValue);			
			return true;
		}
	}

//just for testing purpose...	
//	Item 
//	{
//		anchors.fill: parent
//		focus: true
//		Keys.onPressed: 
//		{
//			if (event.key == Qt.Key_Left) 
//			{
//				functionsID.setTextAndBarLevelFunction("Feedback",0.25);
//			}
//			else if (event.key == Qt.Key_Right) 
//			{
//				functionsID.setTextAndBarLevelFunction("Rest",0.75);
//			}
//			else if (event.key == Qt.Key_Up) 
//			{
//				functionsID.setTextAndBarLevelFunction("Update",0.5);
//			}			
//		}
//	}	
}
