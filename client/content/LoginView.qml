import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.1

Item {

    signal connectClicked

    property alias name: loginField.text
    property alias host: hostField.text
    property alias port: portField.text
    property alias error: errorText.text

    RowLayout {
        id: loginRow
        anchors.centerIn: parent

        ColumnLayout {
            id: inputColumn
            anchors.fill: parent

            TextField {
                id: loginField
                Layout.fillWidth: true
                placeholderText: qsTr("Input name")
                text: name
            }

            RowLayout {
                TextField {
                    id: hostField
                    Layout.fillWidth: true
                    text: host
                    placeholderText: qsTr("Input server host")
                }

                TextField {
                    id: portField
                    placeholderText: qsTr("Input server port")
                    text: port
                }
            }
        }
        Button {
            implicitHeight: inputColumn.height
            text: qsTr("Войти")

            onClicked: {
                errorText.text = ""
                if (loginField.text != "" &&
                        hostField.text != "" &&
                        portField.text != "") {
                    connectClicked()
                }
                // TODO: need to show user that is wrong
            }
        }
    }

    Text {
        width: 250
        id: errorText
        anchors.top: loginRow.bottom
        anchors.horizontalCenter: loginRow.horizontalCenter
        anchors.margins: 5
        wrapMode: Text.WordWrap
        color: "red"
    }
}
