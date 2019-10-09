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
    app.setApplicationName("3DRegistration");

    Utility::initialization();

    MainWindow window;
    window.showMaximized();

    return app.exec();
}
