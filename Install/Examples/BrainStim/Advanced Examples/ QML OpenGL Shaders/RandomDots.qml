import QtQuick 2.0
import QtQuick.Particles 2.0
import BrainStim_QMLExtensions 1.0 // import types from the plugin

Rectangle 
{
	id: root

	color: "black"
	property real fSpeed: 200//in pixels/second
	//property real fcenterSpotDiameter: 100//in pixels
	property bool bIsMoving: false
	property color cParticleColor: "white"
	property int nMaxParticleLifeSpan: 0
	property int nParticlesPerSecond: 0
	
	function generateShader(sType) 
	{
		var sShaderCode = "";
		if(sType === "vertex")
		{
			//	The vertex shader for particles is responsible for simulating the movement of particles over time, the particle data itself only has the starting position and spawn time.
			//	Important! To aid writing a particle vertex shader, the following GLSL code is prepended to your vertex shader as defaultMain()
			/*
			attribute highp vec2 qt_ParticlePos;
			attribute highp vec2 qt_ParticleTex;
			attribute highp vec4 qt_ParticleData; //  x = time,  y = lifeSpan, z = size,  w = endSize
			attribute highp vec4 qt_ParticleVec; // x,y = constant velocity,  z,w = acceleration
			attribute highp float qt_ParticleR;
			uniform highp mat4 qt_Matrix;
			uniform highp float qt_Timestamp;
			varying highp vec2 qt_TexCoord0;
			void defaultMain() {
			    qt_TexCoord0 = qt_ParticleTex;
			    highp float size = qt_ParticleData.z;
			    highp float endSize = qt_ParticleData.w;
			    highp float t = (qt_Timestamp - qt_ParticleData.x) / qt_ParticleData.y;
			    highp float currentSize = mix(size, endSize, t * t);
			    if (t < 0. || t > 1.)
				currentSize = 0.;
			    highp vec2 pos = qt_ParticlePos
					   - currentSize / 2. + currentSize * qt_ParticleTex   // adjust size
					   + qt_ParticleVec.xy * t * qt_ParticleData.y         // apply velocity vector..
					   + 0.5 * qt_ParticleVec.zw * pow(t * qt_ParticleData.y, 2.);
			    gl_Position = qt_Matrix * vec4(pos.x, pos.y, 0, 1);
			}
			*/
			
			sShaderCode = "//vertexShader:\n" +
			"    uniform lowp float qt_Opacity;\n" +
			"    varying lowp float fFade;\n" +
			//"    varying lowp float fBlur;\n" +

			
			"    void main() {\n" +
//			"	defaultMain();\n" +//here we see a call to the above default prepended code
			"	highp float timeAnimationPart = 0.25;\n" +
			"	qt_TexCoord0 = qt_ParticleTex;\n" +
			"	highp float size = qt_ParticleData.z;\n" +
			"	highp float endSize = qt_ParticleData.w;\n" +
			"	highp float t = (qt_Timestamp - qt_ParticleData.x) / qt_ParticleData.y;\n" +
			"	highp float currentSize = mix(size, endSize, min(1.0, t * (1.0/timeAnimationPart)));\n" +
			//"	highp float currentSize = mix(size, endSize, t * t);\n" +
			"	if (t < 0. || t > 1.)\n" +
			"		currentSize = 0.;\n" +
			"	highp vec2 pos = qt_ParticlePos\n" +
			"		   - currentSize / 2. + currentSize * qt_ParticleTex\n" +   // adjust size
			"		   + qt_ParticleVec.xy * t * qt_ParticleData.y\n" +         // apply velocity vector..
			"		   + 0.5 * qt_ParticleVec.zw * pow(t * qt_ParticleData.y, 2.);\n" +
			"	gl_Position = qt_Matrix * vec4(pos.x, pos.y, 0, 1);\n" +



			
			
			//"	highp float t = (qt_Timestamp - qt_ParticleData.x) / qt_ParticleData.y;\n" + //(CurrentTime - Time) / Lifespan ->> t = 0..>>..1
			"	highp float fadeIn = min(t * (1.0/timeAnimationPart), 1.);\n" +
			"	highp float fadeOut = 1. - max(0., min((t - 0.75) * 4., 1.));\n" +
			"	fFade = fadeIn * fadeOut * qt_Opacity;\n" +//
			//"	fFade = 0.1;\n" +
			//"	fBlur = max(0.2 * t, t * qt_ParticleR);\n" +
			"    }"
		}
		else if(sType === "fragment")
		{
			sShaderCode = "//FRAGMENT SHADER:\n" +
			"    uniform sampler2D source;\n" +
			//"    uniform sampler2D blurred;\n" +
			"    varying highp vec2 qt_TexCoord0;\n" +
			//"    varying highp float fBlur;\n" +
			"    varying highp float fFade;\n" +
			"    void main() {\n" +
			"	gl_FragColor = texture2D(source, qt_TexCoord0) * fFade;\n" + //mix(texture2D(source, qt_TexCoord0), texture2D(blurred, qt_TexCoord0), min(1.0,fBlur*3.0)) * fFade;\n" +
			//"	gl_FragColor = vec4(0.0, 1.0, 0.0, .5) * fFade;\n" + //All black
			"    }"
		}			
		console.log(sShaderCode);
		return sShaderCode;
	}
	
	Rectangle 
	{
		id: idBackGroundRect
		//anchors.fill: parent
		color: parent.color
		width: parent.width * 1.45
		height: width
		anchors.centerIn: parent
		visible: false
		
		ParticleSystem 
		{
			id: idParticleSystem
			width: idBackGroundRect.width
			height: idBackGroundRect.height
		
			Attractor 
			{
				id: idAttractor; 
				width: idBackGroundRect.width;
				height: idBackGroundRect.height; 
				pointX: width / 2;
				pointY: height / 2;
				strength: (root.bIsMoving == true) ? -2 : 0
				//affectedParameter: Attractor.Velocity//Acceleration
				//proportionalToDistance: Attractor.InverseQuadratic
	//			Rectangle //Tracer box 
	//			{
	//				anchors.fill: parent
	//				color: "transparent"
	//				border.color: "green"
	//				border.width: 1
	//				visible: false
	//			}
			}
			

			Emitter 
			{
				id: idEmitter
				anchors.centerIn: parent
				width: idBackGroundRect.width//(root.bIsMoving == true) ? idBackGroundRect.width/4 : idBackGroundRect.width
				height: idBackGroundRect.height//(root.bIsMoving == true) ? idBackGroundRect.height/4 : idBackGroundRect.height
				system: idParticleSystem
				emitRate: root.nParticlesPerSecond//in particles per second
				lifeSpan: root.nMaxParticleLifeSpan//Emitter.InfiniteLife//1000 * (root.height/(root.fSpeed * 2)) //in mSecs per particle
				lifeSpanVariation: 0 //lifespan variation between particles
				size: 5 //start size in pixels of particle
				endSize: 5 //end size in pixels of particle
				
				//idEmitter.maximumEmitted:1000
				//velocity: idAngleDirection
				//use variabele life
	//			Rectangle //Tracer box 
	//			{
	//				anchors.fill: parent
	//				color: "transparent"
	//				border.color: "green"
	//				border.width: 1
	//				visible: false
	//			}
			}

		/*
			ImageParticle 
			{
				source: "images/particle3.png"
				system: idParticleSystem
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
					opacity: 1
					height: width
					color: "white"
					radius: height/2
					
					
					/*
					width: 0
					opacity: 0.25
					SequentialAnimation 
					{
						id: idParticleStartAnimation
						running: false
						loops: 1
						
						ParallelAnimation 
						{
							NumberAnimation { target: idRectContainer; property: "width"; from: 0; to: 5; loops: 1; duration: root.nMaxParticleLifeSpan * 0.1 }
							NumberAnimation { target: idRectContainer; property: "opacity"; from: 0.0; to: 1; loops: 1; duration: root.nMaxParticleLifeSpan * 0.4 }
						}
						PauseAnimation { duration: root.nMaxParticleLifeSpan * 0.1 }
						NumberAnimation { target: idRectContainer; property: "opacity"; from: 1; to: 0.0; loops: 1; duration: root.nMaxParticleLifeSpan * 0.4 }
						PropertyAction { target: idRectContainer; property: "color"; value: root.color }
						
						onStopped: 
						{
							//console.log("ss");
							idRectContainer.color = root.color;
							//console.log("ss");
						}
					}
					
					Component.onCompleted: idParticleStartAnimation.start();
					Component.onDestruction: {
						idParticleStartAnimation.stop();
						//idRectContainer.color = root.color;//idParticleStartAnimation.stop(); //console.log("rectangle");//idRectContainer.width = 100;//idParticleStartAnimation.stop(); //idRectContainer.width = 0;//idParticleStartAnimation.stop();//console.log("Destruction Beginning!");
					}
					//*/
				}
			}
			
			//ItemParticle
			//{
			//	id: idItemParticle
			//	system: idParticleSystem
			//	delegate: idParticleItemDelegate
			//	fade: false
			//}
			
			Image
			{
				id: idImageSource
				source: "images/particle3.png"
			}
			
			Rectangle
			{
				id: idRectSource
				width: 10
				opacity: 1
				height: width
				color: "white"
				radius: height/2
			}
			
			ShaderEffectSource 
			{
				id: idShaderSource
				sourceItem: idRectSource//idImageSource
				hideSource: false//true
			}
			
			CustomParticle
			{
				id: idCustomParticle
				system: idParticleSystem
				property variant source: idShaderSource
				vertexShader: generateShader("vertex")
				fragmentShader: generateShader("fragment")
				
				/*
				vertexShader:"
				    uniform lowp float qt_Opacity;
				    varying lowp float fFade;
				    varying lowp float fBlur;

				    void main() {
					defaultMain();
					highp float t = (qt_Timestamp - qt_ParticleData.x) / qt_ParticleData.y;
					highp float fadeIn = min(t * 10., 1.);
					highp float fadeOut = 1. - max(0., min((t - 0.75) * 4., 1.));

					fFade = fadeIn * fadeOut * qt_Opacity;
					fBlur = max(0.2 * t, t * qt_ParticleR);
				    }
				"
				property variant source: theSource
				property variant blurred: ShaderEffectSource {
				sourceItem: ShaderEffect {
				    width: theItem.width
				    height: theItem.height
				    property variant delta: Qt.size(0.0, 1.0 / height)
				    property variant source: ShaderEffectSource {
					sourceItem: ShaderEffect {
					    width: theItem.width
					    height: theItem.height
					    property variant delta: Qt.size(1.0 / width, 0.0)
					    property variant source: theSource
					    fragmentShader: "
						uniform sampler2D source;
						uniform lowp float qt_Opacity;
						uniform highp vec2 delta;
						varying highp vec2 qt_TexCoord0;
						void main() {
						    gl_FragColor =(0.0538 * texture2D(source, qt_TexCoord0 - 3.182 * delta)
								 + 0.3229 * texture2D(source, qt_TexCoord0 - 1.364 * delta)
								 + 0.2466 * texture2D(source, qt_TexCoord0)
								 + 0.3229 * texture2D(source, qt_TexCoord0 + 1.364 * delta)
								 + 0.0538 * texture2D(source, qt_TexCoord0 + 3.182 * delta)) * qt_Opacity;
						}"
					}
				    }
				    fragmentShader: "
					uniform sampler2D source;
					uniform lowp float qt_Opacity;
					uniform highp vec2 delta;
					varying highp vec2 qt_TexCoord0;
					void main() {
					    gl_FragColor =(0.0538 * texture2D(source, qt_TexCoord0 - 3.182 * delta)
							 + 0.3229 * texture2D(source, qt_TexCoord0 - 1.364 * delta)
							 + 0.2466 * texture2D(source, qt_TexCoord0)
							 + 0.3229 * texture2D(source, qt_TexCoord0 + 1.364 * delta)
							 + 0.0538 * texture2D(source, qt_TexCoord0 + 3.182 * delta)) * qt_Opacity;
					}"
				    }
				}
				fragmentShader: "
				    uniform sampler2D source;
				    uniform sampler2D blurred;
				    varying highp vec2 qt_TexCoord0;
				    varying highp float fBlur;
				    varying highp float fFade;
				    void main() {
					gl_FragColor = mix(texture2D(source, qt_TexCoord0), texture2D(blurred, qt_TexCoord0), min(1.0,fBlur*3.0)) * fFade;
				    }"
				
			    }*/
				
			}
			
			AngleDirection
			{
				id: idAngleDirection
				angle: 0 //0 degrees points to the right
				angleVariation: 180 //=/- 180 degrees
				//magnitude: root.fSpeed //pixels per second
				//magnitudeVariation: 50 //magnitude variation
			}
			
			Age 
			{
				id: idAgeAffector
				anchors.fill: parent
				system: idParticleSystem
				once: false
				lifeLeft: 0
				advancePosition: true
				enabled: false
			}
			
			Wander 
			{
				id: idWanderAffector
				anchors.fill: parent
				system: idParticleSystem
				xVariance: 100;//360/(wanderer.affectedParameter+1);
				yVariance: 100;
				pace: 1000;//*(wanderer.affectedParameter+1);
				enabled: false
			}
		}
		
	//	Rectangle //Tracer box 
	//	{
	//		anchors.centerIn: parent
	//		width: root.fcenterSpotDiameter
	//		height: width
	//		color: root.color
	//		radius: height/2
	//		visible: false
	//	}
		
		SequentialAnimation 
		{
			id: idRotationAnimation
			
			property int nStartAngle: 0
			property int nEndAngle: 359
			
			running: false
			loops: Animation.Infinite
			NumberAnimation { target: idBackGroundRect; property: "rotation"; from: idRotationAnimation.nStartAngle; to: idRotationAnimation.nEndAngle; loops: 1; duration: 4000 }
		}
		
		/*
		Item 
		{
			anchors.fill: parent
			focus: true
			Keys.onPressed: 
			{
				if (event.key == Qt.Key_1) //Extending Moving Dots
				{
					functionsID.setDelayedExperimentType("ExtendingMovingDots",2000);
				}
				else if(event.key == Qt.Key_2) // Circular Moving Dots
				{
					functionsID.setDelayedExperimentType("CircularMovingDots_CW",2000);
				}
				else if(event.key == Qt.Key_3)// Static Dots
				{
					functionsID.setDelayedExperimentType("StaticDots",2000);
				}
				else if(event.key == Qt.Key_4)//Random Moving Dots
				{
					functionsID.setDelayedExperimentType("RandomMovingDots",2000);
				}
				else if(event.key == Qt.Key_5)//Fixation
				{
					functionsID.setDelayedExperimentType("Fixation",2000);
				}
				else if(event.key == Qt.Key_6)
				{
					console.log(idEmitter.maximumEmitted);
					//idParticleSystem.start();
				}
				else if(event.key == Qt.Key_7)
				{
					//idAgeRectangle.x = 900;
					//idAgeRectangle.y = 500;
					//idParticleSystem.reset();
					//idAgeAffector.enabled = !idAgeAffector.enabled ;
					//idAgeAffector.enabled = false;
				}
			}
		}*/
	}
	
	Column
	{
		x: 0
		y: 0
		width: 311
		height: 39
		spacing: 10
		DebugMode{}//See DebugMode.qml!
	}
	
	Text
	{
		id: idFixPoint
		anchors.centerIn: parent
		text: "+"
		font.bold: true
		font.family: "Helvetica"
		font.pointSize: 15
		color: "red"
		renderType: Text.NativeRendering
		visible: true
	}
	
	SequentialAnimation 
	{
		id: idChangeAnimationType
		property int nPauseDuration: 0
		property string sExpType: ""
		running: false
		
		SequentialAnimation 
		{
			ScriptAction { script: functionsID.setExperimentType("Fixation"); }
			PauseAnimation { duration: idChangeAnimationType.nPauseDuration }
			ScriptAction { script: functionsID.setExperimentType(idChangeAnimationType.sExpType); }
		 }
		loops: 1
	}
	
	Item
	{
		id: functionsID
		objectName: "functions"
		function setDelayedExperimentType(sExpType, nDelay)
		{
			idChangeAnimationType.nPauseDuration = nDelay;
			idChangeAnimationType.sExpType = sExpType;
			idChangeAnimationType.start();
		}
		
		function setExperimentType(sExpType)
		{
			if (sExpType == "ExtendingMovingDots") //ExtendingMovingDots
			{
				idAgeAffector.enabled = true;
				root.nParticlesPerSecond = 2000;
				root.nMaxParticleLifeSpan = 5000;
				idWanderAffector.enabled = false;
				root.bIsMoving = true;
				idRotationAnimation.running = false;
				idAgeAffector.enabled = false;
				idBackGroundRect.visible = true;
			}
			else if((sExpType == "CircularMovingDots_CW") || (sExpType == "CircularMovingDots_CCW"))// CircularMovingDots
			{
				idAgeAffector.enabled = true;
				root.nParticlesPerSecond = 1000;
				root.nMaxParticleLifeSpan = 1000;
				if(sExpType == "CircularMovingDots_CW")
				{
					idRotationAnimation.nStartAngle = 0;
					idRotationAnimation.nEndAngle = 359;
				}
				else
				{
					idRotationAnimation.nStartAngle = 359;
					idRotationAnimation.nEndAngle = 0;
				}
				idWanderAffector.enabled = false;
				root.bIsMoving = false;
				idRotationAnimation.running = true;
				idAgeAffector.enabled = false;
				idBackGroundRect.visible = true;
			}
			else if(sExpType == "StaticDots")// StaticDots
			{
				idAgeAffector.enabled = true;
				root.nParticlesPerSecond = 200;
				root.nMaxParticleLifeSpan = 10000;
				idWanderAffector.enabled = false;
				root.bIsMoving = false;
				idRotationAnimation.running = false;
				idAgeAffector.enabled = false;
				idBackGroundRect.visible = true;
			}
			else if(sExpType == "RandomMovingDots")//RandomMovingDots
			{
				idAgeAffector.enabled = true;
				root.nParticlesPerSecond = 1000;
				root.nMaxParticleLifeSpan = 1000;
				idWanderAffector.enabled = true;
				root.bIsMoving = false;
				idRotationAnimation.running = false;
				idAgeAffector.enabled = false;
				idBackGroundRect.visible = true;
			}
			else if(sExpType == "Fixation")//Fixation
			{
				idAgeAffector.enabled = true;
				root.nParticlesPerSecond = 1;
				root.nMaxParticleLifeSpan = 1;
				idBackGroundRect.visible = false;
				idWanderAffector.enabled = false;
				root.bIsMoving = false;
				idRotationAnimation.running = false;
			}
		}
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