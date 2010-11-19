import Qt 4.7
import "views"

Rectangle {
	width: 900
	height: 600
	Image {
		id: bgImage
		source: "image://images/background"
		anchors.fill: parent
	}

	MinimapList {
		id: minimapList
	}
}
