//GeoToScreen.cpp

#include "GeoToScreen.h"
#include <iostream>
#include <QPointF>
#include <vector>
#include <QDebug>

CGeoToScreen::CGeoToScreen()
{
    //Create context 
    CreateContext();
}

CGeoToScreen::~CGeoToScreen()
{
    DestroyObjects();
}

void CGeoToScreen::CreateContext()
{
    m_projContext = proj_context_create();
    if (!m_projContext)
    {
        qDebug() << "CGeoToScreen: Failed to create context.";
    }
}

void CGeoToScreen::DestroyObjects()
{
    // Clean up the proj objects to prevent memory leaks
    if (m_projTransform)
    {
        proj_destroy(m_projTransform);
    }

    if (m_projContext)
    {
        proj_context_destroy(m_projContext);
    }
}

int CGeoToScreen::CreateProjection(const QGeoCoordinate& centre)
{
    //Check a context is in place
    if (!m_projContext)
    {
        qDebug() << "CGeoToScreen: No context available when creating projections.";
        return -1;
    }

    // Construct the PROJ string with the given center coordinates
    m_pjGeoCentre = proj_coord(centre.longitude(), centre.latitude(), 0, 0);
    std::string projString = "+proj=laea +lat_0=" + std::to_string(m_pjGeoCentre.xyz.y ) + " +lon_0=" + std::to_string(m_pjGeoCentre.xyz.x ) + " +ellps=WGS84 +units=m";

    //Create the projection transformation
    PJ* p = proj_create_crs_to_crs(m_projContext, "EPSG:4326", projString.c_str(), nullptr);
    if (p == nullptr)
    {
        qDebug() << "CGeoToScreen: Failed to create PROJ transformation pipeline.";
        int err = proj_context_errno(m_projContext);
        qDebug() << "PROJ error code: " << err << " (" << proj_errno_string(err) << ")";
        return -1;
    }

    // Normalize for visualization to get lon/lat order and use degrees not radians 
    m_projTransform = proj_normalize_for_visualization(m_projContext, p );
    if (m_projTransform == nullptr)
    {
        qDebug() << "CGeoToScreen: Failed to normalize PROJ transformation.";
        proj_destroy(p );
        return -1;
    }
    //Tidy up 
    proj_destroy(p);

    return 0;
}

void CGeoToScreen::SetScreenSize(const QSizeF& screenSize)
{    
    m_screenSize = screenSize;
}

int CGeoToScreen::GetGeoBoundingBox(int stepVertical, int stepHorizontal, QGeoCoordinate& bbBottomLeft, QGeoCoordinate& bbTopRight)
{
    //Check there is a transformation object
    if (!m_projTransform)
    {
        qDebug() << "CGeoToScreen: No projection object created when getting bounding box.  Call CreateProjection first.";
        return -1;
    }

    //Set the extent of the bounding rectangle in projected CRS 
    m_pjProjBottomLeft.xyz.x = 0 - stepHorizontal;
    m_pjProjBottomLeft.xyz.y = 0 - stepVertical;
    m_pjProjTopRight.xyz.x = 0 + stepHorizontal;
    m_pjProjTopRight.xyz.y = 0 + stepVertical;

    //Do a reverse transformation 
    PJ_COORD pjGeoBottomLeft, pjGeoTopRight;
    int retval = proj_trans_bounds(m_projContext, m_projTransform, PJ_INV,
        m_pjProjBottomLeft.xyz.x, m_pjProjBottomLeft.xyz.y,
        m_pjProjTopRight.xyz.x, m_pjProjTopRight.xyz.y,
        &pjGeoBottomLeft.xyz.x, &pjGeoBottomLeft.xyz.y,
        &pjGeoTopRight.xyz.x, &pjGeoTopRight.xyz.y,
        21 //Recommended value 
    );
    
    //Put into QGeoCoordinate format 
    bbBottomLeft = QGeoCoordinate(pjGeoBottomLeft.xyz.y, pjGeoBottomLeft.xyz.x);
    bbTopRight= QGeoCoordinate(pjGeoTopRight.xyz.y, pjGeoTopRight.xyz.x);

    return 0;
}

/*int CGeoToScreen::GetGeoBoundingBox(int stepVertical, int stepHorizontal, QGeoCoordinate& bbBottomLeft, QGeoCoordinate& bbTopRight)
{
    //Check there is a transformation object
    if (!m_projTransform)
    {
        qDebug() << "CGeoToScreen: No projection object created when getting bounding box.  Call CreateProjection first.";
        return -1;
    }

    //Set the extent of the bounding rectangle in projected CRS 
    m_pjProjBottomLeft.xyz.x = 0 - stepHorizontal;
        m_pjProjBottomLeft.xyz.y  = 0 - stepVertical;
    m_pjProjTopRight.xyz.x = 0 + stepHorizontal;
    m_pjProjTopRight.xyz.y = 0 + stepVertical;

    //Do a reverse transformation 
    PJ_COORD pjGeoBottomLeft = proj_trans(m_projTransform, PJ_INV, m_pjProjBottomLeft);
    PJ_COORD pjGeoTopRight = proj_trans(m_projTransform, PJ_INV, m_pjProjTopRight);

    //Put into QGeoCoordinate format 
    //bbBottomLeft = QGeoCoordinate(pjGeoBottomLeft.xyz.y, pjGeoBottomLeft.xyz.x);
    //bbTopRight= QGeoCoordinate(pjGeoTopRight.xyz.y, pjGeoTopRight.xyz.x);
        // in case the bounding box isn't rectangular in the geographic CRS
    PJ_COORD bl, br, tl, tr;
    bl.xyz.x = m_pjProjBottomLeft.xyz.x; bl.xyz.y = m_pjProjBottomLeft.xyz.y;
    br.xyz.x = m_pjProjTopRight.xyz.x; br.xyz.y = m_pjProjBottomLeft.xyz.y;
    tl.xyz.x = m_pjProjBottomLeft.xyz.x; tl.xyz.y = m_pjProjTopRight.xyz.y;
    tr.xyz.x = m_pjProjTopRight.xyz.x; tr.xyz.y = m_pjProjTopRight.xyz.y;

    PJ_COORD bl_geo = proj_trans(m_projTransform, PJ_INV, bl);
    PJ_COORD br_geo = proj_trans(m_projTransform, PJ_INV, br);
    PJ_COORD tl_geo = proj_trans(m_projTransform, PJ_INV, tl);
    PJ_COORD tr_geo = proj_trans(m_projTransform, PJ_INV, tr);

    // Find the min/max lon/lat from the four corners
    double min_lon = std::min({ bl_geo.lp.lam, br_geo.lp.lam, tl_geo.lp.lam, tr_geo.lp.lam });
    double max_lon = std::max({ bl_geo.lp.lam, br_geo.lp.lam, tl_geo.lp.lam, tr_geo.lp.lam });
    double min_lat = std::min({ bl_geo.lp.phi, br_geo.lp.phi, tl_geo.lp.phi, tr_geo.lp.phi });
    double max_lat = std::max({ bl_geo.lp.phi, br_geo.lp.phi, tl_geo.lp.phi, tr_geo.lp.phi });

    // Put into QGeoCoordinate format (requires degrees)
    bbBottomLeft = QGeoCoordinate(min_lat, min_lon  );
    bbTopRight = QGeoCoordinate(max_lat, max_lon);

    return 0;
}*/

int CGeoToScreen::transform(const QGeoPolygon& geoPolygon, QPolygonF& screenPolygon)
{
    // Ensure the transformation object has been created
    if (!m_projTransform) 
    {
        qWarning() << "CGeoToScreen: No projection created. Call CreateProjection first.";
        screenPolygon = QPolygonF();
        return -1;
    }

    // The number of points in the input polygon
    int pointCount = geoPolygon.size();
    if (pointCount == 0) 
    {
        screenPolygon.clear();
        return 0; // Nothing to do
    }

    // Create vectors to hold coordinates which deals with memory management 
    std::vector<double> lons(pointCount);
    std::vector<double> lats(pointCount);

    // Copy the coordinates into the arrays 
    for (int i = 0; i < pointCount; ++i) 
    {
        lons[i] = geoPolygon.coordinateAt(i).longitude();
        lats[i] = geoPolygon.coordinateAt(i).latitude();
    }

    // Perform the core transformation using the PROJ library
    int result = proj_trans_generic(
        m_projTransform, PJ_FWD,
        lons.data(), sizeof(double), pointCount,
        lats.data(), sizeof(double), pointCount,
        nullptr, 0, 0, nullptr, 0, 0
    );

    if (result < 0)
    {
        qCritical() << "CGeoToScreen:  Failed to transform coordinates:" << proj_errno_string(proj_context_errno(PJ_DEFAULT_CTX));
        screenPolygon.clear();
        return -1;
    }

    // Pre-calculate constants to avoid redundant calculations in the loop
    const double projWidth = m_pjProjTopRight.xyz.x - m_pjProjBottomLeft.xyz.x;
    const double projHeight = m_pjProjTopRight.xyz.y - m_pjProjBottomLeft.xyz.y;

    if (projWidth == 0.0 || projHeight == 0.0)
    {
        qWarning() << "CGeoToScreen: Projected bounding box has zero width or height.";
        screenPolygon.clear();
        return -1;
    }

    // Now convert the projected coordinates (in meters) to screen coordinates (in pixels)
    for (int i = 0; i < pointCount; ++i)
    {
        double screenX = ((lons[i] - m_pjProjBottomLeft.xyz.x) / projWidth) * m_screenSize.width();

        // Map the y-coordinate from projected space to screen pixels, and flip the Y-axis
        double screenY = ((m_pjProjTopRight.xyz.y - lats[i]) / projHeight) * m_screenSize.height();

        screenPolygon.append(QPointF(screenX, screenY));
    }

    return 0;
}

int CGeoToScreen::transformPoint(const QGeoCoordinate& geoPoint, QPointF& screenPoint)
{
    // Ensure the transformation object has been created
    if (!m_projTransform)
    {
        qWarning() << "CGeoToScreen: No projection created in transformPoint. Call CreateProjection first.";
        return -1;
    }

    //Transform the geoPoint to projection
    PJ_COORD pjGeoPoint;
    pjGeoPoint.xyz.x = geoPoint.longitude();
    pjGeoPoint.xyz.y = geoPoint.latitude();
    PJ_COORD pjProjPoint = proj_trans(m_projTransform, PJ_FWD, pjGeoPoint);

    // Calculate width and height as easier to debug 
    const double projWidth = m_pjProjTopRight.xyz.x - m_pjProjBottomLeft.xyz.x;
    const double projHeight = m_pjProjTopRight.xyz.y - m_pjProjBottomLeft.xyz.y;

    if (projWidth == 0.0 || projHeight == 0.0)
    {
        qWarning() << "CGeoToScreen: Projected bounding box has zero width or height.";
        return -1;
    }

    // Now convert the projected coordinate (in meters) to screen coordinates (in pixels)
    double screenX = ((pjProjPoint.xyz.x - m_pjProjBottomLeft.xyz.x) / projWidth) * m_screenSize.width();
    double screenY = ((m_pjProjTopRight.xyz.y - pjProjPoint.xyz.y ) / projHeight) * m_screenSize.height();

    screenPoint.setX(screenX);
    screenPoint.setY(screenY);

    return 0;
}