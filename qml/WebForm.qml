import QtQuick 2.9
import QtQuick.Controls 2.2
import QtWebView 1.1

Page {
    width: 600
    height: 400
    id: mainHolder

    property int pageNumber: 10

    // private property
    property bool webInitialized : false;

    signal url_titleChanged(string title);

    WebView {
        id: webView
        anchors.fill: parent; // use a flickable to allow scrolling
//        contentWidth: width; // flickable content width is its own width, scroll only vertically
//        contentHeight: layout.height; // content height is the height of the layout of items

        url: settingsHelper.defaultPageForID( mainHolder.pageNumber )

        onUrlChanged: {
            settingsHelper.cacheDefaultPageForID( mainHolder.pageNumber, webView.url )
        }

        onTitleChanged: {
            url_titleChanged(webView.title)
        }
    }

    Connections {
        target: serviceDiscover

        function onJmriWebserverFound( url ){
            if( !settingsHelper.isUsingDefaultPageForID(mainHolder.pageNumber) ){
                webInitialized = true
            }

            if( webInitialized ) return;

            console.log( "Setting webview to load " + url )
            webView.url = url
            webInitialized = true
        }
    }
}
