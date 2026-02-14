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

void CUser::MoveByStep(int dx, int dy)
{
    //Work out the azimuth 
    double azimuth = 0.0; //North is 0, east is 90 etc 
    if (dx > 0)
        azimuth = 90.0;
    else if (dy < 0)
        azimuth = 180.0;
    else if (dx < 0)
        azimuth = 270.0;

    qDebug() << "Move location : from " << m_location;

    //Do the calculation 
    m_location = m_location.atDistanceAndAzimuth(m_StepSize*1000, azimuth);

    qDebug() << "Move location : to " << m_location;
    //Cannot go too high or too low 
    if (m_location.latitude() > 80.0)
        m_location.setLatitude(80.0);
    else if (m_location.latitude() < -80.0)
        m_location.setLatitude(-80.0);

    //And wrap longitude to stay within minus and plus 180 
    if (m_location.longitude() < -180.0)
        m_location.setLongitude(m_location.longitude() + 360.0);
    else if (m_location.longitude() > 180.0)
        m_location.setLongitude(m_location.longitude() - 360.0);
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