import QtQuick 2.3
import QtQuick.Controls 1.3

TableView {
    onClicked: if (isTouch) mngSys.startSound()
}
