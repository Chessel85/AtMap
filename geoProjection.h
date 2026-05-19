// geoProjection.h
#pragma once

#include <QGeocoordinate.h>
#include <proj.h>
#include <string>

class CGeoProjection
{
// Constructor / Destructor
public:
    CGeoProjection();
    ~CGeoProjection();

// Methods
public:
    // Initializes the PROJ transformation centered on the user's location.
    // Can accept a projection type string like "laea" or "aeqd"
    int CreateProjection(const QGeoCoordinate& centre, const std::string& projType = "aeqd");

    // Transforms a single geographic point into raw relative meters from the center
    int ForwardTransform(const QGeoCoordinate& geoPoint, double& outMetersX, double& outMetersY);

    // Convenience method to check if the PROJ pipeline is initialized and valid
    bool isReady() const;

private:
    void CreateContext();
    void DestroyObjects();

// Member variables
private:
    PJ_CONTEXT* m_projContext;
    PJ* m_projTransform; 
    PJ_COORD m_pjGeoCentre;
};