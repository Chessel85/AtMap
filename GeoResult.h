//GeoResult.h
#pragma once

#include <string>
#include <QList>
#include <QGeoPolygon>

//Represents a database polygon making up part of a relation which may have been chunked up due to the viewing window splitting it 
class CGeoResult
{
//Constructor
//public:
    //CGeoResult();

public:
    std::string m_wkt;
    std::string m_name;
    QList<QGeoPolygon> m_geoPolygons; // This will be populated by the converter
    int m_colourIndex;
    double m_minX;  //Whole relation bounding box 
    double m_minY;
    double m_maxX;
    double m_maxY;
    double m_labelX; //Coordinates of label
    double m_labelY;
};