import Qt 4.7

Column {
	width: 225
	anchors.left: parent.left
	anchors.leftMargin: 0.08 * parent.width
	spacing: 20
	anchors.verticalCenter:  parent.verticalCenter

	MenuButton{ text: "Skirmish" }
	MenuButton{ text: "Play online" }
	MenuButton{ text: "Settings" }
	MenuButton{ text: "Update" }
	MenuButton{ text: "How to play" }
	MenuButton{ text: "Website"
		onClicked :{
			webBrowser.state = "Onscreen"
		}
	}
	MenuButton{ text: "Quit"; onClicked: Qt.quit() }

	add: Transition {
			 NumberAnimation {
				 properties: "y"
				 easing.type: Easing.OutBounce
				 duration: 2000

			 }
		 }
}
