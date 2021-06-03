import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtCharts 2.2

/// Minimal qml example that loads sinspekto dds elements
import fkin.Dds 1.0

ApplicationWindow {
  id: window;
  /// Application title
  title: qsTr("Minimal QML example");
  /// Window width [px]
  width: 480;
  /// Window height [px]
  height: 200;
  /// Window visibility
  visible: true;

  /// Makes an instance of QtToDds
  QtToDds {
    id: ddsParticipant;
    readonly property int domain: 0;

    Component.onCompleted: {
      /// initialize participant first, then the dds signals
      init(domain);

      /// initialize DDS adapters in an orderly fashion.
      ddsSubscriber.init(ddsParticipant, "testBit");
      ddsPublisher.init(ddsParticipant, "testBit", aSwitch.checked, true);
    }
  }

  GridLayout {
    anchors.fill: parent;
    anchors.margins: 20;
    columns: 2;

    Label { text: "DDS bit publisher"; }

    /// Switch to send DDS signal.
    Switch {
      id: aSwitch;
      text: qsTr("Send with DDS");
      checked: true;
      onToggled: {
        console.log("Switch signal is: " + ddsPublisher.signal);
      }

      /// sinspekto::DdsBitPublisher's signal is connected to the switch.
      DdsBitPublisher {
        id: ddsPublisher;
        signal: aSwitch.checked;
      }
    }
    Label { text: "DDS bit subscriber"; }

    /// Switch to receive DDS signal.
    Switch {
      id: aSwitchInput;
      text: qsTr("Received from DDS");
      checked: ddsSubscriber.signal;

      /// sinspekto::DdsBitSubscriber's signal is connected to the switch.
      DdsBitSubscriber {
        id: ddsSubscriber;
      }
    }
  }
}
