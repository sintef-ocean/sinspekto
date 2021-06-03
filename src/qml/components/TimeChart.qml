import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtQuick.Layouts 1.11
import QtCharts 2.2

import fkin.Dds 1.0
import "fkinHelpers.js" as Fkin

ChartView {
  id: timeChart;
  height: 300;
  Layout.fillWidth: true;
  theme: style.chartTheme;
  backgroundColor: Material.background;
  titleColor: Material.foreground;
  titleFont: style.plotFont;
  antialiasing: true;
  animationOptions: ChartView.NoAnimation;

  Component.onCompleted: themeChanged();

  legend.visible: false;
  property int futureMS: 1000;
  property int widthMS: 30000;
  property int horizonMS: widthMS - futureMS;
  property point fovY: Qt.point(0, 0.1); // minimal range y-axis
  property alias axisT: timeAxis;
  property alias axisY: yAxis;
  property alias labelY: yAxis.titleText;

  property alias style: fkinStyle;
  FkinStyle { id: fkinStyle; }

  signal themeChanged;

  onThemeChanged:
  {
    timeChart.titleFont = style.plotFont;
    timeChart.axisT.setStyle();
    timeChart.axisY.setStyle();
    timeChart.legend.font = style.plotFont;
  }


  DateTimeAxis {
    id: timeAxis;
    titleText: qsTr("Time");
    format: "hh:mm:ss";
    labelsFont: style.plotFont;
    tickCount : 5;
    min: new Date();
    max: new Date();

    function setStyle(){
      timeAxis.labelsFont = style.plotFont;
      timeAxis.titleFont = style.plotFont;
    }

  }

  ValueAxis {
    id: yAxis;
    titleText: "Y";
    labelsFont: style.plotFont;
    min: 0;
    max: 1;

    function setStyle(){
      yAxis.labelsFont = style.plotFont;
      yAxis.titleFont = style.plotFont;
    }

  }

  function updateRangeT(minT, maxT){

    timeChart.axisT.min = minT;
    timeChart.axisT.max = maxT;
    Fkin.ensureTimeHorizon(timeChart, timeChart.horizonMS, timeChart.futureMS, minT, maxT);

  }


}
