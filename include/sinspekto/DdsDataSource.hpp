#pragma once

#include <QObject>
#include <QAbstractSeries>

QT_CHARTS_USE_NAMESPACE

/**
   @brief Test class to generate artificial data

   This class is used as an artificial data source in conjunction with QtChart data
   structures. This is probably a reminiscent from code examples.

   It can generate a set of data and replace the contents of a QAbstractSeries data
   structure, which is used by QtChart.

*/
class DdsDataSource : public QObject
{
  Q_OBJECT
public:
  /**
     @brief Constructor.

     @param [in] parent QObject pointer.
  */
  explicit DdsDataSource(QObject *parent = nullptr);

public slots:
  /**
     @brief Generates random data.

     Creates some phony data.

     @param[in] type of data, 0 is sine wave, 1 is linearly increasing.
     @param[in] rowCount Number of vectors to create in the list.
     @param[in] colCount Number of data points in a vector.


  */
  void generateData(int type, int rowCount, int colCount);
  /**
     @brief Updates series with next row of data vector.

     For each call it takes the next row of data and increments the row index. The index
     wraps around to 0.

     @param[in,out] series Pointer to series to update.
  */
  void update(QAbstractSeries *series);

private:
  QList<QVector<QPointF> > m_data; ///< Container for generated data. List of vectors
  int m_index; ///< Index in list.


};
