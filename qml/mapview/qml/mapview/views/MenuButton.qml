import Qt 4.7

Image {
	id: menu_button
	width: 217
	height: 49
	property string text
	Text {
		id: but_text
		anchors.horizontalCenter: parent.horizontalCenter
		text: parent.text
		color: "white"
		font.weight: Font.Bold
		font.capitalization: Font.AllUppercase
		font.pixelSize: 22
		anchors.verticalCenter: parent.verticalCenter
		anchors.verticalCenterOffset: -3
	}
	MouseArea {
		anchors.fill: parent
		hoverEnabled: true
		onEntered: { parent.state = "rollover" }
		onExited: { parent.state = "plain"  }
//		onClicked: { parent.state = "clicked"  }
	}
	states: [
		State {
			name: "plain"
			PropertyChanges { target: menu_button; source: "../images/menu_button.png" }
		},
		State {
			name: "rollover"
			PropertyChanges { target: menu_button; source: "../images/menu_button_rollover.png" }
		},
		State {
			name: "clicked"
			PropertyChanges { target: menu_button; source: "../images/menu_button_clicked.png" }
		}
	]
	state: "plain"
//	transitions: [
//		Transition {

//		}
//	]
}
