//InfoPane.cpp
#include "InfoPane.h"
#include <QVBoxLayout>
#include <QApplication.h>
#include <QDebug> // For debugging output

CInfoPane::CInfoPane(QWidget* parent)
: QWidget(parent)
{
    SetupInfoPane();

    //Set up connects
    SetupConnects();
}

void CInfoPane::SetupInfoPane()
{
    //Create the tab widget
    m_pInfoTabWidget = new QTabWidget(this);

    //Create the child panes and add to the tab widget
    m_pInfoBorders = new CInfoWidget(nullptr);
    m_pInfoTabWidget->addTab(m_pInfoBorders, tr("&Borders"));
    m_pInfoContains = new CInfoWidget(nullptr);
    m_pInfoTabWidget->addTab(m_pInfoContains, tr("&Contains"));
    m_pInfoMap = new CInfoTreeWidget(nullptr);
    m_pInfoTabWidget->addTab(m_pInfoMap, tr("Ma&p"));

    //Consists of a vbox to host the tab widget 
    //Create the VBox layout and add the widgets 
    QVBoxLayout* pInfoLayout = new QVBoxLayout(this );
    pInfoLayout->addWidget(m_pInfoTabWidget);

    // Install event filters on the info pane widgets so keystrokes handled by this class eventfilter  for F6 key 
    m_pInfoTabWidget->installEventFilter(this);
    m_pInfoBorders->installEventFilter(this);
    m_pInfoContains->installEventFilter(this);
    m_pInfoMap->installEventFilter(this);

    //Variable defaults
    m_pLastFocusedWidget = nullptr;
    }

void CInfoPane::SetupConnects()
{
    //Connect the tab change event to requesting an update for the child info widget
    connect(m_pInfoTabWidget, &QTabWidget::currentChanged, this, &CInfoPane::onTabChanged);

    //Allow child widgets to signal a location selected
    connect(m_pInfoBorders, &CInfoWidget::childLocationSelected, this, &CInfoPane::locationSelected);
    connect(m_pInfoContains, &CInfoWidget::childLocationSelected, this, &CInfoPane::locationSelected);
    connect(m_pInfoMap, &CInfoTreeWidget::childLocationSelected, this, &CInfoPane::locationSelected);
}

void CInfoPane::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
}

void CInfoPane::UpdateInfoPane()
{
    //Work out which tab is active and then go get this data and populate the sub widget
    QWidget* activeTab = m_pInfoTabWidget->currentWidget();
    if (!activeTab)
        return; //No active tab 

    if (m_pInfoBorders == qobject_cast<CInfoWidget*>(activeTab))
    {
        //Is borders tab 
        NRList relResults;
        emit GetBorderingRelations(relResults);
        m_pInfoBorders->PopulateList(relResults);
    }
    else if (m_pInfoContains == qobject_cast<CInfoWidget*>(activeTab))
    {
        //Get points inside current country  
        NRList cityResults;
        emit GetContainedPoints(cityResults);
        m_pInfoContains->PopulateList(cityResults);
    }
    else if (m_pInfoMap == qobject_cast<CInfoTreeWidget*>(activeTab))
    {
        //Get countries and cities visible on map 
        //Populate a result structure with the layers to retrieve 
        GLList layerResults;
        geoLayer countries;
        countries.layerName = "Countries";
        geoLayer cities;
        cities.layerName = "Cities";
        layerResults.push_back(countries);
        layerResults.push_back(cities);

        //Emit signal to get data from map manager 
        emit GetLayers(layerResults);
        m_pInfoMap->PopulateList(layerResults);
    }
}

void CInfoPane::ReceiveInfoData(NRList& results)
{
    //Get active tab so know what to update 
    QWidget* activeTab = m_pInfoTabWidget->currentWidget();
    if (!activeTab)
        return; //No active tab 

    if (m_pInfoBorders == qobject_cast<CInfoWidget*>(activeTab))
    {
        //Is borders tab 
        m_pInfoBorders->PopulateList(results);
    }
    else if (m_pInfoContains == qobject_cast<CInfoWidget*>(activeTab))
    {
        //Get points inside current country  
        m_pInfoContains->PopulateList(results);
    }
}

void CInfoPane::onTabChanged(int index)
{
    index++; //Get rid of warning 
    UpdateInfoPane();
}

void CInfoPane::childLocationSelected(const QGeoCoordinate& coordinate)
{
    //Pass on up to main window
    emit locationSelected(coordinate);
}

void CInfoPane::RestoreFocus()
{
    //If last widget is known 
    if (m_pLastFocusedWidget)
        m_pLastFocusedWidget->setFocus();
    else
    {
        QWidget* currentChild = m_pInfoTabWidget->currentWidget();
        if (currentChild)
            currentChild->setFocus();
    }
}

void CInfoPane::SaveFocus()
{
    m_pLastFocusedWidget = QApplication::focusWidget();
}