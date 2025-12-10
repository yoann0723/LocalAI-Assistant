import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    width: 480
    height: 640
    title: "QML Chat Demo"

    ListModel {
        id: chatModel
        ListElement { sender: "user"; text: "Hello!" }
        ListElement { sender: "assistant"; text: "Hi! How can I help you?" }
    }

    Column {
        anchors.fill: parent

        Rectangle{
            id: chatViewBg
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: inputBar.top
            color: "#222831"
            ListView {
                id: chatView
                anchors.fill: parent
                model: chatModel

                delegate: Rectangle {
                    width: chatView.width
                    height: textItem.implicitHeight + 20
                    color: "transparent"

                    Rectangle{
                        radius: 12
                        color: model.sender === "user" ? "#4A90E2" : "#7B7B7B"

                        anchors.right: model.sender === "user" ? parent.right : undefined
                        anchors.left: model.sender === "assistant" ? parent.left : undefined
                        anchors.margins: 10

                        implicitWidth: textItem.implicitWidth
                        implicitHeight: textItem.implicitHeight

                        Text {
                            id: textItem
                            anchors.fill: parent
                            text: model.text
                            wrapMode: Text.Wrap
                            padding: 10
                            color: "white"
                        }
                    }
                }
            }
        }

        Rectangle {
            id: inputBar
            height: 50
            width: parent.width
            color: "#333333"
            anchors.bottom: parent.bottom
            Row {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8

                TextField {
                    id: inputField
                    placeholderText: "Type your message..."
                    placeholderTextColor: "#ababab"
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - sendButton.width - 20
                    color: "#cdcdcd"
                    background: Rectangle{
                        radius: 12
                        anchors.fill: parent
                        color: "#303030"
                    }
                }

                Button {
                    id: sendButton
                    text: "Send"
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: {
                        if (inputField.text !== "") {
                            chatModel.append({ sender: "user", text: inputField.text })
                            inputField.text = ""
                            chatView.positionViewAtEnd()
                        }
                    }
                }
            }
        }
    }
}
