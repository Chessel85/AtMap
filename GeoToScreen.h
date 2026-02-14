//Geo2Screen.h
#pragma once

#include <QPolygonF>
#include <QGeocoordinate.h>
#include <QGeopolygon.h>
#include <QPointF>
#include <QSizeF>
#include <proj.h>

class CGeoToScreen
{
// Constructor
public:
    CGeoToScreen();
    ~CGeoToScreen();

//Methods 
public:
    // return a bounding box given user location and size of step vertically and horizontally 
    int GetGeoBoundingBox(int stepVertical, int stepHorizontal, QGeoCoordinate& bbBottomLeft, QGeoCoordinate& bbTopRight );
    void SetScreenSize(const QSizeF& screenSize);
    int CreateProjection(const QGeoCoordinate& centre);

    // Method to transform a polygon from SRID 4326 to screen coordinates
    int transform(const QGeoPolygon& geoPolygon, QPolygonF& screenPolygon );
    int transformPoint(const QGeoCoordinate& geoPoint, QPointF& screenPoint);

private:
    void CreateContext();
    void DestroyObjects();

    // Member variables
    PJ_CONTEXT* m_projContext;
    PJ* m_projTransform; 
    PJ_COORD m_pjGeoCentre;
    PJ_COORD m_pjProjBottomLeft;
    PJ_COORD m_pjProjTopRight;
    QSizeF m_screenSize;
        };
