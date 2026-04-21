//initManager.cpp
#include "initManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QApplication>

//declare static member variables outside of class to allocate memory correctly 
QString CInitManager::m_dataPath = "";
QString CInitManager::m_scriptsPath = "";

CInitManager::CInitManager()
{
}

CInitManager::~CInitManager()
{
}

bool CInitManager::resolvePaths()
{
    m_dataPath = getSubfolderPath( "data" );
    m_scriptsPath = getSubfolderPath( "scripts" );

    if (m_dataPath.isEmpty() || m_scriptsPath.isEmpty()) 
    {
        return false;
    }

    return true;
}

QString CInitManager::getSubfolderPath(const QString& subfolderName)
{
    //Get location of application executable 
    QDir baseDir(QCoreApplication::applicationDirPath());

    //See if the subfolder exists off the absolute path 
    if (baseDir.exists(subfolderName))
    {
        return baseDir.absoluteFilePath(subfolderName) + "/";
    }

    //Move up two folders and try again
    if (baseDir.cdUp() && baseDir.cdUp())
    {
        if (baseDir.exists(subfolderName))
        {
            return baseDir.absoluteFilePath(subfolderName) + "/";
        }
    }

    //Not found so return empty string
    return QString();
}

bool CInitManager::copyDatabaseToAppLocal(QString& filename)
{
    //Identify and create the appdata local folder for AtMap to hold the version of the database for the current user 
    QString localPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir localDir(localPath);

    // Create local path in case it did not exist 
    QDir().mkpath(localPath);

    //Only proceed if the database is not already in appdata local 
    QString nameDb = "land.db";
    QFile qf( localPath + "/" + nameDb);
    filename = qf.fileName();
    if (qf.exists() )
        return true;

    //Get the database file 
    QFile sourceFile( DATA_PATH + "land.db" );

    //Check it exists 
    if (!sourceFile.exists() ) 
    {
        filename = "";
        return false;
    }

    //Do the copy from source to app data local 
    if (!sourceFile.copy(qf.fileName()  ))
    {
        qCritical() << "File Error: Found source database but copy to appdata/local failed." << sourceFile.errorString();
        filename = "";
        return false;
    }

    // Ensure the user has full permissions to write to their local copy
    qf.setPermissions(QFile::WriteOwner | QFile::ReadOwner | QFile::WriteUser | QFile::ReadUser);
    qDebug() << "Success: Database copied from" << sourceFile.fileName()  << "to" << filename;

    return true;
}