//Water.cpp
#include "WaterDB.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <QDebug.h>

using namespace std;

CWaterDB::CWaterDB()
{
	m_WaterDB = nullptr;
    m_bDatabaseOK = false;
}

CWaterDB::~CWaterDB()
{
}

int CWaterDB::InitialiseWaterDatabase()
{
	//Open the water polygon database 
	int rc = sqlite3_open( "Data/Seas.db", &m_WaterDB);
	if (rc != SQLITE_OK) 
	return -1;

	//Give permission to load extensions
	rc = sqlite3_enable_load_extension(m_WaterDB, 1);
	if (rc != SQLITE_OK)
		return -1;

	//Create a call to spatial library to load it 
	string sql = "SELECT load_extension('mod_spatialite');";
	char* messageError;
	rc = sqlite3_exec(m_WaterDB, sql.c_str(), nullptr, 0, &messageError);
	if (rc != SQLITE_OK)
	{
		cerr << messageError << endl;
		sqlite3_free(messageError);
		return -1;
	}

    //Everything loaded okay so databbase is good
    m_bDatabaseOK = true;

    return 0;
}

void  CWaterDB::FreeDatabase()
{
	sqlite3_close(m_WaterDB);
	}

bool CWaterDB::PointInWater(double x, double y)
{
	bool bInWater = false;
	sqlite3_stmt* stmt = nullptr;
//Get the SQL statement
	std::string sQuery = GetQueryFromScript( "Scripts/SelectSeaNameFromPoint.sql");
	int rc = sqlite3_prepare_v2( m_WaterDB, sQuery.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK) 
	{
		cerr << "Prepare statement failed: " << sqlite3_errmsg(m_WaterDB) << std::endl;
		sqlite3_close(m_WaterDB);
		return 1;
	}

	//Bind values to statement
	sqlite3_bind_double(stmt, 1, x );
	sqlite3_bind_double(stmt, 2, y );
	sqlite3_bind_double(stmt, 3, x);
	sqlite3_bind_double(stmt, 4, y);

	// Execute the query
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) 
	{
		bInWater = true;
	}
	else if (rc != SQLITE_DONE) 
	{
		std::cerr << "Step failed: " << sqlite3_errmsg(m_WaterDB) << std::endl;
	}

	// Finalize the statement
	sqlite3_finalize(stmt);


	return bInWater;
}


int CWaterDB::NameOfWater(double x, double y, std::string& name, int& pkID )
{
	//Prepare for the SQLite call 
	sqlite3_stmt* stmt;

	//The query 
	std::string sQuery = GetQueryFromScript("Scripts/SelectSeaNameFromPoint.sql");
	//std::string sQuery = GetQueryFromScript( "Scripts/SelectNearestSeaNameFromPoint.sql");
	int rc = sqlite3_prepare_v2(m_WaterDB, sQuery.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		cerr << "Prepare statement for IHO Seas query failed: " << sqlite3_errmsg(m_WaterDB) << std::endl;
		return -1;
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
        int nameLength = sqlite3_column_bytes(stmt, 1);
        if (nameLength > 0)
        {
            name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            pkID = sqlite3_column_int(stmt, 0);
        }
    }
	else if (rc != SQLITE_DONE)
	{
		std::cerr << "Step failed: " << sqlite3_errmsg(m_WaterDB) << std::endl;
        return -1;
	}

	// Finalize the statement
	sqlite3_finalize(stmt);

	return 0;
}

int CWaterDB::GetBorderingSeas(int pkID, double x, double y, NRList& relResults)
{
    //Prepare for the SQLite call 
    sqlite3_stmt* stmt;

    //The query 
    std::string sQuery = GetQueryFromScript("Scripts/SelectBorderingSeas.sql");
    int rc = sqlite3_prepare_v2(m_WaterDB, sQuery.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(m_WaterDB);
        cerr << "Prepare statement for getting bordering water polygons query failed: " << sqlite3_errmsg(m_WaterDB) << std::endl;
        return -1;
    }

    //Bind values to statement
    rc = sqlite3_bind_double(stmt, 1, x);
    rc = sqlite3_bind_double(stmt, 2, y);
    rc = sqlite3_bind_double(stmt, 3, x);
    rc = sqlite3_bind_double(stmt, 4, y);
    rc = sqlite3_bind_int(stmt, 5, pkID );

    qDebug() << "WaterDB: Getting borders with " << x << ", " << y << " and pk ID" << pkID;

    // Execute the query
    rc = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        nearbyResult borderResult;
        int nameLength = sqlite3_column_bytes(stmt, 0);
        if (nameLength > 0)
        {
            const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            double distance = sqlite3_column_double(stmt, 1);
            double bearing = sqlite3_column_double(stmt, 2);
            double midX = sqlite3_column_double(stmt, 3);
            double midY = sqlite3_column_double(stmt, 4);

            borderResult.name = name;
            borderResult.distance = distance;
            borderResult.bearing = bearing;
            borderResult.longitude = midX;
            borderResult.latitude = midY;

            relResults.push_back(borderResult);
        }
    }
    if (rc != SQLITE_DONE)
    {
        qDebug() << "Step failed: " << sqlite3_errmsg(m_WaterDB);
        sqlite3_finalize(stmt);
        return -1;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    return 0;
}

std::string CWaterDB::GetQueryFromScript(std::string rsScriptFilename)
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
	file.close();
	return buffer.str();

}

bool CWaterDB::IsOK()
{
    return m_bDatabaseOK;
}