#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <map>

#include "sinspekto/FkinDds.hpp"
#include "sinspekto/WeatherDds.hpp"
#include "sinspekto/RatatoskDds.hpp"
#include "sinspekto/QtToDds.hpp"
#include "sinspekto/QtToDdsPriv.hpp"

QtToDds::QtToDds(QObject *parent) :
  QObject(parent),
  dds(nullptr),
  m_ready(false)
{ }

QtToDds::~QtToDds() = default;

bool QtToDds::initialized() const
{
  return m_ready;
}

void QtToDds::init(int domain)
{
  dds = std::make_unique<QtToDds::Dds>(static_cast<uint32_t>(domain));
  m_ready = true;
  emit initializedChanged(m_ready);
}
