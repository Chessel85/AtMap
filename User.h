//User.h
#pragma once

#include <qobject.h>
#include <qgeocoordinate.h>
#include "StepSizes.h"


class CUser : public QObject 
{
    Q_OBJECT

//Constructor
public:
	CUser();
	~CUser();

//Methods
public:
    double GetLocationX();
    double GetLocationY();
    QGeoCoordinate GetCoordinate();
    void MoveByStep(int dx, int dy);
    void SetLocation(double x, double y);
    QGeoCoordinate CalculateSWOffset(double x, double y);
    QGeoCoordinate CalculateNEOffset(double x, double y);
    double GetStepSize();
    void ChangeStep(int direction);
    bool stepAtMax();
    bool stepAtMin();

//Member variables
private:
    QGeoCoordinate m_location;
    int m_stepIndex;
    double m_StepSize;
};