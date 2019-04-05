/** -*- mode: c++ ; c-basic-offset: 2 -*-
 *
 *  @file gmic_qt.cpp
 *
 *  Copyright 2017 Sebastien Fourey
 *
 *  This file is part of G'MIC-Qt, a generic plug-in for raster graphics
 *  editors, offering hundreds of filters thanks to the underlying G'MIC
 *  image processing framework.
 *
 *  gmic_qt is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gmic_qt is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gmic_qt.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "gmic_qt.h"
#include <QApplication>
#include <QDebug>
#include <QList>
#include <QLocale>
#include <QSettings>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QTranslator>
#include <cstring>
#include "Common.h"
#include "DialogSettings.h"
#include "Globals.h"
#include "HeadlessProcessor.h"
#include "Logger.h"
#include "MainWindow.h"
#include "Updater.h"
#include "Widgets/LanguageSelectionWidget.h"
#include "Widgets/ProgressInfoWindow.h"
#include "gmic.h"
#ifdef _IS_MACOS_
#include <libgen.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#endif

namespace GmicQt
{
const InputMode DefaultInputMode = Active;
const OutputMode DefaultOutputMode = InPlace;
const OutputMessageMode DefaultOutputMessageMode = Quiet;

const QString & gmicVersionString()
{
  static QString value = QString("%1.%2.%3").arg(gmic_version / 100).arg((gmic_version / 10) % 10).arg(gmic_version % 10);
  return value;
}
} // namespace GmicQt

int launchPlugin()
{
  TIMING;
  int dummy_argc = 1;
  char dummy_app_name[] = GMIC_QT_APPLICATION_NAME;

#ifdef _IS_WINDOWS_
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
#endif

  char * fullexname = nullptr;
#ifdef _IS_MACOS_
  {
    char exname[2048] = {0};
    // get the path where the executable is stored
    uint32_t size = sizeof(exname);
    if (_NSGetExecutablePath(exname, &size) == 0) {
      printf("executable path is %s\n", exname);
      fullexname = realpath(exname, nullptr);
      printf("full executable name is %s\n", fullexname);
      if (fullexname) {
        char * fullpath = dirname(fullexname);
        printf("full executable path is %s\n", fullpath);
        if (fullpath) {
          char pluginpath[2048] = {0};
          strncpy(pluginpath, fullpath, 2047);
          strncat(pluginpath, "/GMIC/plugins/:", 2047);
          char * envpath = getenv("QT_PLUGIN_PATH");
          if (envpath) {
            strncat(pluginpath, envpath, 2047);
          }
          printf("plugins path is %s\n", pluginpath);
          setenv("QT_PLUGIN_PATH", pluginpath, 1);
        }
      }
    } else {
      fprintf(stderr, "buffer too small; need size %u\n", size);
    }
    setenv("QT_DEBUG_PLUGINS", "1", 1);
  }
#endif
  if (!fullexname) {
    fullexname = dummy_app_name;
  }
  char * dummy_argv[1] = {fullexname};

  QApplication app(dummy_argc, dummy_argv);
  QApplication::setWindowIcon(QIcon(":resources/gmic_hat.png"));
  QCoreApplication::setOrganizationName(GMIC_QT_ORGANISATION_NAME);
  QCoreApplication::setOrganizationDomain(GMIC_QT_ORGANISATION_DOMAIN);
  QCoreApplication::setApplicationName(GMIC_QT_APPLICATION_NAME);
  QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
  DialogSettings::loadSettings(GmicQt::GuiApplication);

  // Translate according to current locale or configured language
  QString lang = LanguageSelectionWidget::configuredTranslator();
  if (!lang.isEmpty() && (lang != "en")) {
    auto translator = new QTranslator(&app);
    translator->load(QString(":/translations/%1.qm").arg(lang));
    QApplication::installTranslator(translator);
  }
  TIMING;
  MainWindow mainWindow;
  TIMING;
  if (QSettings().value("Config/MainWindowMaximized", false).toBool()) {
    mainWindow.showMaximized();
  } else {
    mainWindow.show();
  }
  TIMING;
  return QApplication::exec();
}

int launchPluginHeadlessUsingLastParameters()
{
  int dummy_argc = 1;
  char dummy_app_name[] = GMIC_QT_APPLICATION_NAME;
  char * dummy_argv[1] = {dummy_app_name};

#ifdef _IS_WINDOWS_
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
#endif
  QApplication app(dummy_argc, dummy_argv);
  QApplication::setWindowIcon(QIcon(":resources/gmic_hat.png"));
  QCoreApplication::setOrganizationName(GMIC_QT_ORGANISATION_NAME);
  QCoreApplication::setOrganizationDomain(GMIC_QT_ORGANISATION_DOMAIN);
  QCoreApplication::setApplicationName(GMIC_QT_APPLICATION_NAME);
  QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);

  DialogSettings::loadSettings(GmicQt::GuiApplication);
  Logger::setMode(DialogSettings::outputMessageMode());
  // Translate according to current locale or configured language
  QString lang = LanguageSelectionWidget::configuredTranslator();
  if (!lang.isEmpty() && (lang != "en")) {
    auto translator = new QTranslator(&app);
    translator->load(QString(":/translations/%1.qm").arg(lang));
    QCoreApplication::installTranslator(translator);
  }

  HeadlessProcessor processor;
  ProgressInfoWindow progressWindow(&processor);
  if (processor.command().isEmpty()) {
    return 0;
  }
  processor.startProcessing();
  return QApplication::exec();
}

int launchPluginHeadless(const char * command, GmicQt::InputMode input, GmicQt::OutputMode output)
{
  int dummy_argc = 1;
  char dummy_app_name[] = GMIC_QT_APPLICATION_NAME;
  char * dummy_argv[1] = {dummy_app_name};

#ifdef _IS_WINDOWS_
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
#endif
  QCoreApplication app(dummy_argc, dummy_argv);
  QCoreApplication::setOrganizationName(GMIC_QT_ORGANISATION_NAME);
  QCoreApplication::setOrganizationDomain(GMIC_QT_ORGANISATION_DOMAIN);
  QCoreApplication::setApplicationName(GMIC_QT_APPLICATION_NAME);
  QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);

  DialogSettings::loadSettings(GmicQt::NonGuiApplication);
  Logger::setMode(DialogSettings::outputMessageMode());

  HeadlessProcessor headlessProcessor(&app, command, input, output);
  QTimer idle;
  idle.setInterval(0);
  idle.setSingleShot(true);
  QObject::connect(&idle, SIGNAL(timeout()), &headlessProcessor, SLOT(startProcessing()));
  idle.start();
  return QCoreApplication::exec();
}
