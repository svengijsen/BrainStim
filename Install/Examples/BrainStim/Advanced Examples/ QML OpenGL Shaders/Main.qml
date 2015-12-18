import QtQuick 2.0
import QtQuick.Particles 2.0
import BrainStim_QMLExtensions 1.0 // import types from the plugin

Rectangle 
{
	id: idBackground

	color: "black"
	
	RandomDots//Static
	{
		width: parent.width//200//idBackground.width * 1.5
		height: parent.height//200//width
		anchors.centerIn: parent
		
		NumberAnimation on rotation//rotation 
		{
			id: idRotationAnimation
			from: 0
			to: 359
			duration: 5000//root.nMaxParticleLifeSpan
			loops: Animation.Infinite
			running: true
		}
	
	/*
				SequentialAnimation 
				{
					id: idParticleStartAnimation
					running: false
					loops: 1
					
					ParallelAnimation 
					{
						NumberAnimation { target: idRectContainer; property: "width"; from: 0; to: 25; loops: 1; duration: root.nMaxParticleLifeSpan * 0.8 }
						NumberAnimation { target: idRectContainer; property: "opacity"; from: 0.25; to: 1; loops: 1; duration: root.nMaxParticleLifeSpan * 0.4 }
					}
					//PropertyAnimation { target: idRectContainer; property: "width"; to: 30; duration: 500 }
					//PropertyAction { target: idRectContainer; property: "width"; value: 10 }
					PauseAnimation { duration: root.nMaxParticleLifeSpan/10 }
					PropertyAction { target: idRectContainer; property: "color"; value: root.color }
					
					onStopped: 
					{
						//console.log("ss");
						idRectContainer.color = root.color;
						//console.log("ss");
					}
				}
				//NumberAnimation on width//rotation 
				//{
				//	id: idParticleStartAnimation
				//	from: 0
				//	to: 10
				//	duration: 3000//root.nMaxParticleLifeSpan
				//	loops: 1//Animation.Infinite
				//	running: false
				//}
	*/	
	}
}