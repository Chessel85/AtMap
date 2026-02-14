//initManager.h
#pragma once

#include <QString>

class CInitManager 
{
//Constructors
public:
    CInitManager();
    ~CInitManager();

//Methods
public:
    bool copyDatabaseToAppLocal(QString& filename );
    QString getApplicationPath() const;

//Members
private:
    QString m_applicationPath;
};
