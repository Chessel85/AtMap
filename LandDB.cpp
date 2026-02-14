//LandDB.cpp
#include "LandDB.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <stdexcept>
#include <qstring.h>
#include <QElapsedTimer>
#include <qstring.h> 
#include <qdebug.h>

using namespace std;

CLandDB::CLandDB()
{
	m_LandDB = nullptr;
}

CLandDB::~CLandDB()
{
}
int CLandDB::InitialiseOSMLandDatabase( const QString& dbFilename )
{
	//Open the OSMLand polygon database 
    QByteArray ba = dbFilename.toUtf8();
	int rc = sqlite3_open(ba.constData(), &m_LandDB);
	if (rc != SQLITE_OK) 
	return -1;

	//Give permission to load extensions
	rc = sqlite3_enable_load_extension(m_LandDB, 1);
	if (rc != SQLITE_OK)
		return -1;

	//Create a call to spatial library to load it 
	string sql = "SELECT load_extension('mod_spatialite');";
	char* messageError;
	// rc = sqlite3_exec(m_LandDB, sql.c_str(), nullptr, 0, &messageError);
    //sql = "SELECT load_extension('spatialite', 'spatialite_init');";
    //sql = "SELECT load_extension('mod_spatialite', 'spatialite_init_ex');";
    rc = sqlite3_exec(m_LandDB, sql.c_str(), nullptr, 0, &messageError);	
    if (rc != SQLITE_OK)
	{
		cerr << messageError << endl;
		sqlite3_free(messageError);
		return -1;
	}

return 0;
}

void  CLandDB::FreeDatabase()
{
	sqlite3_close(m_LandDB);
	}

void CLandDB::setApplicationPath(const QString& applicationPath)
{
    m_applicationPath = applicationPath.toStdString();
}

NRList CLandDB::NameOfOSMLand(double x, double y, int adminLevel )
{
	NRList slLand;


	//Prepare for the SQLite call 
	sqlite3_stmt* stmt;

	//The query 
	std::string sQuery = GetQueryFromScript("Scripts/SelectLandNameFromPointAndAdminLevel.sql");
int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		std::string error = sqlite3_errmsg(m_LandDB);
		cerr << "Prepare statement for IHO Seas query failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
		sqlite3_close(m_LandDB);
		return slLand;
	}

	//Bind values to statement
	rc = sqlite3_bind_double(stmt, 1, x);
	rc = sqlite3_bind_double(stmt, 2, y);
	rc = sqlite3_bind_double(stmt, 3, x);
	rc = sqlite3_bind_double(stmt, 4, y);
	rc = sqlite3_bind_int(stmt, 5, adminLevel * 2-1);
	rc = sqlite3_bind_int(stmt, 6, adminLevel * 2 );

	// Execute the query
	rc = 0;
	while(( rc = sqlite3_step(stmt) ) == SQLITE_ROW)
	{
		std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		double area = sqlite3_column_double(stmt, 1);
        int relationID = sqlite3_column_int(stmt, 2);
		nearbyResult nameResult;
		nameResult.name = name;
		nameResult.area = area;
		nameResult.elementID = relationID;
		slLand.push_back(nameResult );
	}
	if (rc != SQLITE_DONE)
	{
		std::cerr << "Step failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
	}

	// Finalize the statement
	sqlite3_finalize(stmt);

	return slLand;
}

NRList    CLandDB::FullListOfOSMLand(double x, double y)
{
    NRList    slLand;


    //Prepare for the SQLite call 
    sqlite3_stmt* stmt;

    //The query 
    std::string sQuery = GetQueryFromScript("Scripts/SelectPolygonNameFromPoint.sql");
    int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
    if (!stmt)
    {
        std::string error = sqlite3_errmsg(m_LandDB);
        QString qe = QString(error.c_str());
        qWarning() << "Prepare statement for retrieving active land polygon failed: " << qe;
        return slLand;
    }

    //Bind values to statement
    rc = sqlite3_bind_double(stmt, 1, x);
    rc = sqlite3_bind_double(stmt, 2, y);

    // Execute the query
    rc = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int polygonId = sqlite3_column_int(stmt, 0);
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double area = sqlite3_column_double(stmt, 2);
        int displayOrder = sqlite3_column_int(stmt, 3);
        nearbyResult landResult;
        landResult.elementType - 3;
        landResult.elementID = polygonId;
        landResult.name = name;
        landResult.area = area;
        landResult.displayOrder = displayOrder;

        slLand.push_back(landResult);
    }
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Step failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    return slLand;
}


SVector CLandDB::NameAndDetails(double x, double y)
{
	//Default value for function return 
	std::string sOSMLandName = "Land"; // "OSMLand";
	SVector slLand;


	//Prepare for the SQLite call 
	sqlite3_stmt* stmt;

	//The query 
	std::string sQuery = GetQueryFromScript("Scripts/SelectNearestPolygons.sql");
	int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		std::string error = sqlite3_errmsg(m_LandDB);
		cerr << "Prepare statement for detailed land query failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
		slLand.push_back(sOSMLandName);
		return slLand;
	}

	//Bind values to statement
	rc = sqlite3_bind_double(stmt, 1, x);
	rc = sqlite3_bind_double(stmt, 2, y);
	rc = sqlite3_bind_double(stmt, 3, x);
	rc = sqlite3_bind_double(stmt, 4, y);
	rc = sqlite3_bind_double(stmt, 5, x);
	rc = sqlite3_bind_double(stmt, 6, y);
	rc = sqlite3_bind_double(stmt, 7, x);
	rc = sqlite3_bind_double(stmt, 8, y);

	// Execute the query
	rc = 0;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		double area = sqlite3_column_double(stmt, 1);
		double distance  = sqlite3_column_double(stmt, 2);
		double bearing  = sqlite3_column_double(stmt, 3);

		// Create a string stream for formatting the output
		std::stringstream output;

		// Format the name, area, distance, and bearing according to your requirements
		output << std::fixed << std::setprecision(1);
		output << name << " is " << distance << "km away at bearing " << bearing << " degrees";

		// Get the formatted string
		std::string formattedString = output.str();


		slLand.push_back(formattedString);
	}
	if (rc != SQLITE_DONE)
	{
		std::cerr << "Step failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
	}

	// Finalize the statement
	sqlite3_finalize(stmt);

	//Ensure something gets returned 
	if (slLand.size() == 0)
		slLand.push_back(sOSMLandName);

	return slLand;
}

int CLandDB::GetBorderingRelations(int relationID, double x, double y, NRList& relResults)
{
	//Prepare for the SQLite call 
	sqlite3_stmt* stmt;

	//The query 
	std::string sQuery = GetQueryFromScript("Scripts/SelectBorderingPolygons.sql");
	int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
	if (!stmt )
	{
		std::string error = sqlite3_errmsg(m_LandDB);
        QString errMsg = QString(error.c_str() );
		qDebug() << "Prepare statement for getting bordering polygons query failed:" << errMsg;
        return -1;
	}

	//Bind values to statement
	rc = sqlite3_bind_double(stmt, 1, x);
	rc = sqlite3_bind_double(stmt, 2, y);
    rc = sqlite3_bind_int(stmt, 3, relationID );

    qDebug() << "LandDB: Getting borders with " << x << ", " << y << " and relation ID" << relationID;

	// Execute the query
	rc = 0;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
        nearbyResult borderResult;
        borderResult.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        borderResult.area = sqlite3_column_double(stmt, 1);
        borderResult.longitude = sqlite3_column_double(stmt, 2);
        borderResult.latitude = sqlite3_column_double(stmt, 3);
        borderResult.distance = sqlite3_column_double(stmt, 4);
        borderResult.bearing = sqlite3_column_double(stmt, 5);
        borderResult.elementType = 3;
        borderResult.elementID= sqlite3_column_int64(stmt, 6);
        borderResult.displayOrder = 0;

		relResults.push_back(borderResult );
	}
	if (rc != SQLITE_DONE)
	{
		qDebug() << "Step failed: " << sqlite3_errmsg(m_LandDB);
	}

	// Finalize the statement
	sqlite3_finalize(stmt);

	return 0;
}

int CLandDB::GetContainedPoints(int polygonId, double x, double y, NRList& pointResults)
{
    //Prepare for the SQLite call 
    sqlite3_stmt* stmt;

    //The query 
    std::string sQuery = GetQueryFromScript("Scripts/SelectContainedPointsFromPolygon.sql");
    int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
    if (!stmt )
    {
        std::string error = sqlite3_errmsg(m_LandDB);
        QString errMsg = error.c_str();
        qDebug() << "Prepare statement for getting points in polygons query failed:" << errMsg;
        return -1;
    }

    //Bind values to statement
    rc = sqlite3_bind_double(stmt, 1, x);
    rc = sqlite3_bind_double(stmt, 2, y);
    rc = sqlite3_bind_int(stmt, 3, polygonId );

    qDebug() << "LandDB: Getting contained points with " << x << ", " << y << " and polygon ID" << polygonId;

    // Execute the query
    rc = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        nearbyResult pointResult;
        pointResult.name =  reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        pointResult.featureClass= reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1 ));
        pointResult.longitude = sqlite3_column_double(stmt, 2);
        pointResult.latitude = sqlite3_column_double(stmt, 3);
        pointResult.area = 0;
        pointResult.distance = sqlite3_column_double(stmt, 4);
        pointResult.bearing = sqlite3_column_double(stmt, 5);
        pointResult.elementType = sqlite3_column_int(stmt, 6);
        pointResult.elementID = sqlite3_column_int(stmt, 7);
        pointResult.displayOrder = 0;

        pointResults.push_back(pointResult );
    }
    if (rc != SQLITE_DONE)
    {
        qDebug() << "Step failed: " << sqlite3_errmsg(m_LandDB);
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    return 0;
}

double CLandDB::Distance(CLongLat p1, CLongLat p2)
{
	//Temporary distance calculator 
	if (true)
	{
		double diffLon = ( p2.longitude() - p1.longitude() ) * CKG_PI / 180.0;
		double lat1 = p1.latitude() * CKG_PI / 180.0;
		double lon1 = p1.longitude() * CKG_PI / 180.0;
		double lat2 = p2.latitude() * CKG_PI / 180.0;
		double lon2 = p2.longitude() * CKG_PI / 180.0;
		double distance = 6371  * acos( sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2 ) * cos(diffLon) );
		return distance;
	}

	double distance = 0;

	//Prepare for the SQLite call 
	sqlite3_stmt* stmt;

	//The query 
	std::string sQuery = GetQueryFromScript( "Scripts/CalculateDistanceBetweenTwoPoints.sql" );
	int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		std::string error = sqlite3_errmsg(m_LandDB);
		cerr << "Prepare statement for distance query failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
		sqlite3_close(m_LandDB);
		return distance;
	}

	//Bind values to statement
	rc = sqlite3_bind_double(stmt, 1, p1.longitude() );
 	rc = sqlite3_bind_double(stmt, 2, p1.latitude());
	rc = sqlite3_bind_double(stmt, 3, p2.longitude() );
	rc = sqlite3_bind_double(stmt, 4, p2.latitude());

	// Execute the query
	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)
	{
		distance = sqlite3_column_double(stmt, 0);
	}
	else
	{
		std::cerr << "Step failedin distance calculation: " << sqlite3_errmsg(m_LandDB) << std::endl;
		const char* p = sqlite3_errmsg(m_LandDB);
		int k = 0;
	}

	// Finalize the statement
	sqlite3_finalize(stmt);


	return distance;
}

double CLandDB::Bearing(CLongLat p1, CLongLat p2)
{
	double bearing = 0;

	//Prepare for the SQLite call 
	sqlite3_stmt* stmt;

	//The query 
	std::string sQuery = GetQueryFromScript("Scripts/CalculateBearingBetweenTwoPoints.sql");
	int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		std::string error = sqlite3_errmsg(m_LandDB);
		cerr << "Prepare statement for bearing query failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
		sqlite3_close(m_LandDB);
		return bearing;
	}

	//Bind values to statement
	rc = sqlite3_bind_double(stmt, 1, p1.longitude());
	rc = sqlite3_bind_double(stmt, 2, p1.latitude());
	rc = sqlite3_bind_double(stmt, 3, p2.longitude());
	rc = sqlite3_bind_double(stmt, 4, p2.latitude());

	// Execute the query
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW)
	{
		bearing = sqlite3_column_double(stmt, 0);
	}
	else
	{
		std::cerr << "Step failedin bearing calculation: " << sqlite3_errmsg(m_LandDB) << std::endl;
		const char* p = sqlite3_errmsg(m_LandDB);
	}

	// Finalize the statement
	sqlite3_finalize(stmt);


	return bearing;
}

int CLandDB::SearchNames(std::string rsText, NRList& nrResults )
{
	//Prepare for the SQLite call 
	sqlite3_stmt* stmt;

	//The query 
	std::string sQuery = GetQueryFromScript("Scripts/SelectSearchResults.sql");
	int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		std::string error = sqlite3_errmsg(m_LandDB);
		cerr << "Prepare statement for search for name query failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
        return -1;
	}

	//Bind values to statement
	rc = sqlite3_bind_text(stmt, 1, rsText.c_str(), rsText.length(), SQLITE_TRANSIENT );
	rc = sqlite3_bind_text(stmt, 2, rsText.c_str(), rsText.length(), SQLITE_TRANSIENT);

	// Execute the query
	rc = 0;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		double x = sqlite3_column_double(stmt, 1);
		double y = sqlite3_column_double(stmt, 2);
        nearbyResult nrResult;
        nrResult.name = name;
        nrResult.longitude = x;
        nrResult.latitude = y;
        nrResults.push_back(nrResult );
	}
	if (rc != SQLITE_DONE)
	{
		std::cerr << "Step failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
        sqlite3_finalize(stmt);
        return -1;
	}

	// Finalize the statement
	sqlite3_finalize(stmt);
return 0;
}

std::string CLandDB::GetQueryFromScript(std::string rsScriptFilename)
{
    std::string queryPath = m_applicationPath + "/" + rsScriptFilename;

	std::ifstream file(queryPath);
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

void CLandDB::UpdateTableToQuery(std::string& rsQuery, int zoomBand)
{
    QString qQuery(rsQuery.c_str());


    //Exchange polygonX for  polygon + zoomBand 
    QString polygonX = "polygon" + QString::number(zoomBand);
    qQuery.replace( "polygonX", polygonX );
    
    rsQuery = qQuery.toStdString();
}

std::string CLandDB::GetWikipediaValue(int type, int ID)
{
	std::string sWikiID = "";

	//Prepare for the SQLite call 
	sqlite3_stmt* stmt;

	//The query 
	std::string sQuery = GetQueryFromScript("Scripts/SelectWikipediaValue.sql");
	int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		std::string error = sqlite3_errmsg(m_LandDB);
		cerr << "Prepare statement for wikipedia query failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
		return sWikiID;
	}

	//Bind values to statement
	rc = sqlite3_bind_int(stmt, 1, type );
	rc = sqlite3_bind_int(stmt, 2, ID );

	// Execute the query
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW)
	{
		sWikiID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
	}
	else
	{
		std::cerr << "Step failed in getting wikipedia entrycalculation: " << sqlite3_errmsg(m_LandDB) << std::endl;
		const char* p = sqlite3_errmsg(m_LandDB);
	}

	// Finalize the statement
	sqlite3_finalize(stmt);


	return sWikiID;
}

bool CLandDB::IsOnLand(double x, double y)
{
	//Default value for function return 
	bool bIsOnLand = false;

	//Prepare for the SQLite call 
	sqlite3_stmt* stmt;

	//The query 
	std::string sQuery = GetQueryFromScript( "Scripts/SelectIfOnLand.sql" );
	int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
        std::string sErr = sqlite3_errmsg(m_LandDB);
		cout << "Prepare statement for testing if on land or not query failed: " << sErr << std::endl;
		//sqlite3_close(m_LandDB);
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
		inLand == 1 ? bIsOnLand = 1 : bIsOnLand = false;
	}
	else if (rc != SQLITE_DONE)
	{
		std::cerr << "Step failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
	}

	// Finalize the statement
	sqlite3_finalize(stmt);


	return bIsOnLand;
}

int CLandDB::GetVisibleLandPolygonsWKT(double south, double west, double north, double east, int zoomBand, QList<CGeoResult>& geoResults )
{
    QElapsedTimer timer;
    timer.start();

    int retval = 0;
    double minArea = 0.0;


    qDebug() << "LandDB get visible polygons: " << west << "," << south << "," << east << "," << north;
    //Prepare for the SQLite call 
    sqlite3_stmt* stmt;

    //The query 
    std::string sQuery = GetQueryFromScript("Scripts/SelectPolygonsInBoundingBox.sql");
    //UpdateTableToQuery(sQuery, zoomBand);
    int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(m_LandDB);
        QString qe = QString(error.c_str() );
        qWarning() << "Prepare statement for retrieving active land polygon failed: " <<qe;
        return -1;
    }

    //Bind values to statement
    rc = sqlite3_bind_double(stmt, 1, west);
    rc = sqlite3_bind_double(stmt, 2, south);
    rc = sqlite3_bind_double(stmt, 3, east );
    rc = sqlite3_bind_double(stmt, 4, north );

    // Execute the query
    rc = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int wktlength = sqlite3_column_bytes(stmt, 0);
        if (wktlength > 0)
        {
            CGeoResult geoResult;
            geoResult.m_wkt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            geoResult.m_name= reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            geoResult.m_colourIndex = (int)(sqlite3_column_int(stmt, 2));
            geoResult.m_minX = (double)(sqlite3_column_double(stmt, 3));
            geoResult.m_minY = (double)(sqlite3_column_double(stmt, 4));
            geoResult.m_maxX = (double)(sqlite3_column_double(stmt, 5));
            geoResult.m_maxY = (double)(sqlite3_column_double(stmt, 6));
            geoResult.m_labelX = (double)(sqlite3_column_double(stmt, 7));
            geoResult.m_labelY = (double)(sqlite3_column_double(stmt, 8));
            //Add retrieved data to result set 
            geoResults.push_back(geoResult);
        }
    }
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Step failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
        retval = -1;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    //Output how long just the database query took 
    qDebug() << "LandDB query function took" << timer.elapsed() << "milliseconds to get" << geoResults.size() << " geoResults.";


    return retval;
}

int CLandDB::GetVisibleCities(double south, double west, double north, double east, int minPopulation, NRList& pointResults)
{
    QElapsedTimer timer;
    timer.start();

    int retval = 0;

    //Prepare for the SQLite call 
    sqlite3_stmt* stmt;

    //The query 
    std::string sQuery = GetQueryFromScript("Scripts/SelectVisibleCitiesByCapitalAndPopulation.sql");
    int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(m_LandDB);
        QString qe = QString(error.c_str());
        qWarning() << "Prepare statement for retrieving visible cities failed: " << qe;
        return -1;
    }

    //Bind values to statement
    rc = sqlite3_bind_double(stmt, 1, west);
    rc = sqlite3_bind_double(stmt, 2, south);
    rc = sqlite3_bind_double(stmt, 3, east);
    rc = sqlite3_bind_double(stmt, 4, north);
    rc = sqlite3_bind_int(stmt, 5, minPopulation);

    // Execute the query
    rc = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int nameLength = sqlite3_column_bytes(stmt, 0);
        if (nameLength > 0)
        {
            nearbyResult nrResult;
            nrResult.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            nrResult.elementID = (int)(sqlite3_column_int(stmt, 1));
            nrResult.longitude = (double)(sqlite3_column_double(stmt, 2));
            nrResult.latitude = (double)(sqlite3_column_double(stmt, 3));

            //Add retrieved data to result set 
            pointResults.push_back(nrResult);
        }
    }
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Step failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
        retval = -1;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    //Output how long just the database query took 
    qDebug() << "LandDB get visible cities query took" << timer.elapsed() << "milliseconds to get" << pointResults.size() << " point results .";


    return retval;
}

int CLandDB::GetVisiblePoints(double south, double west, double north, double east, NRList& pointResults)
{
    QElapsedTimer timer;
    timer.start();

    int retval = 0;

    //Prepare for the SQLite call 
    sqlite3_stmt* stmt;

    //The query 
    std::string sQuery = GetQueryFromScript("Scripts/SelectPointsInMbr.sql");
    int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(m_LandDB);
        QString qe = QString(error.c_str());
        qWarning() << "Prepare statement for retrieving visible point data failed: " << qe;
        return -1;
    }

    //Bind values to statement
    rc = sqlite3_bind_double(stmt, 1, west);
    rc = sqlite3_bind_double(stmt, 2, south);
    rc = sqlite3_bind_double(stmt, 3, east);
    rc = sqlite3_bind_double(stmt, 4, north);

    // Execute the query
    rc = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int nameLength = sqlite3_column_bytes(stmt, 0);
        if (nameLength > 0)
        {
            nearbyResult nrResult;
            nrResult.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            nrResult.longitude = (double)(sqlite3_column_double(stmt, 1));
            nrResult.latitude= (double)(sqlite3_column_double(stmt, 2));

            //Add retrieved data to result set 
            pointResults.push_back(nrResult );
        }
    }
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Step failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
        retval = -1;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    //Output how long just the database query took 
    qDebug() << "LandDB Points in MBR query took" << timer.elapsed() << "milliseconds to get" << pointResults.size() << " point results .";


    return retval;
}

int CLandDB::getSpecificPolygon(int polygonId, QList<CGeoResult>& polygon)
{
    int retval = 0;
    sqlite3_stmt* stmt = nullptr;

    //The query 
    std::string sQuery = GetQueryFromScript("Scripts/SelectSpecificPolygon.sql");
    int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(m_LandDB);
        QString qe = QString(error.c_str());
        qWarning() << "Prepare statement for retrieving active land polygon failed: " << qe;
        return -1;
    }

    //Bind values to statement
    rc = sqlite3_bind_int(stmt, 1, polygonId );
    // Execute the query
    rc = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int wktlength = sqlite3_column_bytes(stmt, 0);
        if (wktlength > 0)
        {
            CGeoResult geoResult;
            geoResult.m_wkt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

            //Add retrieved data to result set 
            polygon.push_back(geoResult);
        }
    }
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Step failed: " << sqlite3_errmsg(m_LandDB) << std::endl;
        retval = -1;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

return retval;
}

int CLandDB::setLayerSelected(int layerId, int selected )
{
    int retval = 0;
    sqlite3_stmt* stmt = nullptr;

    //The query 
    std::string sQuery = GetQueryFromScript("Scripts/UpdateLayerSelected.sql");
    int rc = sqlite3_prepare_v2(m_LandDB, sQuery.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(m_LandDB);
        QString qe = QString(error.c_str());
        qWarning() << "Prepare statement for retrieving active land polygon failed: " << qe;
        return -1;
    }

    //Bind values to statement
    rc = sqlite3_bind_int(stmt, 1, selected );
    if (rc)
        return -2;
    rc = sqlite3_bind_int(stmt, 2, layerId );
    if (rc)
        return -3;

    // Execute the query
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE )
    {
        QString errorMsg = "Error setting layer selected state for layer " + QString::number( layerId ) + " to " + QString::number( selected );
        errorMsg += QString::fromStdString(sqlite3_errmsg(m_LandDB));
        qWarning() << errorMsg;
        cerr << "Error setting layer selected state: " << sqlite3_errmsg(m_LandDB) << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    return retval;
}