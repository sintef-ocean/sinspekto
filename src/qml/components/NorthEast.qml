import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtQuick.Layouts 1.11
import QtCharts 2.2

import fkin.Dds 1.0

ChartView {
  id: map;
  Layout.minimumWidth: 400;
  Layout.minimumHeight: 400;
  Layout.fillWidth: true;
  Layout.fillHeight: true;
  theme: style.chartTheme;
  backgroundColor: Material.background;
  titleColor: Material.foreground;
  titleFont : style.plotFont;
  antialiasing: true;
  animationOptions: ChartView.NoAnimation;
  //title: qsTr("Map plot");
  Component.onCompleted: themeChanged();

  signal themeChanged;

  onThemeChanged:
  {
    map.titleFont = style.plotFont;
    map.axisX.labelsFont = style.plotFont;
    map.axisX.titleFont = style.plotFont;
    map.axisY.labelsFont = style.plotFont;
    map.axisY.titleFont = style.plotFont;
    map.legend.font = style.plotFont;
  }

  property alias style: fkinStyle;
  FkinStyle { id: fkinStyle; }

  legend.visible: false;
  property point fovX: Qt.point(-50, 50);
  property point fovY: Qt.point(-50, 50);
  property alias equalizer: mapEqualizer;
  property alias axisX: mapX;
  property alias axisY: mapY;

  // Updating axis may cause a subtle bind loop that changes plotArea.
  // The changed is not revealed in the plotArea on its previous value, but two or three ago.
  // This timer is a temporary solution to avoid event loop congestion in those cases.
  Timer {
    id: axisAdjuster;
    interval: 100;
    running: false;
    repeat: false;
    onTriggered:
    {
      // When an axis update causes change of plot area
      mapX.min = mapEqualizer.equalAxisX.x;
      mapX.max = mapEqualizer.equalAxisX.y;
      mapY.min = mapEqualizer.equalAxisY.x;
      mapY.max = mapEqualizer.equalAxisY.y;
    }
  }

  AxisEqualizer {
    id: mapEqualizer;
    plotArea: map.plotArea;
    Component.onCompleted: mapEqualizer.registerBox("FOV", map.fovX, map.fovY);
    onEqualAxisChanged: axisAdjuster.start();
  }

  ValueAxis {
    id: mapY;
    titleText: qsTr("North")+" [m]";
    labelsFont: map.style.plotFont;
  }

  ValueAxis {
    id: mapX;
    titleText: qsTr("East")+" [m]";
    labelsFont: map.style.plotFont;
    tickCount: 5;
  }

}
