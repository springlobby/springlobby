import Qt 4.7
import "views"

Rectangle {
	width: 1680
	height: 1080
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
	MenuContainer {}
}
