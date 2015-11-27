import QtQuick 2.0
import BrainStim_QMLExtensions 1.0

Rectangle //Root element
{
   id: rootRectangle //a custom ID
   color: "#555555" //RGB value ("#RRGGBB") DarkGrey background color

   Column
   {
      anchors.top: parent.top
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: 10
      DebugMode{}//Present in DebugMode.qml!
   }

   Rectangle //Child of Root
   {
      id: childRectangle
      width: 400
      height: 400
      color: "#dddddd" //RGB value ("#RRGGBB") Grey background color
      anchors.centerIn: parent //Center in its parent(rootRectangle)
	   
      Text 
      {
         id: centerText
         anchors.bottom: childRectangle.bottom //Make the bottom equal to that of childRectangle 
         anchors.horizontalCenter: parent.horizontalCenter //Center it horizontally in its parent
         text: "Hello World"
         font.family: "Helvetica"
         font.pointSize: 20	      
         color: rootRectangle.color //Set it to the same color of rootRectangle
      }	   
   
      Image
      {
         id: centerImage
         anchors.centerIn: parent
         source: "images/BrainStim.png"
      }
      
   }
   
   MouseArea //A Mouse Area item, that let's us capture mouse events and make it interactive
   {
      anchors.fill: parent //Here we make sure that the Area covers the whole rootRectangle
      onClicked: //this function is automatically called when this area is clicked with the left mouse button
      {
         console.log("Mouse Clicked!") //A way of sending text to the BrainStim Output Log Pane (Default), used for debugging and testing purpose
         if(centerText.text == "Hello World")
         {
            centerText.text = "Hello BrainStim";
         }
         else
         {
            centerText.text = "Hello World";
         }	 
      }
   }
   
   Item //For creating keyboard interaction we make use of a base type (Item) because it supports key handling
   {
      id: keyboardItem
      anchors.fill: parent //Here we make sure that the Area covers the whole rootRectangle again
      focus: true //Change the keyboard focus to this element
      Keys.onPressed: //this function is automatically called when a key is pressed while this area has focus
      {
         if (event.key == Qt.Key_Escape) //Check if the escape key was pressed
         {
            Qt.quit(); //Call the 'Qt.quit()' function to automatically exit, same as CTRL + 'a' (abort)
         } 
         else if (event.key == Qt.Key_1) //Check if the 1 key was pressed
         {
            functionsItem.setImageFunction("images/BrainStim.png");
            functionsItem.setTextFunction("Hello BrainStim");
         } 
         else if (event.key == Qt.Key_2) //Check if the 1 key was pressed
         {
            functionsItem.setImageFunction("images/World.png");
            functionsItem.setTextFunction("Hello World");
         } 	 
      }
   }

   Item
   {
      id: functionsItem
      objectName: "functions"
      function setTextFunction(sText)
      {
         console.log("setTextFunction() called with parameter:", sText)//only for debugging writes to stimulGL output file
         centerText.text = sText;
         return true;
      }

      function setImageFunction(sPath)
      {
         console.log("setTextFunction() called with parameter:", sPath)//only for debugging writes to stimulGL output file
         centerImage.source = sPath;
         return true;
      }
   }
}
