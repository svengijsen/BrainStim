import QtQuick 2.0
import Qt3D 2.0
import Qt3D.Shapes 2.0

Viewport 
{
	width: 640; height: 480
	Cube 
	{
		id: teapot
		pretransform: [
			Rotation3D {
				id: itemx_rotateX
				angle: 30
				axis: Qt.vector3d(1, 0, 0)
			},
			Rotation3D {
				id: itemx_rotateY
				angle: 30
				axis: Qt.vector3d(0, 1, 0)
			},
			Rotation3D {
				id: itemx_rotateZ
				angle: 30
				axis: Qt.vector3d(0, 0, 1)
			}
		]
		effect: program
	}
	
	ShaderProgram 
	{
		id: program
		texture: "../Common/Images/Additional/qt-logo.png"
		vertexShader: "
		attribute highp vec4 qt_Vertex;
		uniform highp mat4 qt_ModelViewProjectionMatrix;
		attribute highp vec4 qt_MultiTexCoord0;
		varying highp vec4 texCoord;
		void main(void)
		{
			gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
			texCoord = -qt_MultiTexCoord0;
			//texCoord = qt_Vertex;
		}
		"
		
		fragmentShader: "
		varying highp vec4 texCoord;
		uniform sampler2D qt_Texture0;
		void main(void)
		{
			mediump vec4 textureColor = texture2D(qt_Texture0, texCoord.st);
			gl_FragColor = textureColor;
			//gl_FragColor = vec4(texCoord.xyz , 1.0);
		}
		"
	}
}