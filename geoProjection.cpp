//geoProjection.cpp

#include "geoProjection.h"
#include <QDebug>

CGeoProjection::CGeoProjection()
    : m_projContext(nullptr)
    , m_projTransform(nullptr)
{
    CreateContext();
}

CGeoProjection::~CGeoProjection()
{
    DestroyObjects();
}

void CGeoProjection::CreateContext()
{
    m_projContext = proj_context_create();
    if (!m_projContext)
    {
        qDebug() << "CGeoProjection: Failed to create context.";
    }
}

void CGeoProjection::DestroyObjects()
{
    if (m_projTransform)
    {
        proj_destroy(m_projTransform);
        m_projTransform = nullptr;
    }

    if (m_projContext)
    {
        proj_context_destroy(m_projContext);
        m_projContext = nullptr;
    }
}

bool CGeoProjection::isReady() const
{
    return (m_projContext != nullptr && m_projTransform != nullptr);
}

int CGeoProjection::CreateProjection(const QGeoCoordinate& centre, const std::string& projType)
{
    if (!m_projContext)
    {
        qDebug() << "CGeoProjection: No context available when creating projection.";
        return -1;
    }

    // Clean up any existing transformation if re-initializing the center
    if (m_projTransform)
    {
        proj_destroy(m_projTransform);
        m_projTransform = nullptr;
    }

    // Capture the anchor coordinates
    m_pjGeoCentre = proj_coord(centre.longitude(), centre.latitude(), 0, 0);

    // Construct the PROJ pipeline string using the requested projection type
    std::string projString = "+proj=" + projType + 
                             " +lat_0=" + std::to_string(m_pjGeoCentre.xyz.y) + 
                             " +lon_0=" + std::to_string(m_pjGeoCentre.xyz.x) + 
                             " +ellps=WGS84 +units=m";

    // Create the projection coordinate operation
    PJ* p = proj_create_crs_to_crs(m_projContext, "EPSG:4326", projString.c_str(), nullptr);
    if (p == nullptr)
    {
        qDebug() << "CGeoProjection: Failed to create PROJ transformation pipeline.";
        int err = proj_context_errno(m_projContext);
        qDebug() << "PROJ error code: " << err << " (" << proj_errno_string(err) << ")";
        return -1;
    }

    // Ensure forward transforms accept degrees/visualization order
    m_projTransform = proj_normalize_for_visualization(m_projContext, p);
    proj_destroy(p); // Clean up the raw intermediate pipeline step

    if (m_projTransform == nullptr)
    {
        qDebug() << "CGeoProjection: Failed to normalize PROJ transformation.";
        return -1;
    }

    return 0;
}

int CGeoProjection::ForwardTransform(const QGeoCoordinate& geoPoint, double& outMetersX, double& outMetersY)
{
    if (!isReady())
    {
        qWarning() << "CGeoProjection: Projection pipeline not ready. Call CreateProjection first.";
        return -1;
    }

    // Load the target point coordinates (Longitude, Latitude)
    PJ_COORD pjGeoPoint;
    pjGeoPoint.xyz.x = geoPoint.longitude();
    pjGeoPoint.xyz.y = geoPoint.latitude();
    pjGeoPoint.xyz.z = 0;
    pjGeoPoint.xyz.w = 0;

    // Run the forward transform from EPSG:4326 to the localized flat plane
    PJ_COORD pjProjPoint = proj_trans(m_projTransform, PJ_FWD, pjGeoPoint);

    // Capture potential transformation projection errors
    if (pjProjPoint.xyz.x == HUGE_VAL || pjProjPoint.xyz.y == HUGE_VAL)
    {
        qWarning() << "CGeoProjection: Coordinate transformation failed.";
        return -1;
    }

    // Output the raw offset distances relative to the center in meters
    outMetersX = pjProjPoint.xyz.x;
    outMetersY = pjProjPoint.xyz.y;

    return 0;
}