//initManager.cpp
#include "initManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QApplication>

CInitManager::CInitManager()
{
    m_applicationPath = QCoreApplication::applicationDirPath();
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
        return true;

    //Work out path of seed database 
    QString seedPath;

    // Candidate 1: Deployment Layout Program Files/data
    QString deployPath = m_applicationPath+ "/data/" + dbName;

    // Candidate 2: cmakelist layout 
    QString devPath = QDir(m_applicationPath).filePath("../../data/" + dbName);

    // Determine which path actually exists
    if (QFile::exists(deployPath)) 
    {
        seedPath = deployPath;
    }
    else if (QFile::exists(devPath)) 
    {
        seedPath = devPath;
    }

    if (seedPath.isEmpty())
    {
        qCritical() << "Initialization Failure: Could not locate seed database.";
        qCritical() << "Attempted paths:" << deployPath << "AND" << devPath;
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

QString CInitManager::getApplicationPath() const
{
    return m_applicationPath;
}