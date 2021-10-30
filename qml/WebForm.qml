import QtQuick 2.12
import QtQuick.Controls 2.5
import QtWebView 1.1

Page {
    width: 600
    height: 400

    property bool webInitialized : false;

    WebView {
        id: webView
        anchors.fill: parent; // use a flickable to allow scrolling
//        contentWidth: width; // flickable content width is its own width, scroll only vertically
//        contentHeight: layout.height; // content height is the height of the layout of items

        url: initialUrl
    }

    Connections {
        target: serviceDiscover

        function onJmriWebserverFound( url ){
            if( webInitialized ) return;

            console.log( "Setting webview to load " + url )
            webView.url = url
            webInitialized = true
        }
    }
}
