import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtQuick.Layouts 1.11

import fkin.Dds 1.0


RowLayout {
  id: root;
  //columns: 6;

  FkinStyle { id: style; }
  property font iconFont: Qt.font({
    family: "IcoFont", pointSize: style.numberFont.pointSize + 10, weight: Font.Medium });

  property alias entity: icon_.text;
  property alias description: icon_.toolTipText;
  property alias arrow: arrow_.text;
  property alias orientation: arrow_.orientation;
  property alias magnitude: magnitude_.value;
  property alias unit: unit_.text;
  property alias extra: extra_.text;

  // Value decimals are not exposed as properties for now.

  /*
  Component.onCompleted:
  {
    root.description = "Wind";
    root.orientation = 33.23;
    root.magnitude = 4.2;
    root.unit = "kn";
    root.arrow = "\uea5b";
    root.entity = "\uee98"; // wind
  }
  */
  Label {
    id: extra_;
    Layout.rightMargin: 15;
  }

  Label {
    id: icon_;
    text: "\uef50";
    font: root.iconFont;
    property string toolTipText: qsTr("Info");
    ToolTip.text: toolTipText;
    ToolTip.visible: toolTipText ? ma.containsMouse : false;
    MouseArea {
      id: ma;
      anchors.fill: parent;
      hoverEnabled: true;
    }
  }

  Label {
    id: arrow_;
    text: "\uea5e"; // arrow-up, or arrow-down "\uea5b"
    font: root.iconFont;
    property real orientation: 0;
    transformOrigin: Item.Center;
    rotation: arrow_.orientation;
  }

  Label {
    id: direction_;
    property int decimals: 0;
    text: isNaN(root.orientation) ? "-" : Number(root.orientation).toLocaleString(Qt.locale(), 'f', direction_.decimals) + "\u00b0";
    Layout.rightMargin: 15;
    Layout.preferredWidth: 70;
  }

  Label {
    text: "\ueff3"; // speed-meter
    font: root.iconFont;
  }

  Label {
    id: magnitude_;
    property real value: 0;
    property int decimals: 1;
    text: isNaN(magnitude_.value) ? "-" : Number(magnitude_.value).toLocaleString(Qt.locale(), 'f', magnitude_.decimals);
    Layout.preferredWidth: 50;
    Layout.alignment: Qt.AlignRight;
  }

  Label {
    id: unit_;
    text: qsTr("-");
    Layout.rightMargin: 15;
  }

}
