//LandByCoastlineDB.h
#pragma once

#include <sqlite3.h>
#include <string>
#include "GeoResult.h"
#include <list>


class CLandByCoastlineDB
{
//Constructor
public:
	CLandByCoastlineDB();
	~CLandByCoastlineDB();

	//Methods
public:
	int InitialiseDatabase();
	void FreeDatabase();
    bool IsOnLand(double x, double y);
    int GetVisibleLandPolygonsWKT(double south, double west, double north, double east, double x, double y, std::list<CGeoResult>& geoResults);

private:
	std::string GetQueryFromScript(std::string rsScriptFilename);
//Member variable
private:
	sqlite3* m_LandByCoastlineDB;
};

