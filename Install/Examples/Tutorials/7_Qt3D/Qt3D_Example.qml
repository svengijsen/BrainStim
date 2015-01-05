import QtQuick 2.0
import Qt3D 2.0 as Qt3D_Main
import Qt3D.Shapes 2.0 as Qt3D_Shapes

Item {
	id: idMainFrame
	width: 720
	height: 480

    Qt3D_Main.Viewport  {
		anchors.fill: parent
		//width: 720
		//height: 480		
        camera: Qt3D_Main.Camera {eye: Qt.vector3d(0,0,10)}
		picking: true

        Qt3D_Main.Item3D
		{
			id: itemx
			scale: 0.5
			position: Qt.vector3d(0.0,1.0,0.0)
            mesh: Qt3D_Main.Mesh {source: "../Common/Meshes/monkey.obj"}
			pretransform: [
                Qt3D_Main.Rotation3D {
					id: itemx_rotateX
					angle: 90
					axis: Qt.vector3d(1, 0, 0)
				},
                Qt3D_Main.Rotation3D {
					id: itemx_rotateY
					angle: 0
					axis: Qt.vector3d(0, 1, 0)
				},
                Qt3D_Main.Rotation3D {
					id: itemx_rotateZ
					angle: 0
					axis: Qt.vector3d(0, 0, 1)
				}
			]			
		}
	
        Qt3D_Shapes.Sphere
		{
			id: sphere
			scale: 0.5
			position: Qt.vector3d(0.0,0.0,0.0)			
		}		
		
        Qt3D_Shapes.Teapot
		{
			id: teapot
			scale: 0.5
			position: Qt.vector3d(0.0,-1.0,0.0)
		}	
	}
}

