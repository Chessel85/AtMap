//WaterDB.h
#pragma once

#include <sqlite3.h>
#include "aliases.h"
#include <string>

class CWaterDB
{
//Constructor
public:
	CWaterDB();
	~CWaterDB();

	//Methods
public:
	int InitialiseWaterDatabase();
	void FreeDatabase();
	bool PointInWater(double x, double y );
	int NameOfWater(double x, double y, std::string& name, int& pkID );
    int GetBorderingSeas(int pkID, double x, double y, NRList& relResults);
    bool IsOK();


private:
	std::string GetQueryFromScript(std::string rsScriptFilename);

//Member variable
private:
	sqlite3* m_WaterDB;
    bool m_bDatabaseOK;
};