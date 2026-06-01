// GeoToScreen.cpp
#include "GeoToScreen.h"
#include <algorithm>

CGeoToScreen::CGeoToScreen()
    : m_minX(0.0), m_maxX(0.0), m_minY(0.0), m_maxY(0.0)
    , m_uniformScale(0.0), m_offsetX(0.0), m_offsetY(0.0)
{
}

int CGeoToScreen::CreateProjection(const QGeoCoordinate& centre)
{
    // Keeping your visual map's original Lambert Equal Area projection layout
    return m_spatialEngine.CreateProjection(centre, "laea");
}

void CGeoToScreen::SetScreenSize(const QSizeF& screenSize)
{
    m_screenSize = screenSize;
}

// MODE 1: Standard Atlas Setup
int CGeoToScreen::GetGeoBoundingBox(int stepVertical, int stepHorizontal, QGeoCoordinate& bbBottomLeft, QGeoCoordinate& bbTopRight)
{
    if (!m_spatialEngine.isReady()) return -1;

    // Define the map boundaries explicitly in meters relative to origin center
    m_minX = 0.0 - stepHorizontal;
    m_minY = 0.0 - stepVertical;
    m_maxX = 0.0 + stepHorizontal;
    m_maxY = 0.0 + stepVertical;

    // In Mode 1, scaling is directly proportional to screen dimensions without extra alignment padding
    m_uniformScale = 0.0;
    m_offsetX = 0.0;
    m_offsetY = 0.0;

    // Reverse project the boundary coordinates back to geocoordinates so the calling loop knows the window extent
    if (m_spatialEngine.ReverseTransform(m_minX, m_minY, bbBottomLeft) < 0) return -1;
    if (m_spatialEngine.ReverseTransform(m_maxX, m_maxY, bbTopRight) < 0) return -1;

    return 0;
}

// MODE 2: High Contrast Single-Country Maximized Setup
int CGeoToScreen::setTransformedBoundingBox(const QGeoCoordinate& geoBottomLeft, const QGeoCoordinate& geoTopRight)
{
    if (!m_spatialEngine.isReady()) return -1;

    // Find the country's layout frame limits in raw projection meters
    if (m_spatialEngine.ForwardTransform(geoBottomLeft, m_minX, m_minY) < 0) return -1;
    if (m_spatialEngine.ForwardTransform(geoTopRight, m_maxX, m_maxY) < 0) return -1;

    double projWidth = m_maxX - m_minX;
    double projHeight = m_maxY - m_minY;
    if (projWidth == 0.0 || projHeight == 0.0) return -1;

    // Calculate maximum visual scale matching factors
    double scaleX = m_screenSize.width() / projWidth;
    double scaleY = m_screenSize.height() / projHeight;
    m_uniformScale = std::min(scaleX, scaleY);

    // Apply the original 1/4 layout translation shifts to center the maximized country
    m_offsetX = (m_screenSize.width() - (projWidth * m_uniformScale)) / 4.0;
    m_offsetY = (m_screenSize.height() - (projHeight * m_uniformScale)) / 4.0;

    return 0;
}

int CGeoToScreen::transform(const QGeoPolygon& geoPolygon, QPolygonF& screenPolygon) const
{
    if (!m_spatialEngine.isReady()) return -1;

    int pointCount = geoPolygon.size();
    if (pointCount == 0) {
        screenPolygon.clear();
        return 0;
    }

    double projWidth = m_maxX - m_minX;
    double projHeight = m_maxY - m_minY;
    if (projWidth == 0.0 || projHeight == 0.0) return -1;

    // Determine viewport transformation scale values
    double scaleX = (m_uniformScale > 0.0) ? m_uniformScale : (m_screenSize.width() / projWidth);
    double scaleY = (m_uniformScale > 0.0) ? m_uniformScale : (m_screenSize.height() / projHeight);

    screenPolygon.clear();
    screenPolygon.reserve(pointCount);

    for (int i = 0; i < pointCount; ++i)
    {
        double meterX = 0.0;
        double meterY = 0.0;
        if (m_spatialEngine.ForwardTransform(geoPolygon.coordinateAt(i), meterX, meterY) < 0) continue;

        // Unified layout formula factoring in boundaries, scale modes, and flipped Y-axis orientations
        double screenX = (meterX - m_minX) * scaleX + m_offsetX;
        double screenY = (m_maxY - meterY) * scaleY + m_offsetY;

        screenPolygon.append(QPointF(screenX, screenY));
    }

    return 0;
}

int CGeoToScreen::transformPoint(const QGeoCoordinate& geoPoint, QPointF& screenPoint) const
{
    if (!m_spatialEngine.isReady()) return -1;

    double projWidth = m_maxX - m_minX;
    double projHeight = m_maxY - m_minY;
    if (projWidth == 0.0 || projHeight == 0.0) return -1;

    double scaleX = (m_uniformScale > 0.0) ? m_uniformScale : (m_screenSize.width() / projWidth);
    double scaleY = (m_uniformScale > 0.0) ? m_uniformScale : (m_screenSize.height() / projHeight);

    double meterX = 0.0;
    double meterY = 0.0;
    if (m_spatialEngine.ForwardTransform(geoPoint, meterX, meterY) < 0) return -1;

    // Uses the EXACT same mathematical projection formula as transform() above
    double screenX = (meterX - m_minX) * scaleX + m_offsetX;
    double screenY = (m_maxY - meterY) * scaleY + m_offsetY;

    screenPoint.setX(screenX);
    screenPoint.setY(screenY);
    return 0;
}