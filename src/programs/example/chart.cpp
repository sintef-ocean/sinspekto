#include <QApplication>
#include <QFontDatabase>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QUrl>
#include <QDebug>

///////// <<HERE>> ////////
#include "sinspekto/SinspektoQml.hpp"

int main(int argc, char *argv[]){

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication app(argc, argv);
  app.setOrganizationName("The Organization");
  app.setApplicationName("your-app");

  qint32 fontId = QFontDatabase::addApplicationFont(":/fonts/PublicSans-VariableFont_wght.ttf");
  QStringList fontList = QFontDatabase::applicationFontFamilies(fontId);
  QString family = fontList.at(0);
  QApplication::setFont(QFont(family));

  ///////// <<HERE>> ////////
  sinspekto::LoadSinspektoQmlTypes();
  QQmlEngine engine;

  QQmlComponent component(&engine);
  QQuickWindow::setDefaultAlphaBuffer(true);
  component.loadUrl(QUrl(QStringLiteral("qrc:/chart.qml")));

  if (component.isReady() )
    component.create();
  else
    qWarning() << component.errorString();

  return app.exec();

}
