import QtQuick 2.0
import Qt3D 2.0
import Qt3D.Shapes 2.0

Item {
	id: screen

	Rectangle 
	{
		id: panel
		width: 640
		height: 480
		border.width:  3
		color: "#0000FF"

		Viewport
		{
			id: _3DViewport
			anchors.fill: parent
			camera: Camera 
			{
				id: _3DCamera
				eye: Qt.vector3d(350, 400, 800)
				center: Qt.vector3d(0,0,0)
				upVector: Qt.vector3d(0,1,0)
			}
			renderMode: "BufferedRender" //Important: otherwise this will not start from within a *.qs or *.exml file!
			navigation: true //Camera navigation allows the user to move the camera position around using the mouse.
			showSceneGraph: false//true //generates debugging information...
			picking: false // true, enables object selection, caution: this may create an error due to a QFuzzyCompareFunction(Viewport width)....
			showPicking: false //true, show a flat map of above for debugging purpose

			light: Light 
			{
				direction: Qt.vector3d(0, 10, -10)
			}
			
			Sphere
			{
				scale: 100
				levelOfDetail: 6
				axis: Qt.YAxis

				effect: Effect
				{
					texture: "../Common/Images/Additional/MoonMap.jpg"
				}
			}
		}
	}
}
