//initManager.cpp
#include "initManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QApplication>

CInitManager::CInitManager()
{
}

CInitManager::~CInitManager()
{
}

bool CInitManager::copyDatabaseToAppLocal(QString& filename)
{
    QString localPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QString dbName = "land.db";
    filename = localPath + "/" + dbName;

    // Create local path in case it did not exist 
    QDir().mkpath(localPath );

    // Only copy if the database does not  exist in AppData yet
    if (QFile::exists(filename))
        return  true;

    //Get path of seed database 
    QString seedPath = DATA_PATH + dbName;

    if (seedPath.isEmpty())
    {
        qCritical() << "Initialization Failure: Could not locate seed database:" << seedPath;
        filename = "";
        return false;
    }

    //Do the copy from seed to local data 
    QFile sourceFile(seedPath);
    if (!sourceFile.copy(filename))
    {
        qCritical() << "File Error: Found seed but copy failed." << sourceFile.errorString();
        filename = "";
        return false;
    }

    // Ensure the user has full permissions to write to their local copy
    QFile::setPermissions(filename, QFile::WriteOwner | QFile::ReadOwner | QFile::WriteUser | QFile::ReadUser);

    qDebug() << "Success: Database copied from" << seedPath << "to" << filename;

    return true;
}
