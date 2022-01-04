import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: root

    property color color
    property string icon

    // color overlay test
    ColorOverlay {
        anchors.centerIn: parent
        width: source.width; height: source.height
        color: enabled ? parent.color : '#aaa'

        source: Item {
            width: root.width * 0.67; height: root.height * 0.67
            Image {
                width: parent.width; height: parent.height
                source: 'circle.svg'
                sourceSize: Qt.size(parent.width * 2, parent.height * 2)
            }
            Image {
                width: parent.width; height: parent.height
                source: icon
                sourceSize: Qt.size(parent.width * 2, parent.height * 2)
                //sourceSize: Qt.size(1002, 1002)
            }
        }
    }

    // opacity mask test
//    Rectangle {
//        id: fill

//        anchors.fill: parent
//        color: enabled ? parent.color : '#aaa'
//        visible: false
//    }

//    OpacityMask {
//        anchors.centerIn: parent
//        width: source.width; height: source.height

//        maskSource: fill

//        source: Item {
//            width: root.width * 0.67; height: root.height * 0.67
//            Image {
//                width: parent.width; height: parent.height
//                source: 'circle.svg'
//                sourceSize: Qt.size(parent.width * 2, parent.height * 2)
//            }
//            Image {
//                width: parent.width; height: parent.height
//                source: icon
//                //sourceSize: Qt.size(parent.width * 2, parent.height * 2)
//                //sourceSize: Qt.size(1002, 1002)
//            }
//        }
//    }

    // svg test
//    Image {
//        anchors.centerIn: parent
//        width: parent.width * 0.67; height: parent.height * 0.67
//        source: 'circle.svg'
//        //sourceSize: Qt.size(parent.width * 2, parent.height * 2)
//        antialiasing: false
//        //sourceSize: Qt.size(width * 2, height * 2)
//    }

//    Image {
//        anchors.centerIn: parent
//        width: parent.width * 0.67; height: parent.height * 0.67
//        source: icon
//        //sourceSize: Qt.size(parent.width * 2, parent.height * 2)
//        antialiasing: false
//        //sourceSize: Qt.size(width * 2, height * 2)
//    }
}
