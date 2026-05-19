//Beacon.h
#pragma once
#include "soundType.h"
#include <QString>
#include <QMetaType>

class CBeacon
{
//Constructor
public:
    CBeacon();

//Methods
    public:
    void setPosition(double x, double y, double z);
    void initialise(const QString& type, SourceCategory cat );
    void getPosition(double& x, double& y, double& z) const;
    QString getSoundType() const;
    SourceCategory getSourceCategory() const;

//Attributes
private:
    double m_posX; 
    double m_posY; 
    double m_posZ;
    QString m_soundType; //such as city or train station 
    SourceCategory m_sourceCategory; //Such as spatial or stereo 
};
//declare beacon macro so pointers to it can be added as variant data in info pane widgets 
Q_DECLARE_METATYPE(CBeacon*)
