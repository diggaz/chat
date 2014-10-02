import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

Rectangle {

    signal sendMessage(string msg)
    signal requestHistory(var date)

    function addMessage(name, message, date) {

        if (chatModel.count > 0) { // FIX dumb insert. replace with custom model with sorting and hookers
            if (date > chatModel.get(chatModel.count - 1).date)
                chatModel.append({"name":name, "message":message, "date":date})
            else {
                for(var i = 0; i < chatModel.count; i++) {
                    if (date < chatModel.get(i).date) {
                        chatModel.insert(i, {"name":name, "message":message, "date":date});
                        break;
                    }
                }
            }
        }
        else
            chatModel.append({"name":name, "message":message, "date":date})

        chatView.currentIndex = chatModel.count - 1
    }

    function addUser(name) {
        userModel.append({"name":name})
    }

    function addUsers(users) {
        for(var i = 0; i < users.length; i++) {
            userModel.append({"name": users[i]})
        }
    }

    function removeUser(name) {
        userModel.remove({"name":name})
    }

    ListModel { id: chatModel }
    ListModel { id: userModel }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        SplitView {
            orientation: Qt.Vertical
            Layout.fillHeight: true
            Layout.fillWidth: true

            ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.fillHeight: true
                Layout.fillWidth: true

                Button {
                    id: button
                    implicitHeight: 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Загрузить предыдущие 10 сообщений")

                    onClicked: {
                        if (chatModel.count > 0)
                            requestHistory(chatModel.get(0).date);
                        else
                            requestHistory(new Date());
                    }
                }

                ScrollView {
                    id: chatScroll
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    ListView {
                        id: chatView
                        model: chatModel
                        delegate: Rectangle {
                            implicitHeight: message.height + 5
                            //height: 20
                            Item {
                                Text {
                                    id: nameDate
                                    color: "#5e60d5"
                                    font.pointSize: 10
                                    text: name + " (" + date.toLocaleTimeString() + "): "
                                }

                                Text {
                                    id: message
                                    anchors.left: nameDate.right
                                    font.pointSize: 10
                                    width: chatView.width
                                    text: model.message
                                    wrapMode: Text.WrapAnywhere
                                }
                            }
                        }
                    }
                }
            }

            RowLayout {
                id: bottomRow
                Layout.minimumHeight: 50

                function send() {
                    if (inputField.text != "") {
                        sendMessage(inputField.text)
                        inputField.text = ""
                    }
                }

                TextField {
                    id: inputField
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onAccepted: bottomRow.send()
                }

                Button {
                    implicitWidth: 100
                    Layout.fillHeight: true
                    text: qsTr("Отправить")
                    Keys.onReturnPressed: bottomRow.send()
                    onClicked: bottomRow.send()
                }
            }
        }

        Rectangle {
            anchors.right: parent.right
            border.width: 1
            border.color: "black"
            Layout.fillHeight: true
            Layout.minimumWidth: 100

            ListView {
                anchors.fill: parent
                anchors.margins: 2
                Layout.fillHeight: true
                model: userModel
                delegate: Component {
                    Text {
                        font.pointSize: 10
                        text: model.name
                        color: "black"
                    }
                }
            }
        }
    }

}
