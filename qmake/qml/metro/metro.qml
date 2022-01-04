import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

Rectangle {
    id: root

    property int size: 2 // tablet = 2, smartphone = 1
    property bool landscape: root.width > root.height
    property string fontUi: 'Segoe WP'
    property string fontText: 'Book Antiqua'
    property color accentColor: '#09e'
    property color baseColor: '#222'
    property color textColor: 'white'

    color: baseColor

    states: [ State { name: 'default' }, State { name: 'navigation' } ]

    Component.onCompleted: {
        //console.log('available fonts', Qt.fontFamilies())

         size == 1 ? width = 480 : width = 1280
         height = 800
    }

    // try to load supplied fonts
    FontLoader { source: 'SegoeWP.ttf' }
    FontLoader { source: 'BkAnt.ttf' }

    ListView {
        id: view

        anchors.fill: root
        anchors.rightMargin: root.landscape ? panel.state == 'minimal' ? panel.minimalHeight : panel.defaultHeight : 0
        anchors.bottomMargin: root.landscape ? 0 : panel.state == 'minimal' ? panel.minimalHeight : panel.defaultHeight
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        model: [ 'KJV', 'ESV', 'RusSynodal' ]
        boundsBehavior: Flickable.DragOverBounds
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightMoveVelocity: view.width * 2

        function arragngeViews() {
            for(var i in contentItem.children)
            {
                var o = contentItem.children[i]
                if(o['size']) {
                    o.size = 1.0 / count
                }
                else
                    console.log('view haven\'t size', i)
            }
        }

        delegate: Item {
            id: viewItem

            property real size: 1.0

            width: view.width * size
            height: root.height

            ListView {
                id: list

                anchors.fill: parent
                anchors.topMargin: parent.children[1].height
                model: [ 'The Sermon on the Mount',
                    'Seeing the crowds, he went up on the mountain, and <b>when</b> he sat down, his disciples came to him.',
                    'And he opened his mouth and <b>taught them, saying</b>:',
                    '“Blessed are the poor in spirit, for theirs is the kingdom of heaven. Blessed are those who mourn, <i><b>for they</b> shall be comforted</i>.',
                    'Blessed are the meek, for they will inherit the earth.',
                    'You are the salt of the earth, but if salt has lost its <i>taste</i>, how shall its saltiness be restored? It is no longer good for anything except to be thrown out and trampled under people\'s feet.',
                    'Blessed are the merciful, for they shall receive mercy.',
                    'Blessed are the pure in heart, for they shall see God.',
                    'Blessed are the peacemakers, for they shall be called <b>sons of God</b>. Blessed are those who are persecuted for righteousness\' sake, for utheirs is the kingdom of heaven.',
                    'Blessed are you when others revile you and persecute you and utter <i>all kinds of evil <b>against you</b></i> falsely on my account. Rejoice and be glad, for your reward is great in heaven, for so they persecuted the prophets who were before you.',
                    'But when you pray, go into your room, close the door and pray to your Father, who is unseen. Then your Father, who sees what is done in secret, will reward you.',
                    'Blessed are you when others revile you and persecute you and utter <i>all kinds of evil <b>against you</b></i> falsely on my account. Rejoice and be glad, for your reward is great in heaven, for so they persecuted the prophets who were before you.',
                    'Blessed are you when others revile you and persecute you and utter <i>all kinds of evil <b>against you</b></i> falsely on my account. Rejoice and be glad, for your reward is great in heaven, for so they persecuted the prophets who were before you.']

                delegate: Item {
                    width: list.width
                    height: children[0].height

                    Text {
                        x: 5
                        width: parent.width - x
                        text: index == 0 ? '<p>&nbsp;</p><h2><center>' + modelData + '</center></h2>' :
                                           '<table width="100%"><tr><td width="' + (root.landscape ? '3%' : '5%') + '"><sup><b style="color: #888888">' + index + '</b></sup></td> <td>' +
                                            modelData + '</td><.tr></table>'
                        font.pixelSize: 28
                        font.family: root.fontText
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        textFormat: Text.RichText
                        color: root.textColor
                    }
                }
            }

            Rectangle {
                id: status

                width: parent.width
                height: panel.state == 'minimal' ? 0 : panel.baseHeight
                color: root.baseColor
                state: 'default'

                states: [
                    State {
                        name: 'default'
                        PropertyChanges {
                            target: status; height: panel.state == 'minimal' ? 0 : panel.baseHeight
                        }
                    },
                    State {
                        name: 'raised'
                        PropertyChanges {
                            target: status; height: panel.state == 'minimal' ? 0 : panel.baseHeight + statusOptions.height
                        }
                    }
                ]

                Behavior on height { PropertyAnimation { } }

                Rectangle {
                    width: children[0].width + children[0].x + 20
                    height: parent.height
                    color: root.baseColor

                    Text {
                        x: 20
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 8
                        text: modelData + ' -  Matt 5:1'
                        font.pixelSize: root.size == 2 ? 24 : 20
                        font.bold: true
                        font.capitalization: Font.AllUppercase
                        font.underline: true
                        font.family: root.fontUi
                        color: root.textColor
                    }
                }

                Column {
                    id: statusOptions

                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: panel.baseHeight
                    anchors.left: parent.left
                    width: parent.width
                    height: childrenRect.height

                    Row {
                        width: parent.width

                        Button {
                            width: height; height: panel.baseHeight
                            icon: 'next.svg'
                            color: root.textColor
                            rotation: 180
                            enabled: index > 0

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    view.decrementCurrentIndex()
                                    status.state = 'default'
                                }
                            }
                        }

                        Item {
                            height: panel.baseHeight
                            width: parent.width - parent.children[0].width - parent.children[2].width //children[0].width

                            Text {
                                anchors.centerIn: parent
                                text: 'Switch'
                                font.pixelSize: panel.baseHeight * 0.4
                                font.family: root.fontUi
                                color: root.textColor
                            }
                        }

                        Button {
                            width: height; height: panel.baseHeight
                            icon: 'next.svg'
                            color: root.textColor
                            enabled: index < view.model.length - 1

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    view.incrementCurrentIndex()
                                    status.state = 'default'
                                }
                            }
                        }
                    }

                    Row {
                        width: parent.width

                        Item {
                            height: panel.baseHeight
                            width: parent.width * 0.35

                            Rectangle {
                                anchors.fill: parent.children[1]
                                anchors.margins: -25
                                anchors.topMargin: -6
                                anchors.bottomMargin: -11
                                border { color: root.textColor ; width: 2 }
                                color: root.baseColor
                            }

                            Text {
                                anchors.centerIn: parent
                                text: 'add left'
                                font.pixelSize: panel.baseHeight * 0.4
                                font.family: root.fontUi
                                color: root.textColor
                            }
                        }

                        Item {
                            height: panel.baseHeight
                            width: parent.width * 0.25

                            Rectangle {
                                anchors.fill: parent.children[1]
                                anchors.margins: -25
                                anchors.topMargin: -6
                                anchors.bottomMargin: -11
                                border { color: root.textColor ; width: 2 }
                                color: root.baseColor
                            }

                            Text {
                                anchors.centerIn: parent
                                text: 'clear'
                                font.pixelSize: panel.baseHeight * 0.4
                                font.family: root.fontUi
                                color: root.textColor
                            }
                        }

                        Item {
                            height: panel.baseHeight
                            width: parent.width * 0.4

                            Rectangle {
                                anchors.fill: parent.children[1]
                                anchors.margins: -25
                                anchors.topMargin: -6
                                anchors.bottomMargin: -11
                                border { color: root.textColor ; width: 2 }
                                color: root.baseColor
                            }

                            Text {
                                anchors.centerIn: parent
                                text: 'add right'
                                font.pixelSize: panel.baseHeight * 0.4
                                font.family: root.fontUi
                                color: root.textColor
                            }
                        }
                    }
                }

                Button {
                    width: height; height: panel.baseHeight
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    rotation: -90
                    color: root.textColor
                    icon: 'settings.svg'

                    MouseArea {
                        anchors.fill: parent
                        onClicked: status.state == 'raised' ? status.state = 'dafault' : status.state = 'raised'
                    }
                }
            }
        }
    }

    Rectangle {
        id: panel

        property color buttonColor: 'white'
        property int baseHeight: root.size == 2 ? root.landscape ? 80 : 70 : 72
        property int minimalHeight: baseHeight * 0.38
        property int defaultHeight: (root.size == 2 && !root.landscape) ? baseHeight * 1.33 : baseHeight
        property int raisedHeight: root.landscape ? panelItems.width + baseHeight : panelItems.height + baseHeight
        property int currentHeight: baseHeight

        width: root.landscape ? panel.currentHeight : root.width
        height: root.landscape ? root.height : panel.currentHeight
        anchors.bottom: root.landscape ? undefined : root.bottom
        anchors.right: root.landscape ? parent.right : undefined
        anchors.top: root.landscape ? parent.top : undefined
        anchors.left: root.landscape ? undefined : root.left
        color: root.accentColor
        state: 'default'

        states: [
            State {
                name: 'default'
                PropertyChanges { target: panel; currentHeight: panel.defaultHeight }
            },
            State {
                name: 'raised'
                PropertyChanges { target: panel; currentHeight: panel.raisedHeight }
            },
            State {
                name: 'minimal'
                PropertyChanges { target: panel; currentHeight: panel.minimalHeight }
            }
        ]

        Behavior on currentHeight {
            id: panelHeightBehavior

            PropertyAnimation { duration: 200; easing.type: Easing.OutCirc }
        }

        //Behavior on color { ColorAnimation { duration: 100 } }

        // gradient
//        Item {
//            anchors.fill: parent
//            clip: true
//            Rectangle {
//                anchors.fill: parent
//                anchors.margins: -panel.baseHeight * 2
//                rotation: -15
//                gradient: Gradient {
//                    GradientStop { position: 0.4; color: root.accentColor }
//                    GradientStop { position: 1.0; color: root.baseColor }
//                }
//            }
//        }

        MouseArea { anchors.fill: parent }

        Flow {
            id: panelFlow

            anchors.top : root.landscape ? undefined : parent.top
            anchors.left: root.landscape ? parent.left : undefined
            anchors.horizontalCenter: root.landscape ? undefined : parent.horizontalCenter
            anchors.verticalCenter: root.landscape ? parent.verticalCenter : undefined
            width: root.landscape ? childrenRect.width : undefined
            height: root.landscape ? undefined : childrenRect.height
            flow: root.landscape ? GridLayout.TopToBottom : GridLayout.LeftToRight
            spacing: (root.size == 2 && !root.landscape) ? 15 : 0

            // hide buttons in minimal
            anchors.topMargin: panel.currentHeight < panel.baseHeight ? root.landscape ? 0 : panel.minimalHeight : 0
            anchors.leftMargin: panel.currentHeight < panel.baseHeight ? root.landscape ? panel.minimalHeight : 0 : 0
            Behavior on anchors.topMargin { PropertyAnimation { duration: 500 } }
            Behavior on anchors.leftMargin { PropertyAnimation { duration: 500 } }

            Repeater {
                model: [ { icon: 'find' }, { icon: 'settings' }, { icon: 'next', script: 'view.incrementCurrentIndex()' } ]

                delegate: Item {
                    width: panel.baseHeight + (root.landscape && root.size == 1 ? text.width : 0)
                    height: panel.baseHeight + (root.landscape && root.size == 1 ? 0 : text.height)

                    Button {
                        id: button

                        width: height; height: panel.baseHeight
                        color: panel.buttonColor
                        icon: modelData.icon + '.svg'
                    }

                    Text {
                        id: text

                        // anchors broke panel here
                        x: root.landscape && root.size == 1 ? button.width : (parent.width - width) / 2
                        y: root.landscape && root.size == 1 ? (parent.height - height) / 2 : button.height

                        text: modelData.icon
                        color: panel.buttonColor
                        font { family: root.fontUi; pixelSize: panel.baseHeight / 5 }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: modelData.script == undefined ? overlaper.state = 'on' : eval(modelData.script)
                    }
                }
            }
        }

        Item {
            id: panelOpener

            width: root.landscape ? undefined : panel.baseHeight
            height: root.landscape ? panel.baseHeight : undefined
            anchors.right: panel.right
            anchors.left: root.landscape ? panel.left : undefined
            anchors.top: panel.top
            anchors.bottom: root.landscape ? undefined : panel.bottom

            // foreground colorize
            ColorOverlay {
                width: source.width; height: source.height
                color: panel.buttonColor
                rotation: root.landscape ? -90 : 0

                source: Image {
                    width: height; height: panel.baseHeight
                    source: 'dots.svg'
                    sourceSize: Qt.size(panel.baseHeight * 2, panel.baseHeight * 2)
                }
            }

            MouseArea {
                anchors.fill: parent

                property int lp: 0

                onPressed: {
                    lp = root.landscape ? mouseX : mouseY
                    panelHeightBehavior.enabled = false
                }
                onReleased: {
                    panelHeightBehavior.enabled = true

                    /*if (panel.state == 'raised' && panel.currentHeight == panel.raisedHeight)
                        ;
                    else*/ if(panel.state == 'minimal') {
                        if(panel.currentHeight < panel.defaultHeight)
                            panel.state = 'default'
                        else
                            panel.state = 'raised'
                    }
                    else if (panel.currentHeight < panel.defaultHeight)
                        panel.state = 'minimal'
                    else if(panel.state == 'raised')
                        panel.state = 'default'
                    else
                        panel.state = 'raised'
                }
                onPositionChanged: {
                    if(panelHeightBehavior.enabled) {
                        lp = root.landscape ? mouseX : mouseY
                        panelHeightBehavior.enabled = false
                    }
                    else {
                        var d = lp - (root.landscape ? mouseX : mouseY)
                        panel.currentHeight = Math.max( panel.minimalHeight, Math.min(panel.currentHeight + d, panel.raisedHeight))
                    }
                }
            }
        }

        ListView {
            id: panelItems

            property int pw: 150

            anchors.top: root.landscape ? panelOpener.bottom : panelFlow.bottom
            anchors.left: root.landscape ? panelFlow.right : panel.left
            width: root.landscape ? pw + panelFlow.width : panel.width - panel.baseHeight
            height: Math.min(contentItem.childrenRect.height + (panel.baseHeight / 3), (root.landscape ?
                    panel.height - panelOpener.height : root.height / 2))
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            model: [ { text: 'Install' },
                { text: 'Clippings' },
                { text: 'Settings' },
                { text: '<i>arrange texts</i>', script: 'view.arragngeViews()' }]

            delegate: Item {
                width: parent.width
                height: children[0].height * 1.5

                Text {
                    x: 20
                    text: modelData.text
                    color: panel.buttonColor
                    font.pixelSize: panel.baseHeight * 0.5
                    font.family: root.fontUi

                    onWidthChanged: panelItems.pw = Math.max(panelItems.pw, width)
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (modelData.script != undefined)
                            eval(modelData.script)
                        else
                            overlaper.state = 'on'
                    }
                }
            }
        }
    }

    Rectangle {
        id: fade
        anchors.fill: root
        color: 'black'
        opacity: 0.0
    }

    Rectangle {
        id: overlaper
        anchors.fill: root
        color: root.accentColor
        state: 'off'
        visible: overlaperRotation.angle > -90 && overlaperRotation.angle < 90

        states: [
            State { name: 'on' },
            State { name: 'off' }
        ]

        transitions: [
            Transition {
                from: 'on'; to: 'off'
                ParallelAnimation {
                    PropertyAnimation { target: overlaperRotation; property: 'angle'; to: root.landscape ? 90 : -90; duration: 300; easing.type: Easing.InCubic}
                    PropertyAnimation { target: fade; property: 'opacity'; from: 0.3; to: 0; duration: 600 }
                }
            },
            Transition {
                from: 'off'; to: 'on'
                SequentialAnimation {
                    ScriptAction { script: if (panel.state == 'raised') panel.state = 'default' }
                    PropertyAnimation { target: fade; property: 'opacity'; from: 0; to: 0.2; duration: 150 }
                    PropertyAnimation { target: overlaperRotation; property: 'angle'; from: root.landscape ? 90 : -90; to: 0; duration: 400; easing.type: Easing.OutCubic }
                }
            }
        ]

        transform: Rotation {
            id: overlaperRotation

            angle: 90
            axis { x: root.landscape ? 1 : 0; y: root.landscape ? 0 : 1; z: 0 }
            origin.x: root.landscape ? (root.width / 2) : 0
            origin.y: root.landscape ? 0 : (root.height / 2)
        }

        Flow {
            anchors.fill: parent
            anchors.margins: spacing
            spacing: 10


            Item { width: parent.width; height: 20 }

            Item {
                width: childrenRect.width
                height: childrenRect.height

                Text { text: 'Light'; font.pixelSize: 34; color: root.textColor }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if(root.textColor != '#000000') {
                            root.textColor = 'black'; root.baseColor = 'white'; overlaper.state = 'off'
                        }
                    }
                }
            }

            Item { width: 10; height: 10 }

            Item {
                width: childrenRect.width
                height: childrenRect.height

                Text { text: 'Dark'; font.pixelSize: 34; color: root.textColor }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (root.textColor != '#ffffff') { root.textColor = 'white'; root.baseColor = 'black'; overlaper.state = 'off'
                        }
                    }
                }
            }

            Item { width: parent.width - x; height: 10 }

            Repeater {
                model: [ '#f80', '#08f', '09e', '#07a', '#3bf', '#444', '#aaa' ]
                delegate: Rectangle {
                    width: color == root.accentColor ? 0 : 100
                    height: width
                    color: modelData


                    MouseArea {
                        anchors.fill: parent
                        onClicked: { root.accentColor = parent.color; overlaper.state = 'off' }
                    }
                }
            }

            Item {
                width: parent.width
                height: 10
            }

            Item {
                width: parent.width
                height: childrenRect.height
                visible: root.size != 2

                Text { text: 'Tablet mode'; font.pixelSize: 34; color: root.textColor }

                MouseArea { anchors.fill: parent; onClicked: { root.size = 2; overlaper.state = 'off' } }
            }

            Item {
                width: parent.width
                height: childrenRect.height
                visible: root.size != 1

                Text { text: 'Smartphone mode'; font.pixelSize: 34; color: root.textColor }

                MouseArea { anchors.fill: parent; onClicked: { root.size = 1; overlaper.state = 'off' } }
            }
        }


        Item {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Math.abs(overlaperRotation.angle * (root.landscape ? 10 : 4))
            width: childrenRect.width
            height: childrenRect.height

            Text {
                anchors.verticalCenter: parent.children[1].verticalCenter
                text: 'back'
                font.pixelSize: 34
                color: root.textColor
            }

            Button {
                width: 80; height: width
                anchors.left: parent.children[0].right
                color: root.textColor
                icon: 'next.svg'
            }

            MouseArea { anchors.fill: parent; onClicked: overlaper.state = 'off' }
        }
    }
}
