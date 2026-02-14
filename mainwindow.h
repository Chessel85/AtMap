//MainWindow.h

#pragma once

#include <QMainWindow>
#include "MapManager.h"
#include "CompassPane.h"
#include "InfoPane.h"
#include "MapWidget.h"
#include <QShowEvent>
#include <QGeocoordinate.h>
#include "SearchLocationDialog.h"
#include "GeoResult.h"
#include <qlist.h>
#include "mapViewType.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& applicationPath, const QString& dbFilename, QWidget *parent = nullptr);
    ~MainWindow();

    //Methods
public:

private:
    void SetupMainMenu();
    void SetupWidgets( const QString& applicationPath, const QString& dbFilename );
    void SetupConnects();
    void UpdateInfoPane( );
    bool HandleF6AndTabKeyEvents(QKeyEvent* pKeyEvent);


protected:
    void keyPressEvent(QKeyEvent* event) override;
    void showEvent(QShowEvent *event ) override;
    
    //Signals
signals:
    void dispatchSearchResults(const QMap<QString,QGeoCoordinate> sdResults );

//Slots
public slots:
    void handleSearchRequested(const QString& searchTerm);
    void handleLocationSelected(const QGeoCoordinate& coordinates);
    void PopulateGetLayersRequest(GLList& layerResults);

private slots:
    void onGoToLocationTriggered(); // Slot for the "Go to Location" menu item
    void onSearchLocationTriggered(); // Slot for the "Go to Location" menu item
    void onMapViewTriggered();  //slot for changing map type 

public:
 void onMapClicked(const QPoint& pixelPos, int width, int height);
    
private:
    void onNorthButtonClicked();
    void onEastButtonClicked();
    void onSouthButtonClicked();
    void onWestButtonClicked();
    void onStepMoreButtonClicked();
    void onStepLessButtonClicked();
    void AnnounceInitialFocus();

    //Menu methods 
    void showDocumentation();
    void showLicensing();
    void showAboutDialog();



//Member variables
private:
    //Data variables
    bool m_bInitialShow; //Flag to catch first time application starts up
    bool m_bDataOK;
    QList<CGeoResult> m_geoResults;
    NRList m_pointResults;
    MapViewType m_mapViewType;
    
    //Main areas 
    CMapManager* m_pMapManager;
    CMapWidget* m_pMapArea;
    CCompassPane* m_pCompassPane;
    CInfoPane* m_pInfoPane;
};
