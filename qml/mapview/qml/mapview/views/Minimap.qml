import Qt 4.7

Item {
	Image {
		id: imgBG
		source: ""
		anchors.fill: parent
		Behavior on opacity { PropertyAnimation{} }
	}
	Image {
		id: imgBG2
		anchors.fill: parent
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
