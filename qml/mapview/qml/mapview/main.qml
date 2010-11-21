import Qt 4.7
import "views"
import "web"

Rectangle {
	width: 1680
	height: 1050
	anchors.fill: parent
	id: rootElement
	Background { id: bgImage }
	Logo { id: logoImage }
//	MinimapList {
//		id: minimapList
//		width:  350
//		anchors.right: parent.right
//		anchors.margins: 10

//		height: parent.height
//		focus: true
//		opacity: 0.1
//	}

	Item {
		Rectangle {
			anchors.fill: parent
			color: "black"
			opacity: 0.1
			radius: 10
		}
		id: mainContainer
		width: parent.width * 0.7 ;
		height: parent.height * 0.7
		anchors.left: mainMenu.right
		anchors.top: logoImage.bottom
		anchors.margins: 30

		Browser {	id: webBrowser }
	}


	MenuContainer { id: mainMenu}
}
