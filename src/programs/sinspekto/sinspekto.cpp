#include <QApplication>
#include <QCommandLineParser>
#include <QFontDatabase>
#include <QTranslator>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QUrl>
#include <QDebug>

#include <cstdlib>
#include <iostream>
#include <vector>

#include "sinspekto/sinspekto.hpp"
#include "sinspekto/SinspektoQml.hpp"

void PrintBanner()
{
  std::vector<std::string> banner;
  banner.push_back("   _____ _                      _    _        ");
  banner.push_back("  / ____(_)                    | |  | |       ");
  banner.push_back(" | (___  _ _ __  ___ _ __   ___| | _| |_ ___  ");
  banner.push_back("  \\___ \\| | '_ \\/ __| '_ \\ / _ \\ |/ / __/ _ \\ ");
  banner.push_back("  ____) | | | | \\__ \\ |_) |  __/   <| || (_) |");
  banner.push_back(" |_____/|_|_| |_|___/ .__/ \\___|_|\\_\\\\__\\___/ ");
  banner.push_back("                    | |                       ");
  banner.push_back("                    |_|                       ");

  for (auto &line : banner)
    std::cout << line << std::endl;
}

int main(int argc, char *argv[])
{

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication app(argc, argv);
  app.setOrganizationName("SINTEF Ocean");
  app.setApplicationName("Sinspekto");

  qint32 fontId = QFontDatabase::addApplicationFont(":/fonts/PublicSans-VariableFont_wght.ttf");
  fontId = QFontDatabase::addApplicationFont(":/fonts/Oswald-VariableFont_wght.ttf");
  QStringList fontList = QFontDatabase::applicationFontFamilies(fontId);
  QString family = fontList.at(0);
  std::cout << "Using font " << family.toStdString() << std::endl;
  QApplication::setFont(QFont(family));


  QTranslator translator;
  QCommandLineParser parser;
  parser.setApplicationDescription("Sintef information inspector is a graphical user interface for decision support");
  parser.addHelpOption();

  QCommandLineOption selectLanguage(QStringList() << "l" << "language",
                                    QApplication::tr("Select displayed language <language>"),
                                    QApplication::tr("language"));

  selectLanguage.setDefaultValue("no");
  parser.addOption(selectLanguage);
  parser.process(app);
  QString lang = parser.value(selectLanguage);

  PrintBanner();

  bool translationLoaded = false;

  if (lang.compare("no", Qt::CaseInsensitive) == 0
      || lang.compare("norwegian", Qt::CaseInsensitive) == 0
      || lang.compare("norsk", Qt::CaseInsensitive) == 0)
    {
      std::cout << "Loading Norwegian" << std::endl;
      translationLoaded = translator.load(":/fkin_no");
    }
  else if (lang.compare("en", Qt::CaseInsensitive) == 0
           || lang.compare("english", Qt::CaseInsensitive) == 0)
    {
      std::cout << "Loading English" << std::endl;
      translationLoaded = translator.load(":/fkin_en");
    }

  if(!translationLoaded)
    std::cerr << "No translation applied" << std::endl;
  else
    app.installTranslator(&translator);


  sinspekto::LoadSinspektoQmlTypes();
  QQmlEngine engine;

  QQmlComponent component(&engine);
  QQuickWindow::setDefaultAlphaBuffer(true);
  component.loadUrl(QUrl(QStringLiteral("qrc:/sinspekto.qml")));

  if (component.isReady() )
    component.create();
  else
    qWarning() << component.errorString();

  return app.exec();

}
