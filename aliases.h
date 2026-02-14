//aliases.h
#pragma once

#include <string>
#include <list>
#include <vector>
#include "LongLat.h"

struct nearbyResult
{
    std::string name;
    std::string featureClass;
    double latitude;
    double longitude;
    double area;
    double distance;
    double bearing;
    int elementType;
    int elementID;
    int displayOrder;
};


using SList = std::list<std::string>;
using SVector = std::vector<std::string>;
using LLVector = std::vector<CLongLat>;
using  NRVector = std::vector<nearbyResult>;
using  NRList = std::list<nearbyResult>;

struct geoLayer
{
    std::string layerName;
    NRList geoResults;
};
using  GLList = std::list<geoLayer>;

