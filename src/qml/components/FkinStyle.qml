import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtCharts 2.2

Item {

  //TODO: usage of this should probably be a singleton available everywhere it is used

  //FontLoader { id: the_icofont; source: "fonts/icofont/icofont.ttf" }
  readonly property string fontFamily: "Public Sans";
  property int chartTheme: {
    Material.theme == Material.Light ? ChartView.ChartThemeLight : ChartView.ChartThemeDark; }
  property font defaultFont: Qt.font({ family: fontFamily, pointSize: 16, weight: Font.Bold });
  property font plotFont:    Qt.font({ family: fontFamily, pointSize: 15, weight: Font.Medium });
  property font numberFont:  Qt.font({ family: fontFamily, pointSize: 21, weight: Font.Bold });

  property real largerPoints: defaultFont.pointSize + 3;
  property real smallerPoints: defaultFont.pointSize - 3;
  property color defaultLineColor: Material.color(Material.Teal, Material.Shade500);

}
