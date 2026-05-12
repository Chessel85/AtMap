//Beacon.cpp
#include "Beacon.h"
#include <stdio.h>

//Max distance at which the beacon can be heard
const double MAX_DISTANCE_HEARD = 150.0;

//Size of audio source
const double AUDIO_SOURCE_SIZE = 2.0;

CBeacon::CBeacon()
{
    m_posX = 0.0;
    m_posY = 0.0;
    m_posZ = 0.0;
    m_soundType = SoundType::standard;
}

void CBeacon::setPosition(double  x, double  y, double z)
{
    m_posX = x;
    m_posY = y;
    m_posZ = z;
}

void CBeacon::getPosition(double& x, double& y, double& z) const
{
    x = m_posX;
    y = m_posY;
    z = m_posZ;
}

void CBeacon::initialise(SoundType type, SourceCategory cat )
{
    m_soundType = type;
    m_sourceCategory = cat;
}

SoundType CBeacon::getSoundType() const
{
    return m_soundType;
}

SourceCategory CBeacon::getSourceCategory() const 
{
    return m_sourceCategory;
}