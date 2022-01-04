import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.0
import BibleTime.Mini 1.0

Rectangle {
    id:root
    width: 480
    height: 800

    property color colorBase:   '#ffffff'
    property color colorAccent: '#ff8800'
    property color colorText:   '#000000'
    property real speedMenu: 100
    property real tileWidth: 200


    Rectangle {
        width: root.width
        height: 80
        color: root.colorAccent
        id:panel
        state: 'default'

        states: [
            State {
                name: 'default'
            },
            State {
                name: 'down'
            },
            State {
                name: 'left'
            }
        ]

        transitions: [
            Transition {
                from: "default"
                to: "down"
                SequentialAnimation {
                    PropertyAction { target: fade; property: 'enabled'; value: true; }
                    ParallelAnimation {
                        PropertyAnimation { target: panel; property: 'y'; to: 600; duration: 300; easing.type: Easing.OutCubic }
                        PropertyAnimation { target: fade; property: 'opacity'; from: 0; to: 0.5; duration: 300; easing.type: Easing.OutCubic }
                    }
                }
            },
            Transition {
                from: "down"
                to: "default"
                SequentialAnimation {
                    ParallelAnimation{
                        PropertyAnimation{ target: fade; property: 'opacity'; to: 0; duration: 300; easing.type: Easing.OutCubic }
                        PropertyAnimation { target: panel; property: 'y'; to: 0; duration: 300; easing.type: Easing.OutCubic }
                    }
                    PropertyAction { target: fade; property: 'enabled'; value: false; }
                }
            },
            Transition {
                from: "default"
                to: "left"
                SequentialAnimation {
                    PropertyAnimation { target: panel; property: 'x'; to: -root.width; duration: 300; easing.type: Easing.OutCubic }
                }
            },
            Transition {
                from: "left"
                to: "default"
                PropertyAnimation { target: panel; property: 'x'; to: 0; duration: 300; easing.type: Easing.OutCubic }
            }
        ]

        Rectangle {
            width: Math.max(moduleText.width + (moduleText.x * 2), panel.height * 1.5)
            height: panel.height
            color: parent.color
            id: moduleRect

            Text {
                id: moduleText
                x: panel.height / 5
                y: panel.height / 2
                font.pixelSize: panel.height / 3
                color: root.colorBase
                text: 'KJV'
                font.family: "Tahoma"
            }

            MouseArea {
                anchors.fill: parent
                onPressed: PropertyAnimation { target: moduleRect; property: 'color'; to: root.colorText }
                onReleased: PropertyAnimation { target: moduleRect; property: 'color'; to: root.colorAccent }
                onClicked: panel.state = 'down'
            }
        }

        Rectangle {
            x: moduleRect.width
            width: Math.max(placeText.width + (placeText.x * 2), panel.height * 1.5)
            height: panel.height
            color: parent.color
            id: placeRect

            Text {
                id: placeText
                x: panel.height / 5
                y: panel.height / 2
                font.pixelSize: panel.height / 3
                color: root.colorBase
                text: "Matt 5:1"
                font.family: "Tahoma"
            }

            MouseArea {
                anchors.fill: parent
                onPressed: PropertyAnimation { target: placeRect; property: 'color'; to: root.colorText }
                onReleased: PropertyAnimation { target: placeRect; property: 'color'; to: root.colorAccent }
                onClicked: panel.state = 'down'
            }
        }

        Rectangle {
            width: panel.height
            x: panel.width - width
            height: panel.height
            color: parent.color
            id: controlRect

            Rectangle {
                id: controlPath
                width: parent.width * 0.8
                height: parent.height * 0.8
                anchors.centerIn: parent
                radius: width * 0.5
                border.color: root.colorBase
                border.width: 4
                color: 'transparent'
            }

            MouseArea {
                anchors.fill: parent
                onPressed: PropertyAnimation { target: controlRect; property: 'color'; to: root.colorText }
                onReleased: PropertyAnimation { target: controlRect; property: 'color'; to: root.colorAccent }
                onClicked: {
                    menuFindRect.state == 'on' ? menuFindRect.state = 'off' : menuFindRect.state = 'on';
                    fade.enabled = menuFindRect.state == 'on'
                }
            }
        }
    }

    WorkerScript {
        id: worker

        source: "worker.js"

        onMessage: {
            messageObject.item.text = 'Hello'
        }
    }

    Rectangle {
        width: root.width
        height: root.height - panel.height
        y: panel.y + panel.height
        x: panel.x
        id:text
        color: root.colorBase

        SwordModuleModel {
            module: 'ESV'
            id: swordModel
        }

        ListView {
            anchors.fill: parent
            anchors.leftMargin: 5
            model: swordModel
            clip: true
            id: list
            highlightFollowsCurrentItem: true

            delegate: Rectangle {
                radius: 5
                width: root.width - 10
                height: textDelegate.height
                color: 'transparent'

                Text {
                    id: textDelegate
                    textFormat: Text.RichText
                    width: parent.width
                    font.pixelSize: 32
                    text: previewUpdate
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

//                    Component.onCompleted: {
//                        worker.sendMessage( { item: this, index: index } )
//                    }
                }

//            delegate: Component {
//                Loader {
//                    asynchronous : true
//                    source: 'item.qml'
//                }
//            }
            }
        }

        Component.onCompleted: {
            list.positionViewAtIndex(swordModel.keyIndex('Matt 5:1'), ListView.Beginning)
        }
    }

//        // work to make dynamical list
//    Rectangle {
//        width: root.width
//        height: root.height - panel.height
//        y: panel.y + panel.height
//        x: panel.x
//        id:text
//        color: root.colorBase

//        Flickable {
//            anchors.fill: parent
//            anchors.topMargin: 5
//            anchors.leftMargin: 5
//            id: list
//            clip: true

//            property int itemId: 0

//            Component {
//                id: flickableItem
//                Rectangle {
//                    property int listId
//                    radius: 5
//                    width: root.width - 10
//                    height: 100
//                    color: '#'+((1<<24)*(Math.random()+1)|0).toString(16).substr(1)
//                    Text{
//                        color: 'white'
//                        anchors.centerIn: parent
//                        font.pixelSize: parent.height / 2
//                        text: parent.listId
//                    }
//                }
//            }

//            function insertItem(id) {
//                var item = flickableItem.createObject(list.contentItem);
//                item.y = 105 * list.itemId
//                list.contentHeight = item.y + (item.height + 5)
//                item.listId = itemId += 1
//             }

//             Component.onCompleted: {
//                 for(var i = 1; i < 100; ++i)
//                    insertItem(0)
//             }
//         }
//    }


//    FastBlur {
//        x: panel.x
//        y: panel.y
//        anchors.fill: root
//        cached: true
//        radius: 32.0
//        source: root
//    }


    Rectangle {
        id: bottomPanel
        x: text.x
        y: text.y + text.height - height
        width: text.width
        height: defaultHeight
        color: root.colorText
        opacity: 0.9

        property int defaultHeight: 30
        property int buttonIndent: 33
        property int buttonHeight: 70

        function raiseHide() {
            if (bottomPanelAnimation.running)
                return

            var h = defaultHeight

            // calculate height
            for(var index = 0; index < children.length; ++index) {
                if (children[index].hasOwnProperty('panelButton'))
                    h += children[index].height
            }

            if (height == defaultHeight)
                height = h
            else
                height = defaultHeight
        }

        Behavior on height { PropertyAnimation { id: bottomPanelAnimation; easing.type: Easing.OutCubic } }

        Rectangle {
            x: parent.buttonIndent
            y: parent.defaultHeight
            width: parent.width - bottomPanelOpener.width
            height: parent.buttonHeight
            color: 'transparent'

            property bool panelButton: true

            Text {
                anchors.fill: parent
                font.pixelSize: parent.height / 2
                color: root.colorBase
                text: 'Find'
            }

            MouseArea {
                anchors.fill: parent
                onClicked: SequentialAnimation {
                    ScriptAction { script: bottomPanel.raiseHide() }
                    PauseAnimation { duration: bottomPanelAnimation.duration }
                    PropertyAction { target: overlaper; property: 'state'; value: 'on' }
                }
            }
        }

        Rectangle {
            x: parent.buttonIndent
            y: parent.defaultHeight + parent.buttonHeight
            width: parent.width - bottomPanelOpener.width
            height: parent.buttonHeight
            color: 'transparent'

            property bool panelButton: true

            Text {
                anchors.fill: parent
                font.pixelSize: parent.height / 2
                color: root.colorBase
                text: 'Install'
            }

            MouseArea {
                anchors.fill: parent
                onClicked: SequentialAnimation {
                    ScriptAction { script: bottomPanel.raiseHide() }
                    PauseAnimation { duration: bottomPanelAnimation.duration }
                    PropertyAction { target: overlaper; property: 'state'; value: 'on' }
                }
            }
        }

        Rectangle {
            x: parent.buttonIndent
            y: parent.defaultHeight + parent.buttonHeight + parent.buttonHeight
            width: parent.width - bottomPanelOpener.width
            height: parent.buttonHeight
            color: 'transparent'

            property bool panelButton: true

            Text {
                anchors.fill: parent
                font.pixelSize: parent.height / 2
                color: root.colorBase
                text: 'Clippings'
            }

            MouseArea {
                anchors.fill: parent
                onClicked: SequentialAnimation {
                    ScriptAction { script: bottomPanel.raiseHide() }
                    PauseAnimation { duration: bottomPanelAnimation.duration }
                    PropertyAction { target: overlaper; property: 'state'; value: 'on' }
                }
            }
        }

        Rectangle {
            x: parent.buttonIndent
            y: parent.defaultHeight + (parent.buttonHeight * 3)
            width: parent.width - bottomPanelOpener.width
            height: parent.buttonHeight
            color: 'transparent'

            property bool panelButton: true

            Text {
                anchors.fill: parent
                font.pixelSize: parent.height / 2
                color: root.colorBase
                text: 'Options'
            }

            MouseArea {
                anchors.fill: parent
                onClicked: SequentialAnimation {
                    ScriptAction { script: bottomPanel.raiseHide() }
                    PauseAnimation { duration: bottomPanelAnimation.duration }
                    PropertyAction { target: overlaper; property: 'state'; value: 'on' }
                }
            }
        }

        Rectangle {
            id: bottomPanelOpener
            anchors.fill: parent
            anchors.leftMargin: parent.width - 80
            color: 'transparent'

            Rectangle {
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: (bottomPanel.defaultHeight - height) / 2
                anchors.horizontalCenterOffset: 15
                width: 7
                height: width
                radius: width / 2
                color: root.colorBase
            }

            Rectangle {
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: (bottomPanel.defaultHeight - height) / 2
                width: 7
                height: width
                radius: width / 2
                color: root.colorBase
            }

            Rectangle {
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: (bottomPanel.defaultHeight - height) / 2
                anchors.horizontalCenterOffset: -15
                width: 7
                height: width
                radius: width / 2
                color: root.colorBase
            }

            MouseArea {
                anchors.fill: parent
                onClicked: bottomPanel.raiseHide()
                onPressed: bottomPanel.raiseHide()
                onPositionChanged: bottomPanel.height == bottomPanel.defaultHeight && bottomPanel.raiseHide()
            }
        }
    }



    Rectangle {
        id: fade
        x: panel.x
        y: panel.y
        width: parent.width
        height: parent.height
        color: '#000000' //root.colorText
        opacity: enabled ? 0.5 : 0;
        enabled: false


        MouseArea {
            anchors.fill: parent
            onClicked: {
                menuFindRect.state = 'off'
                panel.state = 'default'
                parent.enabled = false
            }
        }
    }

    Rectangle {
        id: overlaper
        anchors.fill: root
        color: root.colorAccent
        state: 'off'
        visible: overlaperRotation.angle != -90

        states: [
            State {
                name: 'on'
            },
            State {
                name: 'off'
            }
        ]

        transitions: [
            Transition {
                from: 'on'
                to: 'off'
                ParallelAnimation {
                    PropertyAnimation { target: overlaperRotation; property: 'angle'; to: -90; duration: 300; easing.type: Easing.InCubic}
                    PropertyAnimation { target: fade; property: 'opacity'; from: 0.5; to: 0; duration: 600 }
                }
            },
            Transition {
                from: 'off'
                to: 'on'
                SequentialAnimation {
                    PropertyAnimation { target: fade; property: 'opacity'; from: 0; to: 0.3; duration: 150 }
                    PropertyAnimation { target: overlaperRotation; property: 'angle'; to: 0; duration: 400; easing.type: Easing.OutCubic }
                }
            }
        ]

        transform: Rotation {
            id: overlaperRotation
            angle: -90
            axis.x: 0
            axis.y: 1
            axis.z: 0
            origin.x: root.x
            origin.y: root.y + (root.height / 2)
        }

        MouseArea {
            enabled: overlaper.state == 'on'
            anchors.fill: overlaper
            onClicked: overlaper.state = 'off'
        }

        Rectangle {
            x: 10
            y: 10
            width: root.tileWidth
            height: width
        }
        Rectangle {
            x: root.tileWidth + 20
            y: 10
            width: root.tileWidth
            height: width
        }
        Rectangle {
            x: 10
            y: root.tileWidth + 20
            width: root.tileWidth
            height: width
        }
        Rectangle {
            x: root.tileWidth + 20
            y: root.tileWidth + 20
            width: root.tileWidth
            height: width
        }
        Text {
            x: 10
            y: root.tileWidth + root.tileWidth + 20
            color: root.colorBase
            text: 'Test'
            font.pixelSize: 34
        }
    }

    Rectangle {
        id: menuFindRect
        x: panel.x + panel.width - width
        y: panel.y + panel.height
        width: 0
        height: width
        color: panel.color
        state: 'off'

        states: [
            State {
                name: 'on'
            },
            State {
                name: 'off'
            }
        ]

        transitions: [
            Transition {
                from: 'on'
                to: 'off'
                SequentialAnimation {
                    PropertyAction { target: menuClippingsRect; property: 'state'; value: 'off'; }
                    PauseAnimation { duration: root.speedMenu }
                    PropertyAction { target: menuInstallRect; property: 'state'; value: 'off'; }
                    PropertyAction { target: menuOptionsRect; property: 'state'; value: 'off'; }
                    PauseAnimation { duration: root.speedMenu }
                    PropertyAnimation { target: menuFindRect; property: 'width'; to: 0; duration: root.speedMenu; }
                }
            },
            Transition {
                from: 'off'
                to: 'on'
                SequentialAnimation {
                    PropertyAnimation { target: menuFindRect; property: 'width'; to: root.tileWidth; duration: root.speedMenu; }
                    PropertyAction { target: menuInstallRect; property: 'state'; value: 'on'; }
                    PropertyAction { target: menuOptionsRect; property: 'state'; value: 'on'; }
                    PauseAnimation { duration: root.speedMenu }
                    PropertyAction { target: menuClippingsRect; property: 'state'; value: 'on'; }
                }
            }
        ]

        Text {
            text: 'Find'
            color: root.colorBase
            font.pixelSize: parent.width / 5.5
            x: parent.width * 0.1
            y: parent.height * 0.65
            visible: parent.width > 50
        }

        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2.5
            width: parent.width * 0.3
            height: parent.height * 0.3
            radius: width * 0.5
            border.color: root.colorBase
            border.width: 4
            color: "#00000000"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: PropertyAnimation { target: menuFindRect; property: 'color'; to: root.colorText }
            onReleased: PropertyAnimation { target: menuFindRect; property: 'color'; to: root.colorAccent }
            onClicked: { panel.state = 'left'; menuFindRect.state = 'off'; fade.enabled = false }
        }
    }

    Rectangle {
        id: menuInstallRect
        x: menuFindRect.x - width
        y: menuFindRect.y
        width: 0
        height: width
        color: panel.color
        state: 'off'

        states: [
            State {
                name: 'on'
            },
            State {
                name: 'off'
            }
        ]

        transitions: [
            Transition {
                from: 'on'
                to: 'off'
                SequentialAnimation {
                    PropertyAnimation { target: menuInstallRect; property: 'width'; to: 0; duration: root.speedMenu; }
                }
            },
            Transition {
                from: 'off'
                to: 'on'
                SequentialAnimation {
                    PropertyAnimation { target: menuInstallRect; property: 'width'; to: root.tileWidth; duration: root.speedMenu; }
                }
            }
        ]

        Text {
            text: 'Install'
            color: root.colorBase
            font.pixelSize: parent.width / 5.5
            x: parent.width * 0.1
            y: parent.height * 0.65
            visible: parent.width > 50
        }

        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2.5
            width: parent.width * 0.3
            height: parent.height * 0.3
            radius: width * 0.5
            border.color: root.colorBase
            border.width: 4
            color: "#00000000"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: PropertyAnimation { target: menuInstallRect; property: 'color'; to: root.colorText }
            onReleased: PropertyAnimation { target: menuInstallRect; property: 'color'; to: root.colorAccent }
            onClicked: { panel.state = 'left'; menuFindRect.state = 'off'; fade.enabled = false }
        }
    }

    Rectangle {
        id: menuOptionsRect
        x: menuFindRect.x + menuFindRect.width - width
        y: menuFindRect.y + menuFindRect.height
        width: 0
        height: width
        color: panel.color
        state: 'off'

        states: [
            State {
                name: 'on'
            },
            State {
                name: 'off'
            }
        ]

        transitions: [
            Transition {
                from: 'on'
                to: 'off'
                SequentialAnimation {
                    PropertyAnimation { target: menuOptionsRect; property: 'width'; to: 0; duration: root.speedMenu; }
                }
            },
            Transition {
                from: 'off'
                to: 'on'
                SequentialAnimation {
                    PropertyAnimation { target: menuOptionsRect; property: 'width'; to: root.tileWidth; duration: root.speedMenu; }
                }
            }
        ]

        Text {
            text: 'Options'
            color: root.colorBase
            font.pixelSize: parent.width / 5.5
            x: parent.width * 0.1
            y: parent.height * 0.65
            visible: parent.width > 50
        }

        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2.5
            width: parent.width * 0.3
            height: parent.height * 0.3
            radius: width * 0.5
            border.color: root.colorBase
            border.width: 4
            color: "#00000000"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: PropertyAnimation { target: menuOptionsRect; property: 'color'; to: root.colorText }
            onReleased: PropertyAnimation { target: menuOptionsRect; property: 'color'; to: root.colorAccent }
            onClicked: { panel.state = 'left'; menuFindRect.state = 'off'; fade.enabled = false }
        }
    }

    Rectangle {
        id: menuClippingsRect
        x: menuFindRect.x - width
        y: menuFindRect.y + menuFindRect.height
        width: 0
        height: width
        color: panel.color
        state: 'off'

        states: [
            State {
                name: 'on'
            },
            State {
                name: 'off'
            }
        ]

        transitions: [
            Transition {
                from: 'on'
                to: 'off'
                PropertyAnimation { target: menuClippingsRect; property: 'width'; to: 0; duration: root.speedMenu; }
            },
            Transition {
                from: 'off'
                to: 'on'
                PropertyAnimation { target: menuClippingsRect; property: 'width'; to: root.tileWidth; duration: root.speedMenu; }
            }
        ]

        Text {
            text: 'Clippings'
            color: root.colorBase
            font.pixelSize: parent.width / 5.5
            x: parent.width * 0.1
            y: parent.height * 0.65
            visible: parent.width > 50
        }

        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2.5
            width: parent.width * 0.3
            height: parent.height * 0.3
            radius: width * 0.5
            border.color: root.colorBase
            border.width: 4
            color: "#00000000"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: PropertyAnimation { target: menuClippingsRect; property: 'color'; to: root.colorText }
            onReleased: PropertyAnimation { target: menuClippingsRect; property: 'color'; to: root.colorAccent }
            onClicked: { panel.state = 'left'; menuFindRect.state = 'off'; fade.enabled = false }
        }
    }

    Rectangle {
        id: selector
        x: panel.x
        y: panel.y - height
        width: root.width
        height: 600
        color: root.colorAccent

        Flickable {
            anchors.fill: parent
            anchors.topMargin: 30
            contentWidth: 1200;
            //contentHeight: parent.height
            clip: true

            Text {
                x : 5
                y: 10
                text: 'Favorite'
                color: root.colorBase
                font.pixelSize: 28
            }

            GridLayout {
                x: 5
                y: 5 + 34 + 5
                columnSpacing: 5
                rowSpacing: 5
                columns: 2

                Rectangle {
                    id: moduleButton1
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth


                    Text {
                        id: moduleText1
                        text: 'KJV'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 6.0
                        x: parent.width * 0.1
                        y: parent.height * 0.05
                    }

                    Text {
                        text: 'King James Version (1769) with Strongs Numbers and Morphology'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 13
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        width: parent.width - x
                        height: parent.height - y
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            panel.state = 'default';
                            fade.enabled = false;
                            moduleText.text = moduleText1.text
                        }
                        onPressed: PropertyAnimation { target: moduleButton1; property: 'color'; to: root.colorText }
                        onReleased: PropertyAnimation { target: moduleButton1; property: 'color'; to: root.colorBase }
                        onExited: PropertyAnimation { target: moduleButton1; property: 'color'; to: root.colorBase }
                    }
                }

                Rectangle {
                    id: moduleButton2
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth


                    Text {
                        id: moduleText2
                        text: 'RusSynodal'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 6.0
                        x: parent.width * 0.1
                        y: parent.height * 0.05
                    }

                    Text {
                        text: '1876 Russian Synodal Bible'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 13
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        width: parent.width - x
                        height: parent.height - y
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            panel.state = 'default';
                            fade.enabled = false;
                            moduleText.text = moduleText2.text
                        }
                        onPressed: PropertyAnimation { target: moduleButton2; property: 'color'; to: root.colorText }
                        onReleased: PropertyAnimation { target: moduleButton2; property: 'color'; to: root.colorBase }
                        onExited: PropertyAnimation { target: moduleButton2; property: 'color'; to: root.colorBase }
                    }
                }

                Rectangle {
                    id: moduleButton3
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth


                    Text {
                        id: moduleText3
                        text: 'MHC'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 6.0
                        x: parent.width * 0.1
                        y: parent.height * 0.05
                    }

                    Text {
                        text: 'Matthew Henry\'s Complete Commentary on the Whole Bible'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 13
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        width: parent.width - x
                        height: parent.height - y
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            panel.state = 'default';
                            fade.enabled = false;
                            moduleText.text = moduleText3.text
                        }
                        onPressed: PropertyAnimation { target: moduleButton3; property: 'color'; to: root.colorText }
                        onReleased: PropertyAnimation { target: moduleButton3; property: 'color'; to: root.colorBase }
                        onExited: PropertyAnimation { target: moduleButton3; property: 'color'; to: root.colorBase }
                    }
                }

                Rectangle {
                    id: moduleButton4
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth


                    Text {
                        id: moduleText4
                        text: 'ESV'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 6.0
                        x: parent.width * 0.1
                        y: parent.height * 0.05
                    }

                    Text {
                        text: 'English Standard Version'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 13
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        width: parent.width - x
                        height: parent.height - y
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            panel.state = 'default';
                            fade.enabled = false;
                            moduleText.text = moduleText4.text
                        }
                        onPressed: PropertyAnimation { target: moduleButton4; property: 'color'; to: root.colorText }
                        onReleased: PropertyAnimation { target: moduleButton4; property: 'color'; to: root.colorBase }
                        onExited: PropertyAnimation { target: moduleButton4; property: 'color'; to: root.colorBase }
                    }
                }
            }

            Text {
                x : 5 + 450
                y: 10
                text: 'Bibles'
                color: root.colorBase
                font.pixelSize: 28
            }

            GridLayout {
                x: 5 + 450
                y: 5 + 34 + 5
                columnSpacing: 5
                rowSpacing: 5
                columns: 2

                Rectangle {
                    id: moduleButton5
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth


                    Text {
                        id: moduleText5
                        text: 'KJV'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 6.0
                        x: parent.width * 0.1
                        y: parent.height * 0.05
                    }

                    Text {
                        text: 'King James Version (1769) with Strongs Numbers and Morphology'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 13
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        width: parent.width - x
                        height: parent.height - y
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            panel.state = 'default';
                            fade.enabled = false;
                            moduleText.text = moduleText5.text
                        }
                        onPressed: PropertyAnimation { target: moduleButton5; property: 'color'; to: root.colorText }
                        onReleased: PropertyAnimation { target: moduleButton5; property: 'color'; to: root.colorBase }
                        onExited: PropertyAnimation { target: moduleButton5; property: 'color'; to: root.colorBase }
                    }
                }

                Rectangle {
                    id: moduleButton6
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth


                    Text {
                        id: moduleText6
                        text: 'RusSynodal'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 6.0
                        x: parent.width * 0.1
                        y: parent.height * 0.05
                    }

                    Text {
                        text: '1876 Russian Synodal Bible'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 13
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        width: parent.width - x
                        height: parent.height - y
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            panel.state = 'default';
                            fade.enabled = false;
                            moduleText.text = moduleText6.text
                        }
                        onPressed: PropertyAnimation { target: moduleButton6; property: 'color'; to: root.colorText }
                        onReleased: PropertyAnimation { target: moduleButton6; property: 'color'; to: root.colorBase }
                        onExited: PropertyAnimation { target: moduleButton6; property: 'color'; to: root.colorBase }
                    }
                }

                Rectangle {
                    id: moduleButton7
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth


                    Text {
                        id: moduleText7
                        text: 'ESV'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 6.0
                        x: parent.width * 0.1
                        y: parent.height * 0.05
                    }

                    Text {
                        text: 'English Standard Version'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 13
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        width: parent.width - x
                        height: parent.height - y
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            panel.state = 'default';
                            fade.enabled = false;
                            moduleText.text = moduleText7.text
                        }
                        onPressed: PropertyAnimation { target: moduleButton7; property: 'color'; to: root.colorText }
                        onReleased: PropertyAnimation { target: moduleButton7; property: 'color'; to: root.colorBase }
                        onExited: PropertyAnimation { target: moduleButton7; property: 'color'; to: root.colorBase }
                    }
                }
            }

            Text {
                x : 5 + 450 + 450
                y: 10
                text: 'Commentaries'
                color: root.colorBase
                font.pixelSize: 28
            }

            GridLayout {
                x: 5 + 450 + 450
                y: 5 + 34 + 5
                columnSpacing: 5
                rowSpacing: 5
                columns: 2

                Rectangle {
                    id: moduleButton8
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth


                    Text {
                        id: moduleText8
                        text: 'MHC'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 6.0
                        x: parent.width * 0.1
                        y: parent.height * 0.05
                    }

                    Text {
                        text: 'Matthew Henry\'s Complete Commentary on the Whole Bible'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 13
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        width: parent.width - x
                        height: parent.height - y
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            panel.state = 'default';
                            fade.enabled = false;
                            moduleText.text = moduleText8.text
                        }
                        onPressed: PropertyAnimation { target: moduleButton8; property: 'color'; to: root.colorText }
                        onReleased: PropertyAnimation { target: moduleButton8; property: 'color'; to: root.colorBase }
                        onExited: PropertyAnimation { target: moduleButton8; property: 'color'; to: root.colorBase }
                    }
                }
            }
        }
    }

    Rectangle {
        x: panel.x + root.width
        color: root.colorBase
        width: root.width
        height: root.height
        id: findView

        Rectangle {
            x: parent.x
            y: parent.y
            height: panel.height
            width: parent.width
            color: root.colorAccent

            Rectangle {
                anchors.fill: parent
                anchors.rightMargin: panel.height + 5
                anchors.margins: 5
                color: root.colorBase
                y: 10
                x: 5

                TextField {
                    anchors.fill: parent
                    font.pixelSize: 34
                }
            }

            Rectangle {
                id: findControl
                anchors.fill: parent
                anchors.leftMargin: parent.width - panel.height
                color: parent.color

                Rectangle {
                    width: parent.width * 0.8
                    height: parent.height * 0.8
                    anchors.centerIn: parent
                    radius: width * 0.5
                    border.color: root.colorBase
                    border.width: 4
                    color: "#00000000"
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: PropertyAnimation { target: findControl; property: 'color'; to: root.colorText }
                    onReleased: PropertyAnimation { target: findControl; property: 'color'; to: root.colorAccent }
                    onClicked: panel.state = 'default'
                }
            }
        }


        Rectangle {
            id: rotator
            x: 50
            y: parent.height - 200
            width: parent.width - 100
            height: parent.height - 300

            color: root.colorAccent

            transform: Rotation {
                id: rot2
                origin.x: rotator.width / 2;
                origin.y: 0;
                axis.x:1; axis.y:0; axis.z:0
                angle:0

                Behavior on angle { PropertyAnimation{ duration: 700; easing.type: Easing.OutCubic } }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: rot2.angle == 0 ? rot2.angle = 180 : rot2.angle = 0
            }

            Rectangle {
                x: 100
                y: 100
                width: 200
                height : width
                color: root.colorText
                visible: rot2.angle > 90

                transform: Rotation {
                    id: rot
                    origin.x: 200;
                    origin.y: 100;
                    axis.x:0; axis.y:1; axis.z:0
                    angle:0

                    Behavior on angle { PropertyAnimation{ duration: 300 } }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: rot.angle == 0 ? rot.angle = 180 : rot.angle = 0
                }
            }
        }
    }
}
