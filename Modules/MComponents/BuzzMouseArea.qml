import QtQuick 2.3

MouseArea {
    onClicked: if (isTouch) mngSys.startSound()
}
