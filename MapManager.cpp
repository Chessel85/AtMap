//MapManager.cpp
#include "MapManager.h"
#include <qpoint.h>
#include <algorithm>
#include <QElapsedTimer>
#include "WKTConverter.h"
#include <qdebug.h>
#include "aliases.h"

//For scratch code on point analysis 
#include <QFile>
#include <QTextStream>


#define DEFAULT_VIEWABLE_AREA_SCALE 1.6

CMapManager::CMapManager( const QString& applicationPath, const QString& dbFilename, QObject* parent)
    : QObject(parent)
{
    //Initial flags
    m_objectID = -1;

    //Load databases
    m_Planet.InitialiseDatabases( applicationPath, dbFilename );
}

void CMapManager::MoveUser(int dx, int dy)
{
    m_User.MoveByStep(dx, dy);
}

void CMapManager::MoveUser(double x, double y)
{
    m_User.SetLocation(x, y);
}

void CMapManager::MoveUser(QPoint pixelPos, int width, int height)
{
    //Map area received a click and informed main window about it
    //Main window has called this method and will then start the redraw workflow based on the new position of the user 
    // Cconvert the pixel position to a geographic coordinate
    double geoLeft = m_geoBottomLeft.longitude();
    double geoRight = m_geoTopRight.longitude();
    double geoBottom = m_geoBottomLeft.latitude();
    double geoTop = m_geoTopRight.latitude();
    double geoCentreX = ((double)pixelPos.x() / width) * (geoRight - geoLeft) + geoLeft;
    double geoCentreY = (1.0 - (double)pixelPos.y() / height) * (geoTop - geoBottom) + geoBottom;;
    m_User.SetLocation(geoCentreX, geoCentreY); //Latitude then longitude 

    qDebug() << "Clicked at " << pixelPos.x() << pixelPos.y() << " in geo area " << geoLeft << geoBottom << geoRight << geoTop << " giving new geo location of " << geoCentreX << geoCentreY;
}


void CMapManager::ChangeStep(int direction)
{
    m_User.ChangeStep(direction);
}

double CMapManager::GetStep()
{
    return m_User.GetStepSize();
}

bool CMapManager::stepAtMax()
{
    return m_User.stepAtMax();
}

bool CMapManager::stepAtMin()
{
    return m_User.stepAtMin();
}

double CMapManager::GetUserX()
{
    return m_User.GetLocationX();
}


double CMapManager::GetUserY()
{
    return m_User.GetLocationY();
}

int CMapManager::setMapViewType(MapViewType mapViewType)
{
    int rc = m_Planet.setMapViewType(mapViewType );

    return rc;
}

void CMapManager::GetBorderingRelations(NRList& relResults)
{
    double x, y;

    x = m_User.GetLocationX();
    y = m_User.GetLocationY();

        m_Planet.GetBorderingRelations(m_objectID, x, y, relResults);

        //Emit results back to info pane 
        //emit  borderingRelationsUpdated(relResults);
}

void CMapManager::GetContainedPoints(NRList& pointResults)
{
    double x, y;

    x = m_User.GetLocationX();
    y = m_User.GetLocationY();

    m_Planet.GetContainedPoints(m_objectID, x, y, pointResults);

    //Emit results back to info pane
    //emit containedPointsUpdated(pointResults);
}

double CMapManager::Distance(QGeoCoordinate& targetCoordinate)
{
    QGeoCoordinate userCoordinate = m_User.GetCoordinate();
    return m_Planet.distance(userCoordinate, targetCoordinate);
}

double CMapManager::Bearing(QGeoCoordinate& targetCoordinate)
{
    QGeoCoordinate userCoordinate = m_User.GetCoordinate();
    return m_Planet.Bearing(userCoordinate, targetCoordinate);
}



void CMapManager::UpdateMapData(int width, int height, enumRedrawReason redrawReason, QList<CGeoResult>& geoResults, NRList& pointResults )
{
    //Measure how long it takes to get the data back 
    QElapsedTimer timer;
    timer.start();

    //Work out number of steps to display around current location
    //The maximum buffer distance around user is  based on the largest dimension 
    double scaleX, scaleY;
    if (width < height) //portrait 
    {
        scaleY = DEFAULT_VIEWABLE_AREA_SCALE;
        scaleX = DEFAULT_VIEWABLE_AREA_SCALE * (double)width / (double)height;
    }
    else //landscape 
    {
        scaleX = DEFAULT_VIEWABLE_AREA_SCALE;
        scaleY = DEFAULT_VIEWABLE_AREA_SCALE * (double)height /(double)width;
    }

    //Delegate calculation to the user object as it knows most of the useful data
    //m_geoBottomLeft = m_User.CalculateSWOffset(scaleX, scaleY);
    //m_geoTopRight = m_User.CalculateNEOffset(scaleX, scaleY);

    //Use transformer to get bottom left and top right coordinates 
    m_Transformer.SetScreenSize(QSizeF(width, height));
    const QGeoCoordinate centre = m_User.GetCoordinate();
    m_Transformer.CreateProjection(centre);

    int step = int( m_User.GetStepSize() * 1000 );
    m_Transformer.GetGeoBoundingBox(step*scaleX, step*scaleY, m_geoBottomLeft, m_geoTopRight);

    //Get the polygons and other data that are visible in this area 
    int retval = 0;

    //Get rid of any existing data
    geoResults.clear();
    pointResults.clear();


    //Divert off to do polygon point analysis 
    //polygonPointAnalysis();

    //Ask the planet for polygons and points inside the visible area 
    int stepSize = m_User.GetStepSize();
    retval = m_Planet.GetVisibleData(m_geoBottomLeft.latitude(), m_geoBottomLeft.longitude(), m_geoTopRight.latitude(), m_geoTopRight.longitude(), stepSize, geoResults, pointResults );

    //Output how long querying the database took
    qDebug() << "MapManager Data querying took" << timer.elapsed() << "milliseconds to get" << geoResults.size() << " geoResults.";
    timer.start();

    bool ok = CWKTConverter::ParseGeoResults(geoResults);

    //Do not need to parse point data since can access the longitude and latitude in the nearbyResult structure as it is 

    //Output duration of data collection
    qDebug() << "Interpretation of WKT took" << timer.elapsed() << "milliseconds";

        //Emit signal to say data now refreshed so redraw can take place 
    emit mapDataUpdated( geoResults, pointResults, m_geoBottomLeft, m_geoTopRight, m_Transformer);

    //Also send signal to update the labels
    std::string name;
    double x = m_User.GetLocationX();
    double y = m_User.GetLocationY();
    QGeoCoordinate coordinate(y, x);
    m_Planet.GetCurrentLocationName(x, y, name, m_objectID);
    QString qName = QString::fromStdString(name);
    int stepExtreme = 0;
    if (m_User.stepAtMax())
        stepExtreme = 1;
    else if (m_User.stepAtMin())
        stepExtreme = -1;
    emit   labelUpdateRequested(redrawReason, qName, coordinate, m_User.GetStepSize(), stepExtreme);

    //Tell the info pane to prompt for a refresh 
    emit  promptInfoPaneToRefresh();
}


void CMapManager::onMapClicked(const QPoint& pixelPos, int width, int height )
{
    qDebug() << "CMapManager: Received click at pixel:" << pixelPos;

    // NOW, convert the pixel position to a geographic coordinate
    double geoLeft = m_geoBottomLeft.longitude();
    double geoRight = m_geoTopRight.longitude();
    double geoBottom = m_geoBottomLeft.latitude();
    double geoTop = m_geoTopRight.latitude();
    double geoCentreX  = ((double)pixelPos.x() / width) * (geoRight - geoLeft ) + geoLeft;
    double geoCentreY  = (1.0 - (double)pixelPos.y() / height) * (geoTop - geoBottom) + geoBottom;;
    m_User.SetLocation(geoCentreY, geoCentreX); //Latitude then longitude 
    
    qDebug() << "Clicked at " << pixelPos.x() << pixelPos.y() << " in geo area " << geoLeft << geoBottom << geoRight << geoTop << " giving new geo location of " << geoCentreX<< geoCentreY;


    // Now, trigger the map update process:
    //UpdateMapArea(width, height , rrLocationChange);
}

int CMapManager::SearchNames(QString searchText, NRList& nrResults)
{
    return m_Planet.SearchNames(searchText.toStdString(), nrResults);
}



void CMapManager::polygonPointAnalysis()
{
    QList<CGeoResult> portugal;
    QList<CGeoResult> spain;

    m_Planet.getSpecificPolygon(370151, portugal );
    m_Planet.getSpecificPolygon(374271, spain );

    bool ok = CWKTConverter::ParseGeoResults(portugal );
    ok = CWKTConverter::ParseGeoResults(spain );

    QGeoPolygon polygonPortugal = portugal.first().m_geoPolygons.first();
    QGeoPolygon polygonSpain = spain.first().m_geoPolygons.first();

    //Structure to store results
    struct Distances { QGeoCoordinate geoPortugal; int spainIndex; QGeoCoordinate geoSpain; double distance; };
    int numPointsPortugal = polygonPortugal.perimeter().size();
    int numPointsSpain = polygonSpain.perimeter().size();
    std::vector<Distances> results(numPointsPortugal);

    //Loop through each portugal polygon point
    for (int i = 0; i < numPointsPortugal; i++)
    {
        Distances res;
        double distance = 1000;
        int index = 0;
        QGeoCoordinate p2;
        QGeoCoordinate p1 = polygonPortugal.coordinateAt(i);
        for (int j = 0; j < numPointsSpain; j++)
        {
            p2 = polygonSpain.coordinateAt(j);

            double d = p1.distanceTo(p2);
            if (d < distance)
            {
                distance = d;
                index = j;
            }
        }
        res.distance = distance;
        res.geoPortugal = p1;
        res.geoSpain = polygonSpain.coordinateAt( index );
        res.spainIndex = index;

        results[i] = res;
    }

    QFile file( "port-spain.csv" );
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) 
    {
        QTextStream out(&file);

        // Write the header row to the CSV file
        out << "Portugal Latitude,Portugal Longitude,Spain Index,Spain Latitude,Spain Longitude,Distance\n";

        // Loop through the vector and write each struct's data to a new line
        for (const Distances& d : results ) 
        {
            out << d.geoPortugal.latitude() << ","
                << d.geoPortugal.longitude() << ","
                << d.spainIndex << ","
                << d.geoSpain.latitude() << ","
                << d.geoSpain.longitude() << ","
                << d.distance << "\n";
        }

        file.close();
    }
    else 
    {
        qWarning() << "Could not open file for writing:" << file.errorString();
    }
    }