//This is a basic minimal QML example file...
import QtQuick 2.0

Rectangle								//The outer rectangle item containing everything
{
	id: idMainRectangle					//The rectangle's unique identifier
	color: "blue" 						//Blue background color

	Text 								//A text item
	{
		id: idInformationText				//The text's item unique identifier
		x: 100; y: 100 					//The position on the screen
		width: 200; height: 50 				//The text size
		text: "Hello world" 				//The text string to show
		font.bold: true 					//The text bold style
		font.pixelSize: 40 				//The text pixel size
		color: "white"					//The text color
		renderType: Text.NativeRendering		//The text renderType, with some graphic cards the default OpenGL text rendering gives terrible results, therefore it's set to native (software) rendering.
	}

	Rectangle //Another rectangle item
	{
		id: idSmallRectangle
		anchors.left: idInformationText.left		//Anchors provide a way to position an item by specifying its relationship with other items. Now the left position is anchored to the 'left' property of the item with the ID 'idInformationText'
		anchors.top: idInformationText.bottom	//Now the top position is anchored to the 'bottom' property of the item with the ID 'idInformationText'
		width: 200
		height: 100
		color: "#ff0000"					//The rectangles color (as RGB value)
	}

	MouseArea 							//A Mouse Area item, that let's us capture mouse events and make it interactive
	{
		anchors.fill: parent 				//Here we make sure that this item fills (captures) the whole parent item's (--> 'idMainRectangle') area 
		onClicked: 
		{
			console.log("Mouse Clicked!") 	//A way of sending text to the BrainStim Output Log Pane, used for debugging and testing purpose
			Qt.quit();					//Call the 'Qt.quit()' function to automatically exit
		}
	}	
}
