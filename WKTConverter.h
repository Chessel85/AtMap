//WKTConverter.h
#pragma once

#include <QList>
#include <QGeoCoordinate>
#include <qgeopolygon.h>
#include <QString>
#include <QStringList>
#include "GeoResult.h"
#include <QDebug> // For logging, can be removed in release builds if not needed

class CWKTConverter
{
//Methods
public:
    static bool ParseGeoResults(QList<CGeoResult>& geoResults);

private:
    static QList<QGeoCoordinate> ParseCoordinates(const QString& coordinateString);
    static QGeoPolygon ParsePolygon(const QString& polygonString);
    static QList<QGeoPolygon> ParseMultipolygon(const QString& multiPolygonString); // New method
    static bool ParseWKTToGeoPolygons(CGeoResult& geoResult );
    };