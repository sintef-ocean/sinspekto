#pragma once
/**
   @file AxisEqualizer.hpp
   @brief QML element to equalize axes.
*/

#include <map>
#include <utility>
#include <QObject>
#include <QPointF>
#include <QRectF>

/**
   @brief Helper QML element to ensure that axes of a plot have equal scaling.

   AxisEqualizer has properties that inform on the bounding box of the data series in a
   plot area. It is useful in connection with QtChart plots, to automatically resize axes
   ranges to enforce equal scaling of axes.

*/
class AxisEqualizer : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QPointF bboxX READ bboxX NOTIFY boundingBoxChanged) ///< Value range of the data points along the x-axis.
  Q_PROPERTY(QPointF bboxY READ bboxY NOTIFY boundingBoxChanged) ///< Value range of the data points along the y-axis.
  Q_PROPERTY(QPointF equalAxisX READ equalAxisX NOTIFY equalAxisChanged) ///< Value range for an equal axis scaling, x-axis.
  Q_PROPERTY(QPointF equalAxisY READ equalAxisY NOTIFY equalAxisChanged) ///< Value range for an equal axis scaling, y-axis.
  Q_PROPERTY(QRectF plotArea READ plotArea WRITE setPlotArea NOTIFY plotAreaChanged) ///< Plot area is a QRectF specifying a drawable region on screen.
//  Q_PROPERTY(double marginPercent MEMBER m_marginPercent NOTIFY marginPercentChanged)

public:
  /**
     @brief Constructor.

     Connects signals boundingBoxChanged() and plotAreaChanged() to slot calculateEqualAxes().

     @param[in] parent QObject pointer.
  */
  explicit AxisEqualizer(QObject *parent = nullptr);
  /**
     @brief Destructor.
  */
  virtual ~AxisEqualizer();

  /// Property accessor for data bounding box x-axis range
  QPointF bboxX() const;
  /// Property accessor for data bounding box y-axis range.
  QPointF bboxY() const;
  /// Property accessor for equal x-axis range.
  QPointF equalAxisX() const;
  /// Property accessor for equal y-axis range.
  QPointF equalAxisY() const;
  /// Property accessor for plot area.
  QRectF plotArea() const;

signals:
  /**
     @brief Signal triggered when the bounding box has changed

     The bounds are ranges for a given axis as a <a
     href=https://doc.qt.io/qt-5/qpointf.html>QPointF</a>, which x() being minimum, and
     y() maximum.

     @param[out] bboxX Range of x-axis.
     @param[out] bboxY Range of y-axis.
  */
  void boundingBoxChanged(QPointF bboxX, QPointF bboxY);
  /**
     @brief Signal triggered when on of the equal axes ranges have changed

     The bounds are ranges for a given axis as a <a
     href=https://doc.qt.io/qt-5/qpointf.html>QPointF</a>, which x() being minimum, and
     y() maximum.

     @param[out] axisX Range of x-axis.
     @param[out] axisY Range of y-axis.
  */
  void equalAxisChanged(QPointF axisX, QPointF axisY);
  /**
     @brief Signal triggered when the plot area has changed.

     The plot area is given as a <a href=https://doc.qt.io/qt-5/qrectf.html>QRectF</a>. It
     is typically connected to a chart's plotArea, such as <a
     href=https://doc.qt.io/qt-5/qml-qtcharts-chartview.html#plotArea-prop>ChartView::plotArea</a>.

     @param[out] plotArea Region within which chart is drawn.
  */
  void plotAreaChanged(QRectF plotArea);
  //void marginPercentChanged(double margin);

public slots:
  /**
     @brief Register x and y ranges for a data series.

     This function registers a data series' x-range and y-range. These ranges signifies
     the bounding box of a data series. All registered ranges are used when updating the
     global bounding box. This function is typically called when the range of a data
     series changes, and the plotting region potentially needs to be updated.

     @param[in] seriesName Name of the data series to register or update.
     @param[in] xR x-axis value range.
     @param[in] yR y-axis value range.
  */
  void registerBox(const QString& seriesName, const QPointF& xR, const QPointF& yR);
  /**
     @brief Calculates axes scaling to get an equal scaling of axis for the plot area.

     Given the bounding box and plot area, this function calculates axes ranges to ensure
     that the scaling of each axis is the same.

  */
  void calculateEqualAxes();
  /**
     @brief Evaluate bounding box ranges bboxX bboxY.

     Iterates over all data series that are registered wit registerBox() and finds the
     bounding box that contains all the data series. Conditionally emits
     bondingBoxChanged().

  */
  void evaluateBoundingBox();
  /**
     @brief Sets the plot area member variable.
     @param[in] plotArea Given as a <a href=https://doc.qt.io/qt-5/qrectf.html>QRectF</a>.
  */
  void setPlotArea(QRectF plotArea);

protected:
  /// Data structure to hold value ranges for two axes of a named data series. Set with registerBox().
  std::map<QString, std::pair<QPointF, QPointF>> m_ranges;
  std::pair<QPointF, QPointF> m_bbox; ///< Global bounding box ranges.
  std::pair<QPointF, QPointF> m_equalAxes; ///< Equal axes ranges.
  QRectF m_plotArea; ///< Plot area.
  //double m_marginPercent;
};
