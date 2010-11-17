import Qt 4.7

Rectangle {
	width: 500
	height: 600
	Component {
			id: highlight
			Rectangle {
				id: highlightRect
				width: parent.width
				height: 20
				color: "lightsteelblue"; radius: 5
				y: list.currentItem.y
				x: list.currentItem.x
				Behavior on y {
					PropertyAnimation { easing.type: Easing.OutQuad }
				}
				Behavior on x {
					PropertyAnimation { easing.type: Easing.InOutQuad }
				}
			}
		}

		Component {
			id: albumDelegate
			Rectangle {
				x: ListView.isCurrentItem ? 20 : 10
				id: albumDelegateRec
				height: 30
				width: 400
				opacity: ListView.isCurrentItem ? 1.0 : 0.8
				Behavior on opacity {
					PropertyAnimation{}
				}

				color: "#0a92d5"; radius: 4
				gradient: Gradient {
					GradientStop {
						position: 0.00;
						color: "#0a92d5";
					}
					GradientStop {
						position: 1.00;
						color: "#0e7fb3";
					}
				}
				//			scale: activeFocus ? 1.0 : 0.8
				Text {
					id: mapname
					anchors.fill: parent
					anchors.centerIn: parent
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment:Text.AlignVCenter

	//				wrapMod: Text.Wrap
					text: modelData
					color: "white"
				}

				Behavior on x {
					PropertyAnimation{ easing.type: Easing.Linear; duration: 300 }
				}
			}
		}

		ListView {
			id: list
	//		highlight: highlight
			delegate: albumDelegate
			model: myModel
			anchors.fill: parent
			highlightFollowsCurrentItem: true
			focus: true
			keyNavigationWraps :true
			anchors.margins: 10
			spacing: 5
		}
}
