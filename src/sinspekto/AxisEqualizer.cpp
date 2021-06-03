#include "sinspekto/AxisEqualizer.hpp"
#include <iostream>

AxisEqualizer::AxisEqualizer(QObject *parent) :
  QObject(parent),
  m_bbox(std::make_pair(QPointF(), QPointF())),
  m_equalAxes(std::make_pair(QPointF(), QPointF())),
  m_plotArea(QRectF())
  //m_marginPercent(0.)
{
  QObject::connect(
      this, &AxisEqualizer::boundingBoxChanged,
      this, &AxisEqualizer::calculateEqualAxes);

  QObject::connect(
      this, &AxisEqualizer::plotAreaChanged,
      this, &AxisEqualizer::calculateEqualAxes);
}

AxisEqualizer::~AxisEqualizer() = default;

QPointF AxisEqualizer::bboxX() const { return m_bbox.first; }
QPointF AxisEqualizer::bboxY() const { return m_bbox.second; }
QPointF AxisEqualizer::equalAxisX() const { return m_equalAxes.first; }
QPointF AxisEqualizer::equalAxisY() const { return m_equalAxes.second; }
QRectF AxisEqualizer::plotArea() const { return m_plotArea; }


void AxisEqualizer::registerBox(
    const QString& seriesName,
    const QPointF& xR,
    const QPointF& yR)
{
  m_ranges[seriesName] = std::make_pair(xR, yR);
  evaluateBoundingBox();
}

void AxisEqualizer::calculateEqualAxes()
{
  auto oldEqual = m_equalAxes;

  if(m_bbox.first.isNull() || m_bbox.second.isNull()) return;
  if(m_plotArea.isNull()) return;

  double
   x1 = m_bbox.first.x(),  x2 = m_bbox.first.y(),
   y1 = m_bbox.second.x(), y2 = m_bbox.second.y();

  double diff_x = x2 - x1;
  double diff_y = y2 - y1;
  double ratioScreen = m_plotArea.width()/m_plotArea.height();
  double ratio = ratioScreen/(diff_x/diff_y);

  // Objective: ratio = 1.
  if (ratio > 1 + 1e-3) // increase x-range to reduce y-stretchiness
  {
    // (diff_x + w)/diff_y = ratioScreen, solve wrt w. Extend max and min
    double w = ratioScreen*diff_y - diff_x;
    x2 += 0.5*w;
    x1 -= 0.5*w;
  }
  else if (ratio < 1 - 1e-3) // increase y-range to reduce x-stretchiness
  {
    // (diff_x)/(diff_y + w) = ratioScreen, solve wrt w. Extend max and min
    double w = diff_x/ratioScreen - diff_y;
    y2 += 0.5*w;
    y1 -= 0.5*w;
  }

  auto newEqual = std::make_pair(QPointF(x1,x2), QPointF(y1,y2));

  if(
      oldEqual.first.isNull() || oldEqual.second.isNull() ||
      oldEqual.first != newEqual.first ||
      oldEqual.second != newEqual.second
     )
  {
    m_equalAxes = newEqual;
    emit equalAxisChanged(m_equalAxes.first, m_equalAxes.second);
  }
}

void AxisEqualizer::evaluateBoundingBox()
{
  auto oldBox = m_bbox;
  m_bbox = std::make_pair(QPointF(), QPointF());

  auto expandRange =
   [](QPointF& target, QPointF& candidate)
   {
     if(target.isNull())
       target = candidate;
     else
     {
       if(target.x() > candidate.x())
         target.setX(candidate.x());
       if(target.y() < candidate.y())
         target.setY(candidate.y());
     }
   };

  for (auto& range : m_ranges)
  {
    expandRange(m_bbox.first, range.second.first);
    expandRange(m_bbox.second, range.second.second);
  }

  if(
      oldBox.first.isNull() || oldBox.second.isNull() ||
      oldBox.first != m_bbox.first ||
      oldBox.second != m_bbox.second
     )
    emit boundingBoxChanged(m_bbox.first, m_bbox.second);
}

void AxisEqualizer::setPlotArea(QRectF plotArea)
{
  // Be a bit lazy updating this to avoid flickering
  if(qAbs(m_plotArea.width() - plotArea.width()) > 5 ||
   qAbs(m_plotArea.height() - plotArea.height()) > 5)
  {
    m_plotArea = plotArea;
    emit plotAreaChanged(plotArea);
  }
}
