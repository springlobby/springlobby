import Qt 4.7

Item {
	Rectangle {
		color: "black"
		radius: 5
		anchors.fill: parent
		opacity: 0.6
	}
		Image {
			id: imgBG
			source: ""
			width: parent.width - 20
			height: parent.height - 20
			anchors.centerIn: parent
			Behavior on opacity { PropertyAnimation{} }
		}

		Image {
			id: imgBG2
			width: parent.width - 20
			height: parent.height - 20
			anchors.centerIn: parent
			source: ""
			Behavior on opacity { PropertyAnimation{} }
		}


	property bool activeImage: false
	function load( mapname ) {
		if ( activeImage ) {
			imgBG2.opacity = 0.0
			imgBG.source = "image://minimaps/" + mapname
			imgBG.opacity = 1.0
		}
		else {
			imgBG.opacity = 0.0
			imgBG2.source = "image://minimaps/" + mapname
			imgBG2.opacity = 1.0
		}
		activeImage = !activeImage
	}
}
