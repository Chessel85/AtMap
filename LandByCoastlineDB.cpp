//LandByCoastlineDB.cpp
#include "LandByCoastlineDB.h"
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <stdexcept>

using namespace std;

CLandByCoastlineDB::CLandByCoastlineDB()
{
	m_LandByCoastlineDB = nullptr;
}

CLandByCoastlineDB::~CLandByCoastlineDB()
{
}

int CLandByCoastlineDB::InitialiseDatabase()
{
	//Open the OSMLand polygon database 
	//int rc = sqlite3_open("Data/LandByCoastline-Tiled.db", &m_LandByCoastlineDB);
    int rc = sqlite3_open("Data/land.db", &m_LandByCoastlineDB);
	if (rc != SQLITE_OK) 
	return -1;

	//Give permission to load extensions
	rc = sqlite3_enable_load_extension(m_LandByCoastlineDB, 1);
	if (rc != SQLITE_OK)
		return -1;

	//Create a call to spatial library to load it 
	string sql = "SELECT load_extension('mod_spatialite');";
	char* messageError;
	rc = sqlite3_exec(m_LandByCoastlineDB, sql.c_str(), nullptr, 0, &messageError);
	if (rc != SQLITE_OK)
	{
		cerr << messageError << endl;
		sqlite3_free(messageError);
		return -1;
	}

return 0;
}

void  CLandByCoastlineDB::FreeDatabase()
{
	sqlite3_close(m_LandByCoastlineDB);
	}


bool CLandByCoastlineDB::IsOnLand(double x, double y)
{
	//Default value for function return 
	bool bIsOnLand = false;

	//Prepare for the SQLite call 
	sqlite3_stmt* stmt;

	//The query 
	std::string sQuery = GetQueryFromScript( "Scripts/SelectIfOnLandByCoastline.sql" );
	int rc = sqlite3_prepare_v2(m_LandByCoastlineDB, sQuery.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
        std::string sErr = sqlite3_errmsg(m_LandByCoastlineDB);
		cout << "Prepare statement for testing if on land or not query failed: " << sErr << std::endl;
		return false;
	}

	//Bind values to statement
	sqlite3_bind_double(stmt, 1, x);
	sqlite3_bind_double(stmt, 2, y);
	sqlite3_bind_double(stmt, 3, x);
	sqlite3_bind_double(stmt, 4, y);

	// Execute the query
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW)
	{
		int inLand  = sqlite3_column_int( stmt, 0);
		inLand == 1 ? bIsOnLand = true : bIsOnLand = false;
	}
	else if (rc != SQLITE_DONE)
	{
		std::cerr << "Step failed: " << sqlite3_errmsg(m_LandByCoastlineDB) << std::endl;
	}

	// Finalize the statement
	sqlite3_finalize(stmt);


	return bIsOnLand;
}

int CLandByCoastlineDB::GetVisibleLandPolygonsWKT(double south, double west, double north, double east, double x, double y, std::list<CGeoResult>& geoResults)
{
    int retval = 0;

    //qDebug() << "LandDB get visible polygons: " << west << south << east << north <<  x << y;
    //Prepare for the SQLite call 
    sqlite3_stmt* stmt;

    //The query 
    std::string sQuery = GetQueryFromScript("Scripts/SelectLandByCoastlinePolygonsInBoundingBox.sql");
    //std::string sQuery = GetQueryFromScript("Data/5cli.sql");
    int rc = sqlite3_prepare_v2(m_LandByCoastlineDB, sQuery.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(m_LandByCoastlineDB);
        cerr << "Prepare statement for retrieving active land polygon failed: " << sqlite3_errmsg(m_LandByCoastlineDB) << std::endl;
        return -1;
    }

    //Bind values to statement
    rc = sqlite3_bind_double(stmt, 1, west);
    rc = sqlite3_bind_double(stmt, 2, south);
    rc = sqlite3_bind_double(stmt, 3, east);
    rc = sqlite3_bind_double(stmt, 4, north);
    rc = sqlite3_bind_double(stmt, 5, west);
    rc = sqlite3_bind_double(stmt, 6, south);
    rc = sqlite3_bind_double(stmt, 7, east);
    rc = sqlite3_bind_double(stmt, 8, north);

    // Execute the query
    rc = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int wktlength = sqlite3_column_bytes(stmt, 0);
        if (wktlength > 0)
        {
            CGeoResult geoResult;
            //geoResult.m_wk = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            geoResult.m_name = "Coastline data";

            ////qDebug() << "Interpreted " << name << "of size" << length << "total length" << totalLength;

            //Add retrieved data to result set 
            geoResults.push_back(geoResult);
        }
    }
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Step failed: " << sqlite3_errmsg(m_LandByCoastlineDB) << std::endl;
        retval = -1;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    return retval;
}

std::string CLandByCoastlineDB::GetQueryFromScript(std::string rsScriptFilename)
{
	std::ifstream file(rsScriptFilename);
	if (!file.is_open())
	{
		// Handle error if file cannot be opened
		std::cerr << "Error opening file: " << rsScriptFilename << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();

}