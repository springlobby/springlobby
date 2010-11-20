import Qt 4.7

Image {
	width: 217
	height: 49
	source: "../images/menu_button.png"
	property string text
	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		text: parent.text
		color: "white"
		font.weight: Font.Bold
		font.capitalization: Font.AllUppercase
		font.pixelSize: 22
		anchors.verticalCenter: parent.verticalCenter
		anchors.verticalCenterOffset: -3
	}
}
