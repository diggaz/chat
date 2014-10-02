import QtQuick 2.0
import QtQml.Models 2.1
import Chat 1.0
import "./content"

ListView {
    id: root
    width: 550
    height: 480
    snapMode: ListView.SnapOneItem
    focus: false
    orientation: ListView.Horizontal
    boundsBehavior: Flickable.StopAtBounds
    currentIndex: 0
    interactive: false

    ChatClient{
        id: chatClient

        onJoinedChat: {
            root.currentIndex = 1
            console.log("joined chat")
        }
        onDisconnected: {
            root.currentIndex = 0
            console.log("disconnected")
        }
        onMessageReceived: {
            chatView.addMessage(name, message, dt)
        }
        onUserLoginReceived: {
            chatView.addUser(name)
        }
        onUserLogoutReceived: {
            chatView.removeUser(name)
        }
        onUserListReceived: {
            chatView.addUsers(users)
        }
        onError: {
            loginView.error = errorMessage
        }
    }

    model: ObjectModel {
        LoginView {
            id: loginView
            width: root.width
            height: root.height
            name: chatClient.name
            host: chatClient.host
            port: chatClient.port

            onConnectClicked: {
                chatClient.name = name
                chatClient.host = host
                chatClient.port = port
                chatClient.connectToServer()
            }
        }

        ChatView {
            id: chatView
            width: root.width
            height: root.height

            onSendMessage: {
                chatClient.sendMessage(msg);
            }
            onRequestHistory: {
                chatClient.requestHistory(date)
            }
        }
    }
}
