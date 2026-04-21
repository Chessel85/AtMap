//Main.cpp
#include "messageHandler.h"
#include "mainwindow.h"
#include <QApplication>
#include <QMessagebox>
#include "initManager.h"


int main(int argc, char *argv[])
{
    // Install the handler before creating the QApplication
    qInstallMessageHandler(messageHandler);

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
        return -1;
    }
    MainWindow w(dbFilename);
    w.show();
    return a.exec();
}
