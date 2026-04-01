//aliases.h
#pragma once

#include <string>
#include <list>
#include <vector>
#include "LongLat.h"

struct nearbyResult
{
    std::string name = "";
    std::string featureClass = "";
    double latitude = 0;
    double longitude = 0;
    double area = 0;
    double distance = 0;
    double bearing = 0;
    int elementType = 0;
    int elementID = 0;
    int displayOrder = 0;
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

