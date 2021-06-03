import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtQuick.Layouts 1.11

GridLayout
{
  id: fkin_controls;
  x: 30;
  y: 30;
  columns: 2;
  width: parent.width > 500 ? 500 : parent.width;
  columnSpacing: 5;
  rowSpacing: 5;

  Label { text: qsTr("DDS-connectables"); font.bold: true; }
  Label { text: qsTr("Element look"); font.bold: true; }

  Label { text: "Button"; }
  Button { text: qsTr("Click me"); onClicked: console.log("Button pressed"); }

  Label { text: "CheckBox"; }
  CheckBox
  {
    text: qsTr("To Be Checked");
    onToggled: function() {
      if (checkState == Qt.Checked)
        console.log("CheckBox checked");
      if (checkState == Qt.Unchecked) console.log("CheckBox unchecked");
      if (checkState == Qt.PartiallyChecked) console.log("CheckBox partially checked");
    }
  }

  Label { text: "Switch"; }
  Switch { text: qsTr("Optimize"); onToggled: console.log("Switch toggled: " + checked); }

  Label { text: "RadioButton"; }
  RowLayout
  {
    RadioButton
    {
      text: "Opium";
      checked: true;
      onToggled: function() { if (checked) console.log(qsTr("You chose: ") + text); }
    }
    RadioButton {
      text: qsTr("Beer");
      onToggled: function() { if (checked) console.log(qsTr("You chose: ") + text); }
    }
  }

  Label { text: "ComboBox"; }
  ComboBox
  {
    currentIndex: 1;
    model: [ qsTr("Soda"), qsTr("Beer"), qsTr("Wine") ]
    onActivated: console.log(qsTr("You chose: ") + currentText);
  }

  Label { text: "SpinBox"; }
  SpinBox
  {
    id: spinbox;
    from: 0;
    value: 314;
    to: 314 * 100;
    stepSize: 314;

    validator: DoubleValidator {
      bottom: Math.min(spinbox.from, spinbox.to);
      top: Math.max(spinbox.from, spinbox.to);
    }

    textFromValue: function(value, locale) { return Number(value / 100).toLocaleString(locale, 'f', 2); }
    valueFromText: function(text, locale) { return Number.fromLocaleString(locale, text) * 100; }

    onValueChanged: console.log(qsTr("Spinbox is: ") + value / 100);

  }

  Label { text: "Slider"; }
  RowLayout
  {
    Slider
    {
      id: theSlider;
      from: 10;
      value: 15;
      to: 30;
      stepSize: 1.2;
      snapMode: Slider.SnapOnRelease;
      onValueChanged: function ()
      {
        console.log(qsTr("Slider at: ") + value);
      }
    }
    Label { text: theSlider.value; }

  }

  Label { text: "RangeSlider"; }
  RowLayout
  {
    Label { text: theRange.first.value; }
    RangeSlider {
      id: theRange;
      from: 10;
      to: 30;
      stepSize: 2;
      snapMode: Slider.SnapAlways;

      first.value: 15;
      second.value: 20;

      first.onValueChanged: console.log(qsTr("Lower slider at: ") + first.value);
      second.onValueChanged: console.log(qsTr("Upper slider at: ") + second.value);
    }
    Label { text: theRange.second.value; }
  }

  Label { text: "Pane+Label"; }
  Pane
  {
    //
    background: Rectangle {
      color: 'transparent';
      border.color: Material.color(Material.Teal, Material.Shade700);
      radius: 2;
    }

    Label
    {
      Material.foreground: Material.color(Material.Teal, Material.Shade500);
      text: Number(theSlider.value).toLocaleString(Qt.locale(),'f',2);
      font.family: "Public Sans";
      font.pixelSize: 30;
      font.bold: true;
     }
  }

  Label { text: "BusyIndicator"; }
  BusyIndicator
  {
    running: theSlider.pressed || theRange.first.pressed || theRange.second.pressed;
  }


}
