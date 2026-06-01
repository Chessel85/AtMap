//geoProjection.cpp
#include "GeoProjection.h"
#include <cmath>

CGeoProjection::CGeoProjection() : m_projContext(nullptr), m_projTransform(nullptr)
{
    m_projContext = proj_context_create();
}

CGeoProjection::~CGeoProjection()
{
    Clear();
    if (m_projContext) {
        proj_context_destroy(m_projContext);
    }
}

void CGeoProjection::Clear()
{
    if (m_projTransform) {
        proj_destroy(m_projTransform);
        m_projTransform = nullptr;
    }
}

int CGeoProjection::CreateProjection(const QGeoCoordinate& centre, const std::string& projType)
{
    if (!m_projContext) return -1;
    Clear();

    std::string projString = "+proj=" + projType +
        " +lat_0=" + std::to_string(centre.latitude()) +
        " +lon_0=" + std::to_string(centre.longitude()) +
        " +ellps=WGS84 +units=m";

    // Use proj_create_crs_to_crs to set up the geographic-to-projected pipeline
    PJ* p = proj_create_crs_to_crs(m_projContext, "EPSG:4326", projString.c_str(), nullptr);
    if (!p) return -1;

    // Normalize ensures it uses visualization order: Longitude (X), Latitude (Y)
    m_projTransform = proj_normalize_for_visualization(m_projContext, p);
    proj_destroy(p);

    return m_projTransform ? 0 : -1;
}

int CGeoProjection::ForwardTransform(const QGeoCoordinate& geoPoint, double& outMetersX, double& outMetersY) const
{
    if (!isReady()) return -1;

    double x = geoPoint.longitude();
    double y = geoPoint.latitude();

    // Use generic batch processing to guarantee visualization order axis mapping
    int result = proj_trans_generic(m_projTransform, PJ_FWD, &x, sizeof(double), 1, &y, sizeof(double), 1, nullptr, 0, 0, nullptr, 0, 0);
    if (result < 1 || x == HUGE_VAL || y == HUGE_VAL) return -1;

    outMetersX = x;
    outMetersY = y;
    return 0;
}

int CGeoProjection::ReverseTransform(double metersX, double metersY, QGeoCoordinate& outGeoPoint) const
{
    if (!isReady()) return -1;

    double x = metersX;
    double y = metersY;

    int result = proj_trans_generic(m_projTransform, PJ_INV, &x, sizeof(double), 1, &y, sizeof(double), 1, nullptr, 0, 0, nullptr, 0, 0);
    if (result < 1 || x == HUGE_VAL || y == HUGE_VAL) return -1;

    outGeoPoint.setLongitude(x);
    outGeoPoint.setLatitude(y);
    return 0;
}