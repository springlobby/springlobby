import Qt 4.7

Image {
	width: 217
	height: 49
	source: "../images/menu_button.png"
	property string text
	Text {
		id: but_text
		anchors.horizontalCenter: parent.horizontalCenter
		text: parent.text
		color: "white"
		font.weight: Font.Bold
		font.capitalization: Font.AllUppercase
		font.pixelSize: 22
		anchors.verticalCenter: parent.verticalCenter
		anchors.verticalCenterOffset: -3
	}
	MouseArea {
		anchors.fill: parent
		hoverEnabled: true
		onEntered: { but_text.color = "red" }
		onExited: { but_text.color = "white"  }
	}
}
