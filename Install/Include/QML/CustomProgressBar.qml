import QtQuick 2.0

Item 
{
	id: progressbar
    property double minimum: 0.0
    property double maximum: 100.0
    property double value: 0.0
	
    property color color: "black"

	width: 500
	height: 23
	clip: true

	Rectangle 
	{
		id: border
		anchors.fill: parent
		//anchors.bottomMargin: 1
		//anchors.rightMargin: 1
		color: "transparent"
		border.width: 1
		border.color: parent.color
	}
	
	Rectangle {
	    id: gonogorectangle
        color: progressbar.color;
	    anchors.fill: border
        scale: 1

	    Rectangle {
	        id: activity
	        width: gonogorectangle.width
            color: "black"
	        radius: 5
	        border.width: 5
			anchors.fill: gonogorectangle
            scale: 0.9
			anchors.leftMargin: 15
			anchors.rightMargin: 15

            gradient: Gradient {
                GradientStop { position: 0.0; color: "blue" }
                GradientStop { position: 1-value-0.0001; color: "blue" }
                GradientStop { position: 1-value; color: "red" }
                GradientStop { position: 1.0; color: "red" }
            }

            Grid {
                id: grid1
                columns: 1
                rowSpacing: 0
                anchors.fill: activity

                Rectangle {
                    id: rectangle10
                    color: "transparent"
                    border.color: "black"
                    border.width: activity.border.width
                    width: activity.width
                    height: activity.height/10-grid1.rowSpacing
                }

                Rectangle {
                    id: rectangle9
                    color: "transparent"
                    border.color: "black"
                    border.width: activity.border.width
                    width: activity.width
                    height: activity.height/10-grid1.rowSpacing
                }
                Rectangle {
                    id: rectangle8
                    color: "transparent"
                    border.color: "black"
                    border.width: activity.border.width
                    width: activity.width
                    height: activity.height/10-grid1.rowSpacing
                }

                Rectangle {
                    id: rectangle7
                    color: "transparent"
                    border.color: "black"
                    border.width: activity.border.width
                    width: activity.width
                    height: activity.height/10-grid1.rowSpacing
                }
                Rectangle {
                    id: rectangle6
                    color: "transparent"
                    border.color: "black"
                    border.width: activity.border.width
                    width: activity.width
                    height: activity.height/10-grid1.rowSpacing
                }

                Rectangle {
                    id: rectangle5
                    color: "transparent"
                    border.color: "black"
                    border.width: activity.border.width
                    width: activity.width
                    height: activity.height/10-grid1.rowSpacing
                }
                Rectangle {
                    id: rectangle4
                    color: "transparent"
                    border.color: "black"
                    border.width: activity.border.width
                    width: activity.width
                    height: activity.height/10-grid1.rowSpacing
                }

                Rectangle {
                    id: rectangle3
                    color: "transparent"
                    border.color: "black"
                    border.width: activity.border.width
                    width: activity.width
                    height: activity.height/10-grid1.rowSpacing
                }
                Rectangle {
                    id: rectangle2
                    color: "transparent"
                    border.color: "black"
                    border.width: activity.border.width
                    width: activity.width
                    height: activity.height/10-grid1.rowSpacing
                }

                Rectangle {
                    id: rectangle1
                    color: "transparent"
                    border.color: "black"
                    border.width: activity.border.width
                    width: activity.width
                    height: activity.height/10
                }
            }
	    }
	}		
}
