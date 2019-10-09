#include "stdafx.h"
#include "main_window.h"
#include <QApplication>
//#include "logger.h"
#include "utility.h"
#include "crash_handler.h"
#include <windows.h>
#include <ShellScalingAPI.h>

int main(int argc, char *argv[])
{
    SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    QApplication app(argc, argv);
    //app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    //CrashHandler::HookUnhandledExceptions(PACKAGE_NAME);

//    try
//    {
//        //hp::fortis::Logger::install("HP/WorkTools/Show/log", "Show");
//    }
//    catch (const spdlog::spdlog_ex& ex)
//    {
//        qWarning() << "Cannot install custom logging handler, reason" << ex.what();
//    }

    //app.setApplicationVersion(PACKAGE_VERSION);
    app.setOrganizationName("HP");
    app.setApplicationName("WorkTools/3DRegistration");

    Utility::initialization();

    MainWindow window;
    window.showMaximized();

    return app.exec();
}
