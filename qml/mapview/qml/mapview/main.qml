import Qt 4.7

Rectangle {
	width: 900
	height: 600

	Component {
		id: mapDelegate
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
				text: modelData
				color: "white"
			}
			MouseArea{
				anchors.fill: parent
				onClicked: {  list.currentIndex = index }
			}

			Behavior on x {
				PropertyAnimation{ easing.type: Easing.Linear; duration: 300 }
			}
		}
	}
	Image {
		//use the dummy image provider which currently always returns the bg image
		source: "image://images/red"
	}
	ListView {
		id: list
		delegate: mapDelegate
		model: myModel
		anchors.fill: parent
		highlightFollowsCurrentItem: false
		focus: true
		keyNavigationWraps :true
		anchors.margins: 10
		spacing: 5
	}
}
