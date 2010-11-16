import Qt 4.7

Rectangle {
    width: 360
    height: 360
    Text {
		text: myAwesomeString.value()
        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
}
