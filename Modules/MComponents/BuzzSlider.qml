import QtQuick 2.3
import QtQuick.Controls 1.3

Slider {
    onPressedChanged: if (pressed && isTouch) mngSys.startSound()
}
