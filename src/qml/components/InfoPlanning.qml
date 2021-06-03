import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtQuick.Layouts 1.11

import fkin.Dds 1.0

RowLayout {
  id: root;

  FkinStyle { id: style; }
  property font iconFont: Qt.font({
    family: "IcoFont", pointSize: style.numberFont.pointSize + 10, weight: Font.Medium });

  property alias entity: icon_.text;
  property alias description: icon_.toolTipText;
  property color statusOK: Material.color(Material.Green);
  property color statusFAIL: Material.color(Material.Red);
  property color statusSLOW: Material.color(Material.Orange);
  property string statusRunning: qsTr("RUNNING");
  property string statusNotRunning: qsTr("STOPPED");
  property bool running: false;
  property int expected: 10000; // milliseconds
  property bool down: false;
  property int count0: 0;
  signal reseted;

  onReseted:
  {
    chrono_.reset();
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

  Pane {
    id: status_;
    Layout.rightMargin: 40;
    background: Rectangle {
      color: 'transparent';
      border.color: root.running ? root.statusOK : root.statusFAIL;
      radius: 2;
    }

    Label {
      horizontalAlignment: Text.AlignHCenter;
      width: parent.width;
      text: root.running ? root.statusRunning : root.statusNotRunning;
      color: root.running ? root.statusOK : root.statusFAIL;
    }
  }

  Label {
    text: "\uedcd";
    font: root.iconFont;
  }


  Pane {
    id: counter_;
    property int count;
    background: Rectangle {
      color: 'transparent';
      border.color: root.running ?
        (counter_.count <= root.expected ? root.statusOK : root.statusSLOW) : root.statusFAIL;
      radius: 2;
    }

    Component.onCompleted: count = root.count0;

    Label {
      id: chrono_;
      text: "00:00";
      Layout.rightMargin: 15;
      color: root.running ?
        (counter_.count <= root.expected ? root.statusOK : root.statusSLOW) : root.statusFAIL;

      function reset(){
        counter_.count = root.count0;
        chrono_.text = new Date(counter_.count).toLocaleTimeString(Qt.locale(), "mm:ss.z");
      }

      Timer {
        id: timer_;
        interval: 500;
        running: root.running;
        repeat: true;
        onTriggered:
        {
          counter_.count = counter_.count + (1 - 2*root.down)*timer_.interval;
          if(counter_.count < 0){
            counter_.count = 0;
          }
          chrono_.text =
            new Date(counter_.count).toLocaleTimeString(Qt.locale(), "mm:ss.z");
        }
      }
    }
  }

}
