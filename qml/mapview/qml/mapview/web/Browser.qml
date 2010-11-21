import Qt 4.7

Rectangle {
	property string urlString : "http://www.evolutionrts.info/"

	width: parent.width * 0.7 ; height: parent.height * 0.7
	color: "#343434"
	anchors.bottom: parent.bottom
	anchors.right: parent.right
	anchors.margins: 0.08 * parent.width
	Item { id: headerSpace; width: parent.width; height: 1 }

	Header {
		id: header
		editUrl: webBrowser.urlString
		width: headerSpace.width; height: headerSpace.height
	}
	FlickableWebView {
		id: webView
		url: webBrowser.urlString
		onProgressChanged: header.urlChanged = false
		anchors { top: headerSpace.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
	}

	ScrollBar {
		scrollArea: webView; width: 8
		anchors { right: parent.right; top: header.bottom; bottom: parent.bottom }
	}

	ScrollBar {
		scrollArea: webView; height: 8; orientation: Qt.Horizontal
		anchors { right: parent.right; rightMargin: 8; left: parent.left; bottom: parent.bottom }
	}
}
