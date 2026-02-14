//MapManager.h

#pragma once

#include <QObject>
#include <QGeoCoordinate>
#include <qgeopolygon.h>
#include <QPoint.h>
#include <qlist.h>
#include <string>
#include "Planet.h"
#include "User.h"
#include "GeoToScreen.h"
#include "RedrawReason.h"
#include <string>
#include "mapViewType.h"

class CMapManager  : public QObject 
{
    Q_OBJECT

//Constructor
public:
    CMapManager( const QString& applicationPath, const QString& dbFilename, QObject* parent = nullptr );

//Methods
public:
    void MoveUser(int dx, int dy);
    void MoveUser(QPoint pixelPos, int width, int height );
    void MoveUser(double x, double y);
    void ChangeStep(int direction);
    double GetStep();
    bool stepAtMax();
    bool stepAtMin();
    double GetUserX();
    double GetUserY();
    int setMapViewType(MapViewType mapViewType);
    void GetBorderingRelations(NRList& relResults );
    void GetContainedPoints(NRList& pointResults);
    double Distance(QGeoCoordinate& targetCoordinate);
    double Bearing(QGeoCoordinate& targetCoordinate);
    void UpdateMapData(int width, int height, enumRedrawReason redrawReason , QList<CGeoResult>& geoResults , NRList& pointResults );
    int SearchNames(QString searchText, NRList& nrResults);


//Sockets
    signals:
    void mapDataUpdated(const QList<CGeoResult>& geoResults, const NRList& pointResults, const QGeoCoordinate& bottomLeft, const QGeoCoordinate& topright, CGeoToScreen& transformer );
    void labelUpdateRequested(enumRedrawReason redrawReason, QString name, QGeoCoordinate& coordinate, double stepSize, int stepExtreme );
    void promptInfoPaneToRefresh();
    void borderingRelationsUpdated(NRList& relResults);
    void containedPointsUpdated(NRList& pointResults);


public slots:
    void onMapClicked(const QPoint& pixelPos, int width, int height );

//Scratch private function for testing matching points from two bordering relations
private:
    void polygonPointAnalysis();

    //Member variables
private:
    CPlanet m_Planet;
    CUser m_User;
    QGeoCoordinate m_geoBottomLeft;
    QGeoCoordinate m_geoTopRight;
    CGeoToScreen m_Transformer;
    int m_objectID;
};
