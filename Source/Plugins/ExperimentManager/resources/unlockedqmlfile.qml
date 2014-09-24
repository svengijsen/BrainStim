import QtQuick 2.0

Rectangle {
    color: "#575757"
    Text {
        color: "#ffffff"
        text: qsTr("Experiment ready, waiting for a trigger to start...")
        wrapMode: Text.WordWrap
        font.bold: true
        font.pointSize: 22
        font.family: "Arial"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.centerIn: parent
        renderType: Text.NativeRendering
    }
    //MouseArea {
    //    anchors.fill: parent
    //    onClicked: {
    //        Qt.quit();
    //    }
    //}
}
