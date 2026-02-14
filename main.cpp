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

    //Check database
    CInitManager initManager;
    QString applicationPath, dbFilename;
    bool ok = initManager.copyDatabaseToAppLocal(dbFilename);
    if (!ok)
    {
        // Critical failure: log it and tell the user
        qCritical() << "Failed to initialize the local database.";
        QMessageBox::critical(nullptr, "Initialization Error",
            "Accessible Atlas could not set up its data folder.");
        return -1;
    }
    MainWindow w( initManager.getApplicationPath(), dbFilename );
    w.show();
    return a.exec();
}
