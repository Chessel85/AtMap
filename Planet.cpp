//Planet.cpp
#include "Planet.h"
#include "aliases.h"

#define _USE_MATH_DEFINES // IMPORTANT: Must be defined before #include <cmath> or <math.h>
#include <math.h>

CPlanet::CPlanet()
{
    m_Radius = 6400.0;
    m_onLand = false;
}

CPlanet::~CPlanet()
{
    m_WaterDB.FreeDatabase();
    m_LandDB.FreeDatabase();
    m_LandByCoastlineDB.FreeDatabase();
}

void CPlanet::InitialiseDatabases( const QString& applicationPath, const QString& dbFilename )
{
    m_WaterDB.InitialiseWaterDatabase();
    m_LandDB.InitialiseOSMLandDatabase( dbFilename );
    m_LandDB.setApplicationPath(applicationPath);
    m_LandByCoastlineDB.InitialiseDatabase();
    }

bool CPlanet::DatabasesOK()
{
    return m_WaterDB.IsOK();
}

int CPlanet::GetCurrentLocationName(double x, double y, std::string& name, int& objectID )
{
    //Test the land by coastline database to accurately see if on land or not
    //Coastline database currently set to land.db so actually using single database for both land detection and getting data about land 
    m_onLand = true; //  m_LandByCoastlineDB.IsOnLand(x, y);

    //If on land query the land database to get name of the land otherwise use water database to get name of sea 
    if (m_onLand)
    {
        NRList landLocations;
        landLocations = m_LandDB.FullListOfOSMLand(x, y);
        if (landLocations.size() > 0)
        {
            //Cycle through results to concatenate the list of place names
            for (auto it = landLocations.begin(); it != landLocations.end(); it++)
            {
                nearbyResult nr = *it;
                name += (nr.name + ",");
            }
            name[name.size() - 1] = ' ';
            objectID = landLocations.front().elementID ;
        }
        else
        {
            name = "Unknown";
        }
    }
    else
    {
        m_WaterDB.NameOfWater(x, y, name, objectID );
    }

    //Return something for now
    if (name == "")
    {
        name = "Unknown";
        objectID = -1;
    }

    return 0;
}

int CPlanet::GetBorderingRelations(int objectID, double x, double y, NRList& relResults)
{
    int res = 0;

    if (m_onLand)
    {
        res = m_LandDB.GetBorderingRelations(objectID, x, y, relResults);
    }
    else
    {
        //On water
        res = m_WaterDB.GetBorderingSeas(objectID, x, y, relResults);
    }

    return res;
}

int CPlanet::GetContainedPoints(int relationID, double x, double y, NRList& pointResults)
{
    int res = 0;

    if (m_onLand)
    {
        res = m_LandDB.GetContainedPoints(relationID, x, y, pointResults);
    }

    return res;
}

int CPlanet::GetVisibleData(double south, double west, double north, double east, double stepSize, QList<CGeoResult>& geoResults, NRList& pointResults)
{
    int retval = 0;

    //Determine if two bounding boxes are needed due to international date line, poles or both 
    double south2, west2, north2, east2;
    bool split = splitBoundingBox(south, west, north, east, south2, west2, north2, east2);

    //Translate step size to a zoom band
    int zoomBand = getZoomBand(stepSize);
    //Get polygons first 
    retval = m_LandDB.GetVisibleLandPolygonsWKT(south, west, north, east, zoomBand, geoResults  );

    //If that went okay and there is a split get data from other bounding box
    if (split && retval== 0 )
    {
        retval = m_LandDB.GetVisibleLandPolygonsWKT(south2, west2, north2, east2, zoomBand, geoResults);
    }
    
    //If that went okay get point data 
    if (retval == 0)
    {
        int populationThreshold = TransformZoomBandToPopulation(zoomBand );
        retval = m_LandDB.GetVisibleCities(south, west, north, east, populationThreshold, pointResults);

        //If okay and there is a split get cities in second bounding box
        if (split && retval == 0)
        {
            retval = m_LandDB.GetVisibleCities(south2, west2, north2, east2, populationThreshold, pointResults);
        }
    }
    return retval;
}

int CPlanet::SearchNames(std::string searchText, NRList& nrResults)
{
    return m_LandDB.SearchNames(searchText, nrResults);
}

double CPlanet::distance(QGeoCoordinate& p1, QGeoCoordinate& p2)
{
    CLongLat llp1 = CLongLat(p1.longitude(), p1.latitude());
    CLongLat llp2 = CLongLat(p2.longitude(), p2.latitude());

    return m_LandDB.Distance(llp1, llp2);
}

double CPlanet::Bearing(QGeoCoordinate& p1, QGeoCoordinate& p2)
{
    CLongLat llp1 = CLongLat(p1.longitude(), p1.latitude());
    CLongLat llp2 = CLongLat(p2.longitude(), p2.latitude());

    return m_LandDB.Bearing(llp1, llp2);
}

int CPlanet::setMapViewType(MapViewType mapViewType)
{
    //Hard coding some layers here for now
    int rc1 = 0, rc2 = 0;
    if (mapViewType == viewCountry)
    {
        rc1 = m_LandDB.setLayerSelected(3, 1);
        rc2 = m_LandDB.setLayerSelected(4, 0);
    }
    else if (mapViewType == viewState)
    {
        rc1 = m_LandDB.setLayerSelected(3, 0);
        rc2 = m_LandDB.setLayerSelected(4, 1);
    }

    qDebug() << "Land layer selection changed with codes " << rc1 << "and " << rc2;

    return !(rc1 == 0 && rc2 == 0);
}
int CPlanet::TransformZoomBandToPopulation(int zoomBand )
{
    int populationThreshold = 0;

    //Zoom band goes 1 (most detailed) to 4  (least detailed)
    if (zoomBand>= 4 )
        populationThreshold = 1000000000; //One billion so only get capital cities 
    else if (zoomBand == 3 )
        populationThreshold = 1000000; //One million  plus capital cities 
    else if (zoomBand == 2 )
        populationThreshold = 200000;  //plus capital cities 
    else
        populationThreshold = 0; //All cities 

    return populationThreshold;
}

int CPlanet::getZoomBand(double stepSize)
{
    //Step size is in kilometres 
    int zoomBand = 0;

    if (stepSize >= 500)
        zoomBand = 4;
    else if (stepSize >= 100)
        zoomBand = 3;
    else if (stepSize >= 20)
        zoomBand = 2;
    else
        zoomBand = 1;

    return zoomBand;
}

bool CPlanet::splitBoundingBox(double& south1, double& west1, double& north1, double& east1, double& south2, double& west2, double& north2, double& east2)
{
    bool split = false;

    //Check if bottom left longitude is greater than top right longitude  indicating crosses international date line
    if (west1 > east1 )
    {
        split = true;

        //Set values for second bounding box
        south2 = south1;
        west2 = -180.0;
        north2 = north1;
        east2 = east1;

        //And cut back first bounding box
        east1 = 180.0;
    }

    return split;
}

int CPlanet::getSpecificPolygon(int polygonId, QList<CGeoResult>& polygon)
{
    return m_LandDB.getSpecificPolygon(polygonId, polygon);
}