//geoProjection.h
#pragma once

#include <QGeocoordinate.h>
#include <proj.h>
#include <string>

class CGeoProjection
{
public:
    CGeoProjection();
    ~CGeoProjection();

    // Initializes the projection pipeline centered on the user's location
    int CreateProjection(const QGeoCoordinate& centre, const std::string& projType = "aeqd");

    // Core transformation methods (forward and reverse)
    int ForwardTransform(const QGeoCoordinate& geoPoint, double& outMetersX, double& outMetersY) const;
    int ReverseTransform(double metersX, double metersY, QGeoCoordinate& outGeoPoint) const;

    bool isReady() const { return m_projTransform != nullptr; }

private:
    void Clear();

    PJ_CONTEXT* m_projContext;
    PJ* m_projTransform;
};