import Qt 4.7

Column {
	width: 225
	anchors.left: parent.left
	anchors.leftMargin: 0.08 * parent.width
	spacing: 20
	anchors.verticalCenter:  parent.verticalCenter
	Repeater {
		model: 6
		MenuButton{ text: "I'm item " + index }
	}

	add: Transition {
			 NumberAnimation {
				 properties: "y"
				 easing.type: Easing.OutBounce
				 duration: 2000

			 }
		 }
}
