import QtQuick 2.0
import QtQuick.Particles 2.0
import BrainStim_QMLExtensions 1.0 // import types from the plugin

Rectangle 
{
	id: root

	color: "black"
	property real fSpeed: 200//in pixels/second
	property real fcenterSpotDiameter: 100//in pixels
	property color cParticleColor: "white"
	
	Rectangle 
	{
		id: idParticleRect
		
		anchors.fill: parent
		color: root.color
		
		ParticleSystem 
		{
			id: particleSystem
		}

		Emitter 
		{
			id: emitter
			anchors.centerIn: parent
			width: parent.width
			height: parent.height
			system: particleSystem
			emitRate: 100 //in particles per second
			lifeSpan: 1000 * (root.height/(root.fSpeed * 2)) //in mSecs per particle
			lifeSpanVariation: 0 //lifespan variation between particles
			size: 1 //start size in pixels of particle
			endSize: 1 //end size in pixels of particle
			
			velocity: idAngleDirection
			
			Rectangle //Tracer box 
			{
				anchors.fill: parent
				color: "transparent"
				border.color: "green"
				border.width: 1
				visible: false
			}
		}

	/*
		ImageParticle 
		{
			source: "images/particle3.png"
			system: particleSystem
			color: root.cParticleColor//'#FFD700' //gold color
			//colorVariation: 0.2 //change color +/- 20%
			//rotation: 15 //start by 15 degrees clockwise
			//rotationVariation: 5//vary start by +/- 5 degrees
			//rotationVelocity: 45 //45 degrees per second clockwise
			//rotationVelocityVariation: 15 //+/- 15 degrees variation on velocity
			//entryEffect: ImageParticle.Scale //entry effect for the particle
			entryEffect: ImageParticle.None
		}
	*/

		Component 
		{
			id: idParticleItemDelegate
			Rectangle
			{
				id: idRectContainer
				width: 10
				height: width
				color: "white"
				radius: height/2
			}
		}
		
		ItemParticle
		{
			id: idItemParticle
			system: particleSystem
			delegate: idParticleItemDelegate
		}
		
		AngleDirection
		{
			id: idAngleDirection
			angle: 0 //0 degrees points to the right
			angleVariation: 180 //=/- 180 degrees
			magnitude: root.fSpeed //pixels per second
			//magnitudeVariation: 50 //magnitude variation
		}
		
		Rectangle //Tracer box 
		{
			anchors.centerIn: parent
			width: root.fcenterSpotDiameter
			height: width
			color: root.color
			radius: height/2
			visible: true
		}
		
		Item 
		{
			anchors.fill: parent
			focus: true
			Keys.onPressed: 
			{
				if (event.key == Qt.Key_1) 
				{
					//mainRectangle.changeMovement();
				}
				else if (event.key == Qt.Key_Up) 
				{
					//mainRectangle.changeOrientation(barShaderEffect.rotation + 6);
				}
				else if (event.key == Qt.Key_Down) 
				{
					//mainRectangle.changeOrientation(barShaderEffect.rotation - 6);
				}
				else if (event.key == Qt.Key_Right) 
				{
					//mainRectangle.changeFlickr();
					idParticleRect.rotation = idParticleRect.rotation + 2;
				}	
			}
		}
	}
}
