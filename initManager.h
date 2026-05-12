//initManager.h
#pragma once

#include <QString>
#include <string>

#define DATA_PATH CInitManager::dataPath()
#define SCRIPTS_PATH CInitManager::scriptsPath()
#define SOUNDS_PATH CInitManager::soundsPath()


class CInitManager 
{
//Constructors
public:
    CInitManager();
    ~CInitManager();

//Methods
public:
    bool resolvePaths();
    bool copyDatabaseToAppLocal(QString& filename );
    static QString dataPath() { return m_dataPath; }
    static QString scriptsPath() { return m_scriptsPath; }
    static std::string soundsPath() { return m_soundsPath.toStdString(); }

private:
    QString getSubfolderPath(const QString& subfolder);

//Members
private:
    static QString m_dataPath;
    static QString m_scriptsPath;
    static QString m_soundsPath;
};
