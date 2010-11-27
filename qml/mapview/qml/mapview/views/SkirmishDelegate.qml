import Qt 4.7

Item {
    width: 100
	height: 35

	property int max_width
	property int max_height

	Rectangle {
		id: bgRect
		anchors.centerIn: parent
		color: "blue"
		radius: 5
		opacity: 0.4
		anchors.fill: parent
	}

	Text {
		id: name
		text:  display
		color: "white"
		anchors.centerIn: parent
	}

	Item {
		y: (index) * -(parent.height +4) - 20
		x: bgRect.x + 100

		width: parent.max_width
		height: parent.max_height
		Rectangle {
			color: "white"
			opacity: 0.6
			anchors.fill: parent
		}
		Text {
			text:  display
		}
	}

}
