import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtCharts 2.2

import fkin.Dds 1.0

ApplicationWindow {
  id: window;
  /// Application title
  title: qsTr("Chart Example");
  /// Window width [px]
  width: 600;
  /// Window height [px]
  height: 300;
  /// Application font
  font : defaultFont;
  /// Window visibility
  visible: true;
  /// Font family used
  readonly property string fontFamily: "Public Sans";
  /// Chart theme used
  property int chartTheme: { Material.theme == Material.Light ?
    ChartView.ChartThemeLight : ChartView.ChartThemeDark; }
  /// Default font
  property font defaultFont: Qt.font({ family: fontFamily, pixelSize: 20, weight: Font.Bold });
  /// Plot font
  property font plotFont:    Qt.font({ family: fontFamily, pixelSize: 16, weight: Font.Bold });
  /// Number font
  property font numberFont:  Qt.font({ family: fontFamily, pixelSize: 25, weight: Font.Bold });

  /// Makes an instance of QtToDds
  QtToDds {
    id: ddsParticipant;
    readonly property int domain: 0;

    Component.onCompleted: {
      /// Initialize participant first, then the dds signals
      init(domain);

      /// Initialize the IdVec1d Publisher, on topic 'anInput' with recipient 'Test'.
      ddsSlide.init(ddsParticipant, "anInput", "Test", theSlider.value, true);
      /// Initialize the Subscriber buffer on same topic, with a buffer size of 50 values.
      ddsSlideBuffer.init(ddsParticipant, "anInput", "Test", 50, false);
    }
  }

  // Dds instances
  Item {
    DdsIdVec1dPublisher { id: ddsSlide; }
    DdsIdVec1dBuffer { id: ddsSlideBuffer; }
  }

  GridLayout {

    anchors.fill: parent;
    anchors.margins: 20;
    columns: 2;

    Label { text: "Slider"; }
    RowLayout {
      /// Slider value to send over dds
      Slider {
        id: theSlider;
        from: 10;
        value: 15;
        to: 30;
        stepSize: 1;
        snapMode: Slider.SnapOnRelease;
        onValueChanged: { ddsSlide.value = theSlider.value; }
      }
      Label { text: theSlider.value; }
    }

    Label { text: qsTr("Time series"); }
    /// Chart to show line
    ChartView {
      id: depthChart;
      height: 250;
      Layout.fillWidth: true;
      title: qsTr("Title");
      theme: window.chartTheme; // must be set before other customizations
      titleColor: Material.foreground;
      backgroundColor: Material.background;
      titleFont : window.plotFont;
      legend.visible: false;
      antialiasing: true;
      animationOptions: ChartView.NoAnimation;

      property point fov_y: Qt.point(0,1);

      Connections {
        target: ddsSlideBuffer;
        onNewData:
        {
          ddsSlideBuffer.updateSeries(slideSignal, FKIN.T, FKIN.X);
        }


        function ensureTimeHorizon(timeChart, horizonMS, futureMS, t_min, t_max)
        {
          if (timeChart.axes.length < 2)
            console.log("There are no axes to adjust the time horizon");

          // assume for now Qt.Horizontal is time
          var Tidx = 0;
          for (var i = 0; i < timeChart.axes.length; ++i)
          {
            if (timeChart.axes[i].orientation == Qt.Horizontal)
              Tidx = i;
          }

          timeChart.axes[Tidx].max = new Date(timeChart.axes[Tidx].max.getTime() + futureMS);

          var diff = t_max.getTime() - t_max.getTime();
          if (diff < horizonMS)
            timeChart.axes[Tidx].min = new Date(t_max.getTime() - horizonMS + futureMS);
        }

        onRangeChanged: {
          if(dim == FKIN.X)
          {
            slideSignal.axisY.min = ddsSlideBuffer.rangeX.x;
            slideSignal.axisY.max = ddsSlideBuffer.rangeX.y;
          }
        }

        onRangeTChanged:
        {
          slideSignal.axisX.min = ddsSlideBuffer.rangeTmin;
          slideSignal.axisX.max = ddsSlideBuffer.rangeTmax;

          ensureTimeHorizon(
            depthChart,
            slideSignal.horizonMS,
            slideSignal.futureMS,
            ddsSlideBuffer.rangeTmin,
            ddsSlideBuffer.rangeTmax);
        }
      }

      /// Graph of slider signal from dds.
      LineSeries {
        id: slideSignal;
        name: qsTr("Slider Signal");

        property int futureMS: 1000;
        property int horizonMS: 10000 - futureMS;

        axisX: DateTimeAxis {
          titleText: qsTr("Time");
          format: "hh:mm:ss";
          labelsFont: window.plotFont;
          tickCount : 3;
          min: new Date();
          max: new Date();

          onRangeChanged: { }
        }
        axisY: ValueAxis {
          titleText: qsTr("Value") + " [-]";
          labelsFont: window.plotFont;
          reverse: false;
          min: 0;
          max: 0.1;
        }

        // QTBUG-58230: cannot use OpenGL because 32 bit int overflow (time is int64)
        useOpenGL: false;
        color: Material.color(Material.Blue, Material.Shade500);
        style: Qt.SolidLine;
        width: 3;
      }
    }
  }
}
