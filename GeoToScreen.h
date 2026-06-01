// GeoToScreen.h
#pragma once

#include "GeoProjection.h"
#include <QPolygonF>
#include <QGeocoordinate.h>
#include <QGeopolygon.h>
#include <QPointF>
#include <QSizeF>

class CGeoToScreen
{
public:
    CGeoToScreen();
    ~CGeoToScreen() = default;

    int CreateProjection(const QGeoCoordinate& centre);
    void SetScreenSize(const QSizeF& screenSize);

    // Mode 1: Establishing the Atlas view boundaries
    int GetGeoBoundingBox(int stepVertical, int stepHorizontal, QGeoCoordinate& bbBottomLeft, QGeoCoordinate& bbTopRight);

    // Mode 2: Establishing the tight Single-Country Maximized boundaries
    int setTransformedBoundingBox(const QGeoCoordinate& geoBottomLeft, const QGeoCoordinate& geoTopRight);

    // Coordinate translation functions used by both modes
    int transform(const QGeoPolygon& geoPolygon, QPolygonF& screenPolygon) const;
    int transformPoint(const QGeoCoordinate& geoPoint, QPointF& screenPoint) const;

private:
    CGeoProjection m_spatialEngine;
    QSizeF m_screenSize;

    // Viewport layout states (stored in raw meters relative to projection center)
    double m_minX;
    double m_maxX;
    double m_minY;
    double m_maxY;

    // Viewport rendering modifiers
    double m_uniformScale;
    double m_offsetX;
    double m_offsetY;
};