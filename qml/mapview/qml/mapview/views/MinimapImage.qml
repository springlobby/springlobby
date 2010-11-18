import Qt 4.7

Image {
	id: imgBG
	source: ""
	width: parent.width - 20
	height: parent.height - 20
	anchors.verticalCenter: parent.verticalCenter
	anchors.verticalCenter: parent.horizontalCenter
	Behavior on opacity { PropertyAnimation{} }
}
