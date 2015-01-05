import QtQuick 2.0
import QtMultimedia 5.0
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
		text: qsTr("Press key ('1' to '5') for selecting a corresponding vocal sound file,\nPress the 'p' key to play the file.\nPress the 'm' key to play a movie file.\nPress the 'CTRL+'a' key combination to stop.\n")
		anchors.centerIn: parent
		font.family: "Helvetica"
		font.pointSize: 20
		font.bold: true
		renderType: Text.NativeRendering
	}
	
	Text 
	{
		id: txtInputFile
		text: qsTr("")
		anchors.top: txtInfo.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		font.family: "Helvetica"
		font.pointSize: 20
		font.bold: true
		visible: false
		renderType: Text.NativeRendering
	}	
	
	SoundEffect 
	{
		//This type allows you to play uncompressed audio files (typically WAV files) in a generally lower latency way, 
		//and is suitable for "feedback" type sounds in response to user actions (e.g. virtual keyboard sounds, positive or negative feedback for popup dialogs, or game sounds). 
		//If low latency is not important, consider using the MediaPlayer or Audio types instead, since they support a wider variety of media formats and are less resource intensive.
		//Typically the sound effect should be reused, which allows all the parsing and preparation to be done ahead of time, and only triggered when necessary. 
		//This is easy to achieve with QML, since you can declare your SoundEffect instance and refer to it elsewhere.		
		id: playSoundEffect
		source: ""
	}	

	/*
	Audio 
	{
		id: playSound
		source: ""
	}
	*/

	/*
	Video 
	{
		id: video
		x : 100
		width : 400
		height : 400
		source: "../Common/Movies/Notes_INTRO_BG.wmv" 
	}
	*/
	
	MediaPlayer 
	{
		id: mediaplayer
		source: "../Common/Movies/Notes_INTRO_BG.wmv" 
	}

	VideoOutput 
	{
		anchors.horizontalCenter: parent.horizontalCenter
		width : 400
		height : 400
		source: mediaplayer
	}
	

	////Instead of the keyboard you can also detect a mouse click for a further action to perform...	
	//MouseArea 
	//{
	//	id: playArea
	//	anchors.fill: parent
	//	onPressed: mediaplayer.play();
	//}

	Item 
	{
		anchors.fill: parent
		focus: true
		Keys.onPressed: 
		{
			if (event.key == Qt.Key_1) 
			{
				functionsID.setSoundFunction("./../Common/Sounds/1.wav"); 
				event.accepted = true;
			} 
			if (event.key == Qt.Key_2) 
			{
				functionsID.setSoundFunction("./../Common/Sounds/2.wav"); 
				event.accepted = true;
			}
			if (event.key == Qt.Key_3) 
			{
				functionsID.setSoundFunction("./../Common/Sounds/3.wav"); 
				event.accepted = true;
			}
			if (event.key == Qt.Key_4) 
			{
				functionsID.setSoundFunction("./../Common/Sounds/4.wav"); 
				event.accepted = true;
			}
			if (event.key == Qt.Key_5) 
			{
				functionsID.setSoundFunction("./../Common/Sounds/5.wav"); 
				event.accepted = true;
			}
			else if (event.key == Qt.Key_P) 
			{
				functionsID.playSoundFunction();	
				event.accepted = true;
			} 
			else if (event.key == Qt.Key_M) 
			{
				functionsID.playMovieFunction();	
				event.accepted = true;
			}			
		}
	}
	
	Item
	{
		id: functionsID
		objectName: "functions"
		function setSoundFunction(path)
		{
			console.log("setSoundFunction() Got parameter:", path)//only for debugging writes to stimulGL output file
			playSoundEffect.source = path;
			txtInputFile.visible = true;
			txtInputFile.text = "Selected file: " + path;
			return true;
		}
		function playSoundFunction()
		{
			console.log("playSoundFunction()")//only for debugging writes to stimulGL output file
			playSoundEffect.play();			
			//playSound.play(); 
			return true;
		}
		function setAndPlaySoundFunction(path)
		{
			console.log("setAndPlaySoundFunction()")//only for debugging writes to stimulGL output file
			playSoundEffect.source = path;
			txtInputFile.visible = true;
			txtInputFile.text = "Selected file: " + path;			
			playSoundEffect.play();			
			return true;
		}
		function playMovieFunction()
		{
			console.log("playMovieFunction()")//only for debugging writes to stimulGL output file
			//video.play(); 
			mediaplayer.play();
			return true;
		}				
	}	
}