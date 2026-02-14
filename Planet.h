//Planet.h
#pragma once

#include <string>
#include "WaterDB.h"
#include "LandDB.h"
#include "LandByCoastlineDB.h"
#include "GeoResult.h"
#include <qlist.h>
#include <QGeocoordinate.h>
#include "mapViewType.h"


class CPlanet
{
//Constructor
public:
    CPlanet();
    ~CPlanet();

//Methods 
public:
    void InitialiseDatabases( const QString& applicationPath, const QString& dbFilename );
    bool DatabasesOK();
    int GetCurrentLocationName(double x, double y, std::string& name, int& objectID );
    int GetBorderingRelations(int objectID, double x, double y, NRList& relResults);
    int GetContainedPoints(int relationID, double x, double y, NRList& pointResults);
    int GetVisibleData(double south, double west, double north, double east, double stepSize, QList<CGeoResult>& geoResults, NRList& pointResults);
    int getSpecificPolygon(int polygonId, QList<CGeoResult>& polygon);
    int SearchNames(std::string searchText, NRList& nrResults);
    double distance(QGeoCoordinate& p1, QGeoCoordinate& p2);
    double Bearing(QGeoCoordinate& p1, QGeoCoordinate& p2);
    int setMapViewType(MapViewType mapViewType);

private:
    int TransformZoomBandToPopulation(int zoomBand );
    int getZoomBand(double stepSize);
    bool splitBoundingBox(double& south1, double& west1, double& north1, double& east1, double& south2, double& west2, double& north2, double& east2);

//Member variables
private:
    CWaterDB m_WaterDB;
    CLandDB m_LandDB;
    CLandByCoastlineDB m_LandByCoastlineDB;
    double m_Radius; //In km
    bool  m_onLand;
};