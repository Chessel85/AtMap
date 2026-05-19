//Main.cpp
#include "messageHandler.h"
#include "mainwindow.h"
#include <QApplication>
#include <QMessagebox>
#include "initManager.h"

#define _CRTDBG_MAP_ALLOC



int main(int argc, char* argv[])
{
    //Cause memory leak reporting to happen on exit
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //Ignore external dll memory allocations 
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & ~_CRTDBG_DELAY_FREE_MEM_DF);

    //Set a break point when a memory allocation happens at 2327 occurances
    //_CrtSetBreakAlloc(332228);

    // Install the handler before creating the QApplication
    qInstallMessageHandler(messageHandler);

    int result = 0;
    {
        //Create application object 
        QApplication a(argc, argv);

    //Set up database 
    CInitManager initManager;
    bool resourcePathsOk = initManager.resolvePaths();
    if (!resourcePathsOk)
    {
        qCritical() << "FATAL: Resource directories not found.";
        QMessageBox::critical(nullptr, "Initialization Error",
            "AtMap could not resolve resource paths.");
        return -1;
    }

    QString dbFilename;
    bool ok = initManager.copyDatabaseToAppLocal(dbFilename);
    if (!ok)
    {
        // Critical failure: log it and tell the user
        qCritical() << "FATAL: Unable to move database to appdata/local/AtMap.";
        QMessageBox::critical(nullptr, "Initialization Error",
            "AtMap could not set up its data folder in appdata/local.");
    }

        MainWindow w(dbFilename);
        w.show();
        result = a.exec();
    }

    return result;
}