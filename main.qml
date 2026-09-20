import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

ApplicationWindow {
    visible: true
    width: 450
    height: 600
    title: "To-Do List"
    color: "#1e1e1e"

    SystemTrayIcon {
        id: trayIcon
        visible: true
        tooltip: "To-Do List"
    }

    Connections {
        target: taskManager
        function onNotifyUser(title) {
            trayIcon.showMessage("Час вийшов!", "Завдання: " + title, SystemTrayIcon.Warning, 5000)
            noticeText.text = "⏰ ЧАС ВИЙШОВ!\n" + title
            noticePopup.open()
        }
    }

    Popup {
        id: noticePopup
        x: (parent.width - width) / 2
        y: 20
        width: 320
        height: 70
        modal: false
        focus: true
        closePolicy: Popup.CloseOnPressOutside

        background: Rectangle {
            color: "#dc3545"
            radius: 8
            border.color: "#ff6b6b"
            border.width: 1
        }

        Text {
            id: noticeText
            anchors.centerIn: parent
            color: "white"
            font.bold: true
            font.pixelSize: 13
            horizontalAlignment: Text.AlignHCenter
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            TextField {
                id: taskInput
                placeholderText: "Нове завдання..."
                Layout.fillWidth: true
                color: "white"
                placeholderTextColor: "#888888"
                background: Rectangle {
                    color: "#2b2b2b"
                    border.color: "#444444"
                    radius: 6
                }
            }

            TextField {
                id: dateInput
                placeholderText: "РРРР-ММ-ДД ГГ:ХВ"
                implicitWidth: 135
                color: "white"
                font.pixelSize: 12
                placeholderTextColor: "#888888"
                background: Rectangle {
                    color: "#2b2b2b"
                    border.color: "#444444"
                    radius: 6
                }
            }

            Button {
                id: addButton
                text: "Додати"
                implicitWidth: 80

                contentItem: Text {
                    text: addButton.text
                    font.bold: true
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: addButton.down ? "#0056b3" : "#007bff"
                    radius: 6
                }

                onClicked: {
                    if (taskInput.text.trim().length > 0) {
                        taskManager.addTask(taskInput.text, 1, dateInput.text)
                        taskInput.text = ""
                        dateInput.text = ""
                    }
                }
            }
        }

        TabBar {
            id: filterBar
            Layout.fillWidth: true
            currentIndex: 0

            TabButton { text: "Усі" }
            TabButton { text: "Активні" }
            TabButton { text: "Завершені" }
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: taskManager
            clip: true
            spacing: 4

            delegate: ItemDelegate {
                id: itemDelegate
                width: listView.width

                property bool isMatch: (filterBar.currentIndex === 0) ||
                                      (filterBar.currentIndex === 1 && !model.isCompleted) ||
                                      (filterBar.currentIndex === 2 && model.isCompleted)

                visible: isMatch
                height: isMatch ? implicitHeight : 0
                padding: isMatch ? 8 : 0

                contentItem: RowLayout {
                    spacing: 10

                    CheckBox {
                        checked: model.isCompleted
                        onClicked: taskManager.toggleCompleted(index)
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Text {
                            text: model.title
                            font.strikeout: model.isCompleted
                            font.pixelSize: 14
                            color: model.isCompleted ? "#777777" : "white"
                        }

                        Text {
                            text: model.dueDate !== "" ? "⏳ " + model.dueDate : ""
                            font.pixelSize: 11
                            color: model.isCompleted ? "#777777" : "#ffc107"
                            visible: model.dueDate !== ""
                        }
                    }

                    Button {
                        id: delButton
                        text: "✕"
                        implicitWidth: 32
                        implicitHeight: 32

                        contentItem: Text {
                            text: delButton.text
                            font.bold: true
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: delButton.down ? "#bd2130" : "#dc3545"
                            radius: 6
                        }
                        onClicked: taskManager.removeTask(index)
                    }
                }
            }
        }
    }
}