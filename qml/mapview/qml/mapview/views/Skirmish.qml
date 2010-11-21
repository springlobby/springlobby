import Qt 4.7

Item {
	id: skirmishView
	width: parent.width - 24
	height: parent.height - 24
	anchors.horizontalCenter: parent.horizontalCenter
	MinimapList {
		id: minimapList
		width:  350
		anchors.right: parent.right
		anchors.margins: 10

		height: parent.height
		focus: true
		opacity: 1
	}

	states: [
		State {
			name: "Offscreen"
			PropertyChanges { target: skirmishView; 	anchors.topMargin:  4000 }
			AnchorChanges { target: skirmishView; anchors {top: mainContainer.bottom } }

		},
		State {
			name: "Onscreen"
			AnchorChanges { target: skirmishView; anchors {top: mainContainer.top } }
			PropertyChanges { target: skirmishView; 	anchors.topMargin:  12 }
		}
	]
	transitions: [
		Transition {
			AnchorAnimation{  easing.type: Easing.InOutQuad; duration: 1000  }
		}
	]
	state:  "Offscreen"
}
