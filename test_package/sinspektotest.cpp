#include <QApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickWindow> // find and link Quick
#include <QUrl>
#include <QDebug>
#include <QTimer>

///////// <<HERE>> ////////
#include "sinspekto/SinspektoQml.hpp"

int main(int argc, char *argv[]){

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication app(argc, argv);
  app.setOrganizationName("The Organization");
  app.setApplicationName("your-app");

  ///////// <<HERE>> ////////
  sinspekto::LoadSinspektoQmlTypes();
  QQmlEngine engine;

  QQmlComponent component(&engine);
  QQuickWindow::setDefaultAlphaBuffer(true);

  QTimer::singleShot(200, QApplication::instance(), &QApplication::quit);

  return app.exec();

}
