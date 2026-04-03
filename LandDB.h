//LandDB.h
#pragma once

#include "aliases.h"
#include "LongLat.h"
#include "GeoResult.h"
#include "layerManager.h"
#include <sqlite3.h>
#include <string>
#include <qlist.h>

class CLandDB
{
//Constructor
public:
    CLandDB();
    ~CLandDB();

//Methods
public:
    int InitialiseLandDatabase(const QString& dbFilename);
    void FreeDatabase();
    bool getLayerData(CLayerManager& layerManager);
    bool  setLayers(const QMap<int, bool>& layerStates);
    int setBaseGroupLayer(int layerId);
    bool IsOnLand(double x, double y);
    NRList NameOfOSMLand(double x, double y, int adminLevel);
    NRList    FullListOfOSMLand(double x, double y);
    int GetPolygonFromPoint(double x, double y, QList<CGeoResult>& geoResults);
    SVector NameAndDetails(double x, double y);
    double Distance(CLongLat p1, CLongLat p2);
    double Bearing(CLongLat p1, CLongLat p2);
    int SearchNames(std::string rsSearchTerm, NRList& nrResults);
    std::string  GetWikipediaValue(int type, int ID);
    int GetVisibleLandPolygonsWKT(double south, double west, double north, double east, int zoomBand, QList<CGeoResult>& geoResults);
    int getSpecificPolygon(int polygonId, QList<CGeoResult>& polygon);
    int GetVisibleCities(double south, double west, double north, double east, int minPopulation, NRList& pointResults);
    int GetVisiblePoints(double south, double west, double north, double east, NRList& pointResults);
    int GetBorderingRelations(int objectID, double x, double y, NRList& relResults);
    int GetContainedPoints(int relationID, double x, double y, NRList& pointResults);


private:
    std::string GetQueryFromScript(std::string rsScriptFilename);
    void UpdateTableToQuery(std::string& rsQuery, int zoomBand);

//Member variable
private:
    sqlite3* m_LandDB;
};

