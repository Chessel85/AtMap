//Beacon.h
#pragma once
#include "soundType.h"

class CBeacon
{
//Constructor
public:
    CBeacon();

//Methods
    public:
    void setPosition(double x, double y, double z);
    void initialise(SoundType type, SourceCategory cat );
    void getPosition(double& x, double& y, double& z) const;
    SoundType getSoundType() const;
    SourceCategory getSourceCategory() const;

//Attributes
private:
    double m_posX; 
    double m_posY; 
    double m_posZ;
    SoundType m_soundType; //such as city or train station 
    SourceCategory m_sourceCategory; //Such as spatial or stereo 
};