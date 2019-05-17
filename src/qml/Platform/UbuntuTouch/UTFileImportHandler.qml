import QtQuick 2.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3 as Popups
import Ubuntu.Content 1.3

Popups.PopupBase {
    id: picker
    objectName: "contentPickerDialog"
    z:2

    // Set the parent at construction time, instead of letting show()
    // set it later on, which for some reason results in the size of
    // the dialog not being updated.
    //parent: QuickUtils.rootItem(this)

    property var activeTransfer: null
    property bool allowMultipleFiles
    property var contentHandler

    signal accept(var files)
    signal reject()

    onAccept: hide()
    onReject: hide()

    Rectangle {
        anchors.fill: parent

        ContentTransferHint {
            anchors.fill: parent
            activeTransfer: picker.activeTransfer
        }

        ContentPeerPicker {
            id: peerPicker
            anchors.fill: parent
            visible: true
            contentType: ContentType.Music
            handler: ContentHandler.Source

            onPeerSelected: {

                peer.selectionType = ContentTransfer.Single
                picker.activeTransfer = peer.request()

            }

            onCancelPressed: {
                picker.reject()
            }
        }
    }

    Connections {
        id: stateChangeConnection
        target: picker.activeTransfer
        onStateChanged: {
            //import
            if (picker.activeTransfer.state === ContentTransfer.Charged) {
                var selectedItems = []
                for(var i in picker.activeTransfer.items) {

                    selectedItems.push(String(picker.activeTransfer.items[i].url).replace("file://", ""))
                }
                //utPicker.storeFiles(selectedItems)
                picker.accept(selectedItems)
                picker.hide()
            }





        }
    }





}
