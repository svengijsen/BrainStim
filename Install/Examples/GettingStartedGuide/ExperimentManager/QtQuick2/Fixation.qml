import QtQuick 2.0

Rectangle
{
	id: rectStimuliArea
	width: 640
	height: 480
	color: "white" //White background color

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
}
