import Qt 4.7
import "views"
import "web"

Rectangle {
	width: 1680
	height: 1050
	anchors.fill: parent
	Background {}
//	MinimapList {
//		id: minimapList
//		width:  350
//		anchors.right: parent.right
//		anchors.margins: 10

//		height: parent.height
//		focus: true
//		opacity: 0.1
//	}

	Browser {
		opacity: 0.0

		Behavior on opacity {
			PropertyAnimation{ duration: 4000 }
		}
		id: webBrowser
	}

	MenuContainer {}
}
