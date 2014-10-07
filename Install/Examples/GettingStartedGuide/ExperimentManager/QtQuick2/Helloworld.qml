import QtQuick 2.0
//Here's some comment...

Rectangle
{
	id: rectangle_root					//The rectangle's item ID
	width: 600;	height: 800 
	/*
	This is the size of the (root) Rectangle item. 
	This value is set to (600:800) but when running this file directly from BrainStim it's automatically maximized to full screen so the set value is ignored.
	When this file is run from within the ExperimentManager then the set size is preserved.
	*/	
	color: "#ffff00" //Yellow background color (RGB-value)

	Text //A text item
	{
		id: text1 						//The text's item ID
		x: 100; y: 100 					//The position on the screen
		width: 200; height: 50 			//The text size
		text: "Hello world" 				//The text string to show
		font.bold: true 					//The text bold style
		font.pixelSize: 20 				//The text pixel size
		renderType: Text.NativeRendering	//The text renderType, with some graphic cards the default OpenGL text rendering gives terrible results, therefore it's set to native (software) rendering, try to comment this line...
	}

	Rectangle //A rectangle item
	{
		id: rectangle1
		anchors.left: text1.left				//Anchors provide a way to position an item by specifying its relationship with other items. Now the left position is anchored to the 'left' property of the item with the ID 'text1'
		anchors.top: text1.bottom
		width: 200
		height: 100
		color: "#ff0000"
	}

	Rectangle  //Another rectangle item
	{
		id: rectangle2
		anchors.left: rectangle1.right
		anchors.top: rectangle1.top
		width: rectangle1.width
		height: rectangle1.height / 2			//Let's make the 'height' property the 'height' property value of the item with the ID 'rectangle1' and divide that value by 2
		color: "blue"					//Set the color by using it's name
	}
	
	MouseArea //A Mouse Area item, that let's us capture mouse events
	{
		anchors.fill: parent 				//Here we make sure that this item fills (captures) the whole parent item's (--> 'rectangle_root') area 
		onClicked: 
		{
			console.log("Mouse click detected...") 	//A way of sending text to the BrainStim Output Log Pane, used for debugging and testing purpose
			Qt.quit();									//Call the 'Qt.quit()' function to automatically exit
		}
	}	
}
