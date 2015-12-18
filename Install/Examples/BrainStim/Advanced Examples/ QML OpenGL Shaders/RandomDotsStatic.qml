import QtQuick 2.0
import QtQuick.Particles 2.0
import BrainStim_QMLExtensions 1.0 // import types from the plugin

Rectangle 
{
	id: root

	color: "black"
	//property real fSpeed: 200//in pixels/second
	//property real fcenterSpotDiameter: 100//in pixels
	property color cParticleColor: "white"
	property int nMaxParticleLifeSpan: 1000

	ParticleSystem 
	{
		id: particleSystem
		width: parent.width
		height: parent.height
	
		/*
		Attractor 
		{
			id: idAttractor; 
			width: root.width;
			height: root.height; 
			pointX: root.width/2; 
			pointY: root.height/2; 
			strength: -2;
			//affectedParameter: Attractor.Velocity//Acceleration
			//proportionalToDistance: Attractor.InverseQuadratic
			Rectangle //Tracer box 
			{
				anchors.fill: parent
				color: "transparent"
				border.color: "green"
				border.width: 1
				visible: false
			}
		}
		*/

		Emitter 
		{
			id: emitter
			anchors.centerIn: parent
			width: parent.width
			height: parent.height
			//x: root.width/2
			//y: root.height/2
			system: particleSystem
			emitRate: 100 //in particles per second
			lifeSpan: root.nMaxParticleLifeSpan//Emitter.InfiniteLife//1000 * (root.height/(root.fSpeed * 2)) //in mSecs per particle
			lifeSpanVariation: 0 //lifespan variation between particles
			size: 1 //start size in pixels of particle
			endSize: 1 //end size in pixels of particle
			
			//velocity: idAngleDirection
			//use variabele life
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
				opacity: 1//0.0
				
				SequentialAnimation 
				{
					id: idParticleStartAnimation
					running: false
					loops: 1
					
					ParallelAnimation 
					{
						//NumberAnimation { target: idRectContainer; property: "width"; from: 0; to: 25; loops: 1; duration: root.nMaxParticleLifeSpan * 0.8 }
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
				Component.onCompleted: idParticleStartAnimation.start();
				Component.onDestruction: {
					idParticleStartAnimation.stop();
					//idRectContainer.color = root.color;//idParticleStartAnimation.stop(); //console.log("rectangle");//idRectContainer.width = 100;//idParticleStartAnimation.stop(); //idRectContainer.width = 0;//idParticleStartAnimation.stop();//console.log("Destruction Beginning!");
				}
			}
		}
		
		ItemParticle
		{
			id: idItemParticle
			system: particleSystem
			delegate: idParticleItemDelegate
			fade: false//false
		}
		
		AngleDirection
		{
			id: idAngleDirection
			angle: 0 //0 degrees points to the right
			angleVariation: 180 //=/- 180 degrees
			//magnitude: root.fSpeed //pixels per second
			//magnitudeVariation: 50 //magnitude variation
		}
	}
	
	Rectangle //Tracer box 
	{
		anchors.centerIn: parent
		width: root.fcenterSpotDiameter
		height: width
		color: root.color
		radius: height/2
		visible: false
	}
	
	SequentialAnimation 
	{
		id: idsadfrdg
		running: false
		loops: 100

	//ParallelAnimation 
	//{
		NumberAnimation { target: root; property: "rotate"; from: 0; to: 25; loops: 1; duration: 5000 }
	//	NumberAnimation { target: idRectContainer; property: "opacity"; from: 0.25; to: 1; loops: 1; duration: root.nMaxParticleLifeSpan * 0.4 }
	}
}


/*
ShaderEffect 
{
	id: randomDotsEffect
	visible: true
	mesh: GridMesh { resolution: Qt.size(100, 100) }
	//source: null
	
	property variant source
	property real fViewportWidth: source.width
	property real fViewportHeight: source.height
	property real fStimSquareLenght: Math.min(source.width/2,source.height/2)
	property real fItemWidthCount: 10
	property real fItemHeightCount: 10
	property bool bIsFlickering: true
	property bool bIsRotating: true
	property real fRotationStartAngle: 0.0
	property int nRotationSpeed: 12000
	property int nFlickrOn: 0
	property bool nShowFixPoint: true
	property int nFixPointSize: 20
	property real fFlickrFreq: 7.5
	property real fPolarWedgeSpan: 22.5
	property int nGapDiameter: 20
	property int nMovementDirection: 1
	//property int nTime: 0
	
	width: fStimSquareLenght*2
	height: fStimSquareLenght*2
	rotation: fRotationStartAngle
	
	fragmentShader: generateShader("fragment")
	vertexShader: generateShader("vertex")
	
        //NumberAnimation on nTime 
	//{
        //       from: 0; to: 1000; duration: 1000; loops: Animation.Infinite
	//	running: true
        //}	
	NumberAnimation on nFlickrOn 
	{
                from: 0; to: 2; duration: 2000/fFlickrFreq; loops: Animation.Infinite
		running: bIsFlickering
        }
	NumberAnimation on rotation 
	{
                from: fRotationStartAngle; to: nMovementDirection*(360.0+fRotationStartAngle); duration: nRotationSpeed; loops: Animation.Infinite
		running: bIsRotating
        }

//	Item
//	{
//		id: wedgesFunctionsID
//		objectName: "wedges_functions"
//		function doSomething(var)
//		{
//		}
//	}
		
	function generateShader(sType) 
	{
		var sShaderCode = "";
		if(sType === "fragment")
		{
			sShaderCode = "//FRAGMENT SHADER:\n" +

			"	varying highp vec2 qt_TexCoord0;\n" +
			"	uniform lowp sampler2D source;\n" +
			"	uniform lowp float qt_Opacity;\n" +
			"	uniform lowp float fStimSquareLenght;\n" +
			"	uniform lowp float fItemWidthCount;\n" +
			"	uniform lowp float fItemHeightCount;\n" +
			"	uniform lowp int nFlickrOn;\n" +
			//" 	uniform lowp int nTime;\n" +
			" 	uniform lowp int nGapDiameter;\n" +
			"	void main()\n" +
			"	{\n" +
			"		float fXindex = qt_TexCoord0.x * (fStimSquareLenght*2);\n" +
			"		if(fXindex > (fStimSquareLenght + (nGapDiameter/2)))\n" +
			"		{\n" +
			"			if((qt_TexCoord0.y > 0.25) && (qt_TexCoord0.y <= 0.75))\n" +
			"			{\n" +
							//Create a checkerboard
			"				float fYindex = qt_TexCoord0.y * (fStimSquareLenght*2);\n" +
			"				float fItemWidth = (fStimSquareLenght - (nGapDiameter/2))/fItemWidthCount;\n" +
			"				lowp int nIsColor1X = int(int(( ((fXindex - nGapDiameter) / fItemWidth) ) + nFlickrOn))%2;\n" +
			"				lowp int nIsColor1Y = int((fYindex / (fStimSquareLenght/fItemHeightCount)) + (mod(floor(fItemHeightCount), 2.0) * 0.5)   )%2;\n" +
			"				gl_FragColor = vec4(nIsColor1Y ^^ nIsColor1X, nIsColor1Y ^^ nIsColor1X, nIsColor1Y ^^ nIsColor1X, 1.0) * qt_Opacity;\n" +
			"			}\n" +
			"		}\n" +			
			"	}"		
		}
		else if(sType === "vertex")
		{
			sShaderCode = "//VERTEX SHADER:\n" +
				"uniform highp mat4 qt_Matrix;\n" +
				"attribute highp vec4 qt_Vertex;\n" +
				"attribute highp vec2 qt_MultiTexCoord0;\n" +
				"varying highp vec2 qt_TexCoord0;\n" +
				"uniform lowp float fStimSquareLenght;\n" +
				" uniform lowp int nGapDiameter;\n" +
				"uniform lowp float fPolarWedgeSpan;\n" +
				"void main()\n" +
				"{\n" +
				"	qt_TexCoord0 = qt_MultiTexCoord0;\n" +
				"	highp vec4 pos = qt_Vertex;\n" +
				"	if((true) && (qt_Vertex.x >= fStimSquareLenght))  {\n" +
						//Calculate for wedgespan, at first atan(0.5) is used
				"		float wedgespan_adj = 0.5 / tan(radians(fPolarWedgeSpan/2));\n" +//tan(radians(22.5))
						//Make triangular shape
				"		pos.y = ((qt_Vertex.y - fStimSquareLenght) / wedgespan_adj * ((qt_MultiTexCoord0.x - 0.5) * 2)) + fStimSquareLenght;\n" + 
						//Calculate the current points angle
				"		float rad_angle = atan( ( ((fStimSquareLenght*2) - pos.y)-(fStimSquareLenght) ) / (qt_Vertex.x - fStimSquareLenght));\n" + 
						//Calculate the bend adjusted X component
				"		highp float fXDist = cos(rad_angle) * (qt_Vertex.x - fStimSquareLenght);\n" +
						//Calculate the bend adjusted Y component
				"		highp float fYDist = sin(rad_angle) * (qt_Vertex.x - fStimSquareLenght);\n" +
						//Calculate the real new positions
				"		pos.x = fXDist + fStimSquareLenght;\n" +
				"		pos.y = fStimSquareLenght - fYDist;\n" +
				"	}\n" +
				"	gl_Position = qt_Matrix * pos;\n" +
				"}"
		}			
		//console.log(sShaderCode);
		return sShaderCode;
	}
}
*/