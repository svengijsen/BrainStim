import QtQuick 2.0
import QtMultimedia 5.0 //We first need to import this library to make use of the 'SoundEffect' item for playing sounds, see below

Rectangle
{
	id: rectStimuliArea
	width: 640
	height: 480
	color: "white" //White background color

	Image
	{
		id: imgFruit
		anchors.horizontalCenter: parent.horizontalCenter 		//Anchors provide a way to position an item by specifying its relationship with other items
		anchors.verticalCenter: parent.verticalCenter
		fillMode: Image.PreserveAspectFit
		width: 300; height: 300 						//This is the area in which the source file is automatically scaled with a preserved aspect fit
		source: ""									//The relative path to the image file
	}
	
	Text
	{
		id: txtFruit
		color: "green"
		text: ""
		font.family: "Lucida Sans"
		font.bold: true
		font.pointSize: 32
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
		anchors.horizontalCenter: rectStimuliArea.horizontalCenter
		anchors.topMargin: 5
		anchors.top: imgFruit.bottom
		renderType: Text.NativeRendering
	}
	
	Text
	{
		id: txtFixation
		width: 60
		height: 50
		anchors.horizontalCenter: rectStimuliArea.horizontalCenter
		anchors.verticalCenter: rectStimuliArea.verticalCenter
		color: "red"
		text: "+"
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
		font.bold: true
		font.pixelSize: 20
		renderType: Text.NativeRendering
	}
	
	SoundEffect 
	{
		//This type allows you to play uncompressed audio files (typically WAV files) in a generally lower latency way, 
		//and is suitable for "feedback" type sounds in response to user actions (e.g. virtual keyboard sounds, positive or negative feedback for popup dialogs, or game sounds). 
		//If low latency is not important, consider using the MediaPlayer or Audio types instead, since they support a wider variety of media formats and are less resource intensive.
		//Typically the sound effect should be reused, which allows all the parsing and preparation to be done ahead of time, and only triggered when necessary. 
		//This is easy to achieve with QML, since you can declare your SoundEffect instance and refer to it elsewhere.		
		id: audBeepSound
		source: ""
	}	
	
	Column //We'll use a column placeholder here as a to hold the item 'ShowExperimentStructure' that is implemented in the file 'ShowExperimentStructure.qml'
	{
		x:20; y:20; width:100; height:100; spacing:10
		ShowExperimentStructure{}
	}

	Item 
	{
		id: functionsID
		objectName: "Fruits_Functions"
		function updateExperiment(sType) 
		{
			if(sType == "acai")
			{
				txtFixation.visible = false
				txtFruit.text = sType
				imgFruit.source = "images/acai.jpg"
				audBeepSound.source = "./audio/audio_2.wav"
				audBeepSound.play();
			}
			else if(sType == "banana")
			{
				txtFixation.visible = false
				txtFruit.text = sType;
				imgFruit.source = "images/banana.jpg"
				audBeepSound.source = "./audio/audio_1.wav"
				audBeepSound.play();				
			}
			else if(sType == "fixation")
			{
				txtFixation.visible = true
				txtFruit.text = sType;
				imgFruit.source = ""
				audBeepSound.source = ""			
			}
			else//undefined value...
			{
				txtFixation.visible = false;
				txtFruit.text = "Undefined Value !!!";
				imgFruit.source = ""
				audBeepSound.source = ""
			}
			return true;
		}
	}	
}