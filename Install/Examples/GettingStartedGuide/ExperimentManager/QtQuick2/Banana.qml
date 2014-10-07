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
		source: "images/banana.jpg"					//The relative path to the image file
		onStatusChanged: if (imgFruit.status == Image.Ready)	
		{
			/*
			Here we define an Active property event that is executed whenever the status of the 'imgFruit' item chages to 'Image.Ready' (When the image is loaded a sound file is played)
			*/
			audBeepSound.play()						//Call the play() slot to play the sound
		}
	}
	
	Text
	{
		id: txtFruit
		color: "blue"
		text: "Banana"
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
	
	SoundEffect 
	{
		//This type allows you to play uncompressed audio files (typically WAV files) in a generally lower latency way, 
		//and is suitable for "feedback" type sounds in response to user actions (e.g. virtual keyboard sounds, positive or negative feedback for popup dialogs, or game sounds). 
		//If low latency is not important, consider using the MediaPlayer or Audio types instead, since they support a wider variety of media formats and are less resource intensive.
		//Typically the sound effect should be reused, which allows all the parsing and preparation to be done ahead of time, and only triggered when necessary. 
		//This is easy to achieve with QML, since you can declare your SoundEffect instance and refer to it elsewhere.		
		id: audBeepSound
		source: "./audio/audio_1.wav"
	}	
}
