//MainWindow.cpp
#include "mainwindow.h"
#include <QApplication>   
#include <QMenuBar.h>
#include <QActionGroup>
#include <QDesktopServices>
#include <QUrl.h>
#include <QHBoxLayout> // For horizontal layout
#include <QVBoxLayout> // For vertical layout
#include <QSplitter>   // For resizable areas
#include <QWidget>     // Base widget for containers
#include <QGridLayout> // For grid layout
#include <QKeyEvent>   // For key event handling#include <QDebug> // Optional: for debugging output
#include <QAccessibleValueChangeEvent> 
#include <QAccessibleAnnouncementEvent>
#include <QMessageBox>
#include <QTimer>
#include <QMap.h>
#include "gotoLocationDialog.h"
#include "RedrawReason.h"



MainWindow::MainWindow(const QString& applicationPath, const QString& dbFilename, QWidget *parent)
    : QMainWindow(parent)
{
//Create the menu 
    SetupMainMenu();

    //Create the widget structure of map, compass and info pane
    SetupWidgets( applicationPath, dbFilename );

    //Now all objects exist, and to keep them all in one place, create connects
    SetupConnects();

    //Set title of main window
    setWindowTitle("Accessible Atlas");

    //Set size of main window and a minimum size
    resize(1200, 800);
    setMinimumSize(800, 600);

    //Set initial flags and variable defaults 
    m_bInitialShow = true;
}

MainWindow::~MainWindow()
{
}

void MainWindow::SetupMainMenu()
{
    //Create the menu bar
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    //Create top level menus onto the menu bar
    QMenu* fileMenu = menuBar->addMenu(tr( "&File" ));
    QMenu* mapMenu = menuBar->addMenu(tr("&Map"));
    QMenu* helpMenu = menuBar->addMenu(tr("&Help"));

    //Set up menu items in the file menu 
    //Quit gets connected to the application quit slot 
    QAction* quitAction = fileMenu->addAction(tr("&Quit"));
    quitAction->setShortcut(QKeySequence::Quit);
    quitAction->setMenuRole(QAction::QuitRole);
    connect(quitAction, &QAction::triggered, this, &MainWindow::close);

    //Add to map menu 
    //Goto coordinates 
    QAction* goToLocationAction = mapMenu->addAction(tr("&Go to coordinates..."));
    goToLocationAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    connect(goToLocationAction, &QAction::triggered, this, &MainWindow::onGoToLocationTriggered);

    //Search for location
    QAction* searchLocationAction = mapMenu->addAction(tr("&Search location..."));
    searchLocationAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    connect(searchLocationAction, &QAction::triggered, this, &MainWindow::onSearchLocationTriggered);


    //Put in a seperator 
    mapMenu->addSeparator();

    //Create an action group to hold the map view options 
    QActionGroup* mapViewGroup = new QActionGroup(this);

    //Set map to countries
    QAction* mapViewCountryAction = mapMenu->addAction(tr("&Country view"));
    mapViewCountryAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
    mapViewCountryAction->setCheckable(true); 
    mapViewCountryAction->setChecked(true);   // Set as default active view
    m_mapViewType = viewCountry;
    mapViewCountryAction->setData(viewCountry); // store enum of country style map 
    mapViewCountryAction->setActionGroup(mapViewGroup); // Add to group
    connect(mapViewCountryAction, &QAction::triggered, this, &MainWindow::onMapViewTriggered);


    //Set map to states and provinces 
    QAction* mapViewStateAction = mapMenu->addAction(tr("&State/province view"));
    mapViewStateAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
    mapViewStateAction->setCheckable(true);
    mapViewStateAction->setData(viewState); // store enum of state style map 
    mapViewStateAction->setActionGroup(mapViewGroup); // Add to group
    connect(mapViewStateAction, &QAction::triggered, this, &MainWindow::onMapViewTriggered);

    //Help menu
    QAction* documentationAction = helpMenu->addAction(tr("&Documentation"));
    connect(documentationAction, &QAction::triggered, this, &MainWindow::showDocumentation);
    QAction* licensingAction = helpMenu->addAction(tr("&Licensing"));
    connect(licensingAction, &QAction::triggered, this, &MainWindow::showLicensing);
    QAction* aboutAction = helpMenu->addAction(tr("&About..."));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

void MainWindow::SetupWidgets( const QString& applicationPath, const QString& dbFilename )
{
    //Set up map manager on the heap to avoid events calling member variables during destruction
    m_pMapManager = new CMapManager(applicationPath, dbFilename, this);

    //Create main splitter
    QSplitter* pMainSplitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(pMainSplitter);

    //Create a map area as a child of the main splitter
    m_pMapArea = new CMapWidget(pMainSplitter);
    m_pMapArea->setMinimumSize(50, 50);

    //Create the splitter that goes on the right side 
    QSplitter* pRightSplitter = new QSplitter(Qt::Vertical, pMainSplitter);
    pRightSplitter->setMinimumSize(50, 50);

    //Add the map and right column widgets to the main splitter where the order of adding them goes left to right
    pMainSplitter->addWidget(m_pMapArea);
    pMainSplitter->addWidget(pRightSplitter);

    //Add an initial size for the main splitter 
    QList<int> initialSizes;
    initialSizes.append(400);
    initialSizes.append(200);
    pMainSplitter->setSizes(initialSizes);

    //Set stretch factors to keep ratio of map and right hand column constant at 2 to 1
    pMainSplitter->setStretchFactor(0, 2);
    pMainSplitter->setStretchFactor(1, 1);

    //Create compass pane to add to right splitter
    m_pCompassPane = new CCompassPane(pRightSplitter);

    //Create info pane to add to right splitter
    m_pInfoPane = new CInfoPane(pRightSplitter);
    m_pInfoPane->setStyleSheet("background-color: lightgreen; border: 1px solid green;");
    m_pInfoPane->setMinimumSize(50, 50);
    m_pInfoPane->setAccessibleName("Location Information Pane");


    //Add these to the right splitter, order is top to bottom
    pRightSplitter->addWidget(m_pCompassPane);
    pRightSplitter->addWidget(m_pInfoPane);

    //Set initial size for compass and info pane
    QList<int> initialRightSizes;
    initialRightSizes.append(130);
    initialRightSizes.append(370);
    pRightSplitter->setSizes(initialRightSizes);

    //Set ratio of top and bottom of right splitter
    pRightSplitter->setStretchFactor(0, 1);
    pRightSplitter->setStretchFactor(1, 2);
}

void MainWindow::SetupConnects()
{
    // Connect the signal from CMapManager to the slot in CMapWidget so map manager can cause a redraw of the map 
    connect(m_pMapManager, &CMapManager::mapDataUpdated,
        m_pMapArea, &CMapWidget::receiveMapData);

    //Create a connect so map area can tell main window of a mouse click 
    connect(m_pMapArea, &CMapWidget::clickedAtPixel,
        this, &MainWindow::onMapClicked);

    //Connects to receive compass signals and process
    connect(m_pCompassPane, &CCompassPane::northRequested, this, &MainWindow::onNorthButtonClicked);
    connect(m_pCompassPane, &CCompassPane::eastRequested, this, &MainWindow::onEastButtonClicked);
    connect(m_pCompassPane, &CCompassPane::southRequested, this, &MainWindow::onSouthButtonClicked);
    connect(m_pCompassPane, &CCompassPane::westRequested, this, &MainWindow::onWestButtonClicked);
    connect(m_pCompassPane, &CCompassPane::stepLessRequested, this, &MainWindow::onStepLessButtonClicked);
    connect(m_pCompassPane, &CCompassPane::stepMoreRequested, this, &MainWindow::onStepMoreButtonClicked);

    //And from map manager to compass 
    connect(m_pMapManager, &CMapManager::labelUpdateRequested, m_pCompassPane, &CCompassPane::UpdateLabels);

    //Map manager needs to tell info pane to request a refresh
    connect(m_pMapManager, &CMapManager::promptInfoPaneToRefresh, m_pInfoPane, &CInfoPane::UpdateInfoPane);

    //Connect info pane to map manager to get data for populating tabbed info widgets 
    connect(m_pInfoPane, &CInfoPane::GetBorderingRelations, m_pMapManager, &CMapManager::GetBorderingRelations);
    connect(m_pInfoPane, &CInfoPane::GetContainedPoints, m_pMapManager, &CMapManager::GetContainedPoints);

    //InfoMap can be handled by main window 
    connect(m_pInfoPane, &CInfoPane::GetLayers, this, &MainWindow::PopulateGetLayersRequest);

    //And connect map manager to info pane to return results  and populate children of the tab widget 
    connect(m_pMapManager, &CMapManager::borderingRelationsUpdated, m_pInfoPane, &CInfoPane::ReceiveInfoData);
    connect(m_pMapManager, &CMapManager::containedPointsUpdated, m_pInfoPane, &CInfoPane::ReceiveInfoData);

    //InfoPane says a location has been selected 
    connect(m_pInfoPane, &CInfoPane::locationSelected, this, &MainWindow::handleLocationSelected);
    }






void MainWindow::showEvent(QShowEvent *event)
{
    //Call base class implementation first 
    QMainWindow::showEvent(event);

    //Carry out following only on initial show
    if (m_bInitialShow)
    {
        m_bInitialShow = false;
        m_pMapManager->setMapViewType(m_mapViewType);
        m_pMapManager->UpdateMapData(m_pMapArea->width(), m_pMapArea->height(), rrBoth, m_geoResults, m_pointResults);
    }
}

void MainWindow::AnnounceInitialFocus()
{
    //m_pCompassPane->RestoreFocus();
    //QAccessibleAnnouncementEvent event(m_pNorthButton, m_pNorthButton->accessibleName());
    //event.setPoliteness(QAccessible::AnnouncementPoliteness::Assertive);
    //QAccessible::updateAccessibility(&event);
}

void MainWindow::keyPressEvent(QKeyEvent* keyEvent)
{
    //Handles F6 
    if (HandleF6AndTabKeyEvents(keyEvent))
        keyEvent->accept();
    else 
        QWidget::keyPressEvent(keyEvent);

    //Need something to handle mouse click events on map area 
    //xxxx 
}


bool MainWindow::HandleF6AndTabKeyEvents(QKeyEvent* pKeyEvent)
{
    QWidget* focusedWidget = QApplication::focusWidget();

    //Indicate did not handle keystroke if there is no focused widget 
    if (!focusedWidget)
        return false;

    //Capture F6 and shift F6 
    if (pKeyEvent->key() == Qt::Key_F6)
    {
        // Check which widget currently has focus
        if (m_pCompassPane->isAncestorOf(focusedWidget))
        {
            //Focus is on a widget in the compass area 
            m_pCompassPane->SaveFocus();
            m_pInfoPane->RestoreFocus();
            return true;
        }
        else if (m_pInfoPane->isAncestorOf(focusedWidget ) )
        {
            //Focus is in info pane 
            m_pInfoPane->SaveFocus();
            m_pCompassPane->RestoreFocus();
            return true;
        }
    }

    return false;  //If got this far then it wasn't handled 
}

void MainWindow::onNorthButtonClicked()
{
    m_pCompassPane->setEnabledAllButtons(false);
    m_pMapManager->MoveUser(0, 1);
    m_pMapManager->UpdateMapData(m_pMapArea->width(), m_pMapArea->height(), rrLocationChange, m_geoResults, m_pointResults );
}

void MainWindow::onEastButtonClicked()
{
    m_pCompassPane->setEnabledAllButtons(false);
    m_pMapManager->MoveUser(1, 0);
    m_pMapManager->UpdateMapData(m_pMapArea->width(), m_pMapArea->height(), rrLocationChange, m_geoResults, m_pointResults);
}

void MainWindow::onSouthButtonClicked()
{
    m_pCompassPane->setEnabledAllButtons(false);
    m_pMapManager->MoveUser(0, -1);
    m_pMapManager->UpdateMapData(m_pMapArea->width(), m_pMapArea->height(), rrLocationChange, m_geoResults, m_pointResults);
}

void MainWindow::onWestButtonClicked()
{
    m_pCompassPane->setEnabledAllButtons(false);
    m_pMapManager->MoveUser(-1, 0);
    m_pMapManager->UpdateMapData(m_pMapArea->width(), m_pMapArea->height(), rrLocationChange, m_geoResults, m_pointResults);
}

void MainWindow::onStepMoreButtonClicked()
{
    m_pCompassPane->setEnabledAllButtons(false);
    m_pMapManager->ChangeStep(1);
    m_pMapManager->UpdateMapData(m_pMapArea->width(), m_pMapArea->height(), rrStepSizeChange , m_geoResults, m_pointResults);
}

void MainWindow::onStepLessButtonClicked()
{
    m_pCompassPane->setEnabledAllButtons(false);
    m_pMapManager->ChangeStep(-1);
    m_pMapManager->UpdateMapData(m_pMapArea->width(), m_pMapArea->height(), rrStepSizeChange, m_geoResults, m_pointResults);
}

void MainWindow::onMapClicked(const QPoint& pixelPos, int width, int height)
{
    m_pCompassPane->setEnabledAllButtons(false);
    m_pMapManager->MoveUser(pixelPos, width, height );
    m_pMapManager->UpdateMapData(m_pMapArea->width(), m_pMapArea->height(), rrLocationChange, m_geoResults, m_pointResults);
}

void MainWindow::showAboutDialog() 
{
    QMessageBox::about(this, tr("About Accessible Atlas"),
        tr("Accessible Atlas version 0.0.2 released August 2025"));
}

void MainWindow::showDocumentation()
{
    //Create path based on application path plus Documentation subfolder 
    QString docPath = QCoreApplication::applicationDirPath() + "/Documents/Accessible Atlas documentation.html";

    //Call desktop services to open the HTML file 
    QDesktopServices::openUrl(QUrl::fromLocalFile(docPath));
}

void MainWindow::showLicensing()
{
    //Create path based on application path plus Documentation subfolder 
    QString docPath = QCoreApplication::applicationDirPath() + "/Documents/Licensing.html";

    //Call desktop services to open the HTML file 
    QDesktopServices::openUrl(QUrl::fromLocalFile(docPath));
}

void MainWindow::onGoToLocationTriggered()
{
    //Capture current focus so can return to it after dialogue closes
    QWidget* focusedWidget = QApplication::focusWidget();

    CGotoLocationDialog dialog(this); 

    //Seed the dialog with current coordinates 
    double latitude = m_pMapManager->GetUserY();
    double longitude = m_pMapManager->GetUserX();
    dialog.setLatitude(latitude);
    dialog.setLongitude(longitude);

    //Display dialog
    if (dialog.exec() == QDialog::Accepted)
    {
        double y = dialog.getLatitude();
        double x = dialog.getLongitude();

        //Update the map
        m_pCompassPane->setEnabledAllButtons(false);
        m_pMapManager->MoveUser(x, y);
        m_pMapManager->UpdateMapData(m_pMapArea->width(), m_pMapArea->height(), rrLocationChange, m_geoResults, m_pointResults);
    }

    //Return focus to pre-dialogue widget
    if (focusedWidget)
        focusedWidget->setFocus();
}

void MainWindow::onSearchLocationTriggered()
{
    //Capture current focused widget so can return to it later
    QWidget* focusedWidget = QApplication::focusWidget();

    // Display the search for location dialog 
    CSearchLocationDialog searchLocationDialog(this);

// Connect signals from this specific dialog instance to MainWindow slots
connect(&searchLocationDialog, &CSearchLocationDialog::searchRequested, this, &MainWindow::handleSearchRequested);
    connect(&searchLocationDialog, &CSearchLocationDialog::locationSelected, this, &MainWindow::handleLocationSelected);
    connect(this, &MainWindow::dispatchSearchResults, &searchLocationDialog, &CSearchLocationDialog::handleSearchResults);


    searchLocationDialog.exec(); // Show the dialog modally

    //Slots and signals handle getting data to and from the search dialog 

    //Return focus to original widget 
    if (focusedWidget)
        focusedWidget->setFocus();
}

void MainWindow::handleSearchRequested(const QString& searchTerm)
{
    //Create the result data structure here which will get signalled back to the search dialog
    NRList nrResults;
    int retval =     m_pMapManager->SearchNames(searchTerm, nrResults);

    if (retval != 0)
    {
        qDebug() << "MainWindow :  handleSearchRequested : Bad result from search by name :" << retval;
    }

    //Convert data format to what the dialog is expecting
    QMap<QString, QGeoCoordinate> sdResults;
    for (nearbyResult& nrResult : nrResults)
    {
        QString qName = QString::fromStdString(nrResult.name);
        QGeoCoordinate geoCoordinate(nrResult.latitude, nrResult.longitude);
        sdResults[qName ] = geoCoordinate;
    }
    emit dispatchSearchResults(sdResults);
}

void MainWindow::handleLocationSelected(const QGeoCoordinate& coordinate)
{
    qDebug() << "MainWindow moving to " << coordinate;
    m_pCompassPane->setEnabledAllButtons(false);
    m_pMapManager->MoveUser(coordinate.longitude(), coordinate.latitude());
    m_pMapManager->UpdateMapData(m_pMapArea->width(), m_pMapArea->height(), rrLocationChange, m_geoResults, m_pointResults);
}

void MainWindow::PopulateGetLayersRequest(GLList& layerResults)
{
    //This is a fudge for now until implement layers fully across the whole application
    //Iterate through the layers requested and populate each layer based on name of layer

    for (geoLayer& layer : layerResults)
    {
        if (layer.layerName == "Countries")
        {
            //Keep track of number of duplicates 
            QMap<QString, int> duplicates;

            //Populate from the geoResults list
            for (CGeoResult& geoResult : m_geoResults)
            {
                //Skip if a duplicate
                QString n = QString::fromStdString(geoResult.m_name);
                if (duplicates.contains(n))
                {
                    duplicates[n]++;
                    continue;
                }
                else
                    duplicates[n] = 1;

                nearbyResult nrResult;
                nrResult.name = geoResult.m_name;

                double countryLongitude = geoResult.m_labelX;//geoResult.m_minY + ( geoResult.m_maxY - geoResult.m_minY) / 2 ;
                double countryLatitude = geoResult.m_labelY; // geoResult.m_minX + (geoResult.m_maxX - geoResult.m_minX) / 2;
                QGeoCoordinate countryCentre = QGeoCoordinate(countryLatitude, countryLongitude);
                nrResult.distance = m_pMapManager->Distance(countryCentre);
                nrResult.bearing = m_pMapManager->Bearing(countryCentre);
                nrResult.longitude = countryLongitude;
                nrResult.latitude = countryLatitude;

                layer.geoResults.push_back(nrResult);
            }

            //Output duplicate info 
            for (auto const& key : duplicates.keys())
            {
                const int value = duplicates.value(key);
                qDebug() << key << " not added" << value-1 << " times.";
            }
        }
        if (layer.layerName == "Cities")
        {
            //Populate from the pointResults list 
            for (nearbyResult&  pointResult : m_pointResults)
            {
                nearbyResult nrResult;
                nrResult.name = pointResult.name;
                QGeoCoordinate cityCoordinate = QGeoCoordinate( pointResult.latitude, pointResult.longitude);
                nrResult.distance = m_pMapManager->Distance(cityCoordinate);
                nrResult.bearing = m_pMapManager->Bearing(cityCoordinate);
                nrResult.longitude = cityCoordinate.longitude();
                nrResult.latitude = cityCoordinate.latitude();

                layer.geoResults.push_back(nrResult);
            }
        }
    }
}

void MainWindow::onMapViewTriggered() 
{
    //Change the map being displayed 
    MapViewType oldMapType = m_mapViewType;
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) 
    {
        int viewType = action->data().toInt();

        switch (viewType) {
        case viewCountry:
            m_mapViewType = viewCountry;
            break;
        case viewState:
            m_mapViewType = viewState;
            break;
        }
    }

    //Do a map refresh if needed
    if (oldMapType != m_mapViewType)
    {
        qDebug() << "Map type changing from" << ( oldMapType == viewCountry ? "country" : "state")
            << "to" << (m_mapViewType == viewCountry ? "country" : "state");

        m_pCompassPane->setEnabledAllButtons(false);
        m_pMapManager->setMapViewType(m_mapViewType);
        m_pMapManager->UpdateMapData(m_pMapArea->width(), m_pMapArea->height(), rrLocationChange, m_geoResults, m_pointResults);
    }
}