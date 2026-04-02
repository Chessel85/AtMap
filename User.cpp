//User.cpp
#include "User.h"
#define _USE_MATH_DEFINES
#include <cmath>

CUser::CUser()
{
    m_location = QGeoCoordinate(52.0, -1.5);  //latitude then longitude 
    m_stepIndex = 17;
    m_StepSize = constStepSizes[ m_stepIndex]/1000.0;
}

CUser::~CUser()
{
}

double CUser::GetLocationX()
{
    //return m_x;
    return m_location.longitude();
}

double CUser::GetLocationY()
{
    //return m_y;
    return m_location.latitude();
}

QGeoCoordinate CUser::GetCoordinate()
{
    return m_location;
}

void CUser::SetLocation(double x, double y)
{
    if (y <= 80.0 && y >= -80.0 ) 
        m_location = QGeoCoordinate(y, x);
}

#include <QtMath>

void CUser::MoveByStep(int dx, int dy)
{
    // Calculate Latitude Change 
    // One degree of latitude is roughly 111,132 meters everywhere
    double deltaLat = (dy * m_StepSize * 1000) / 111132.0;
    double newLat = m_location.latitude() + deltaLat;

    // 2. Calculate Longitude Change (dx)
    double latRadians = qDegreesToRadians(m_location.latitude());
    double metersPerDegreeLon = 111320.0 * qCos(latRadians);
    double deltaLon = (dx * m_StepSize * 1000 ) / metersPerDegreeLon;
    double newLon = m_location.longitude() + deltaLon;

    // Apply Clamping and Wrapping
    if (newLat > 80.0)  newLat = 80.0;
    if (newLat < -80.0) newLat = -80.0;

    while (newLon <= -180.0) newLon += 360.0;
    while (newLon > 180.0)   newLon -= 360.0;

    qDebug() << "Move location : from " << m_location;

    m_location.setLatitude(newLat);
    m_location.setLongitude(newLon);

    qDebug() << "Move location : to " << m_location;
}

double CUser::GetStepSize()
{
    return m_StepSize;
}

QGeoCoordinate CUser::CalculateSWOffset(double x, double y)
{
    double distance = sqrt(x * x + y * y);
    double azimuth = 180.0 + (180.0 / M_PI) * asin(x / distance);
    QGeoCoordinate offset;
    offset = m_location.atDistanceAndAzimuth(1000.0*m_StepSize* distance, azimuth );

    return offset;
}

QGeoCoordinate CUser::CalculateNEOffset(double x, double y)
{
    double distance = sqrt(x * x + y * y);
    double azimuth = (180.0/M_PI)*asin(x / distance);
    QGeoCoordinate offset;
    offset = m_location.atDistanceAndAzimuth(1000.0*m_StepSize*distance, azimuth);

    return offset;
}


void CUser::ChangeStep(int direction)
{
    int oldIndex = m_stepIndex;
    //Valid step sizes held in an array with step index holding current position in that array 
    if (direction > 0 && m_stepIndex < constStepSizes.size() - 1 )
    {
        m_stepIndex++;
        m_StepSize = (double)constStepSizes[m_stepIndex] / 1000.0;
    }
    else if (direction < 0 && m_stepIndex > 0)
    {
        m_stepIndex--;
        m_StepSize = (double)constStepSizes[m_stepIndex] / 1000.0;
    }
    qDebug() << "Old index " << oldIndex << "New index " << m_stepIndex;
}

bool CUser::stepAtMax()
{
    return m_stepIndex == constStepSizes.size() - 1;
}

bool CUser::stepAtMin()
{
    return m_stepIndex == 0;
}