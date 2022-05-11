import QtQuick 2.0
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import org.nvmr.videodisplay 1.0

Item {
    ColumnLayout{
        RowLayout {
            Text {
                text: "Tab 1"
            }

            Button {
                text: "Use current page as default"
                onClicked: settingsHelper.saveDefaultPageForID(0)
            }
            Button {
                text: "Clear default page"
                onClicked: settingsHelper.clearDefaultPageForID(0)
            }
        }

        RowLayout {
            Text {
                text: "Tab 2"
            }

            Button {
                text: "Use current page as default"
                onClicked: settingsHelper.saveDefaultPageForID(1)
            }
            Button {
                text: "Clear default page"
                onClicked: settingsHelper.clearDefaultPageForID(1)
            }
        }

        RowLayout {
            Text {
                text: "Videos"
            }

            Button {
                text: "Use current layout and videos as default"
    //            onClicked: model.submit()
            }
            Button {
                text: "Clear defaults"
    //            onClicked: model.revert()
            }
        }
    }
}
