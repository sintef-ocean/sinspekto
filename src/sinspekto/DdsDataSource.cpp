
#include <QXYSeries>
#include <QAreaSeries>
#include <QDebug>
#include <QRandomGenerator>
#include <QtMath>

#include "sinspekto/DdsDataSource.hpp"

QT_CHARTS_USE_NAMESPACE

Q_DECLARE_METATYPE(QAbstractSeries *)
Q_DECLARE_METATYPE(QAbstractAxis *)

DdsDataSource::DdsDataSource(QObject *parent) :
  QObject(parent),
  m_index(-1)
{
  qRegisterMetaType<QAbstractSeries*>();
  qRegisterMetaType<QAbstractAxis*>();

  generateData(0, 5, 1024);
}

void DdsDataSource::update(QAbstractSeries *series)
{
  if (series)
  {
    QXYSeries *xySeries = static_cast<QXYSeries *>(series);
    m_index++;
    if (m_index > m_data.count() - 1)
      m_index = 0;

    QVector<QPointF> points = m_data.at(m_index);
    // Use replace instead of clear + append, more performance
    xySeries->replace(points);
  }
}

void DdsDataSource::generateData(int type, int rowCount, int colCount)
{
  // Remove old data
  m_data.clear();

  // Append new data
  for (int i(0); i < rowCount; i++)
  {
    QVector<QPointF> points;
    points.reserve(colCount);
    for (int j(0); j < colCount; j++)
    {
      qreal x(0);
      qreal y(0);
      switch (type)
      {
      case 0:
        y = qSin(M_PI / 50 * j) + 0.5 + QRandomGenerator::global()->generateDouble();
        x = j;
        break;
      case 1:
        x = j;
        y = (qreal) i / 10;
        break;
      default:
        break;
      }
      points.append(QPointF(x, y));
    }
    m_data.append(points);
  }
}
