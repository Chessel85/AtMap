//InfoPane.h
#pragma once

#include "InfoWidget.h"
#include <QListWidget.h>
#include "InfoTreeWidget.h"
#include <QKeyEvent> 
#include <QGeocoordinate.h>
#include <qvariant.h>
#include "aliases.h"

class CInfoPane : public QWidget 
{
    Q_OBJECT

//Constructor
public:
    explicit CInfoPane(QWidget* parent = nullptr);

//Methods
public:
    void SaveFocus();
    void RestoreFocus();

private:
    void SetupInfoPane();
    void SetupConnects();

    //Slots
public slots:
    void UpdateInfoPane();
    void ReceiveInfoData(NRList& results);
    void childLocationSelected(const QGeoCoordinate& coordinate);
    void onTabChanged(int index);

protected:
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void GetBorderingRelations(NRList&  relResults);
    void GetContainedPoints(NRList& cityResults);
    void GetLayers( GLList& layerResults);
    void locationSelected(const QGeoCoordinate& coordinate);

//Member variables
private:
    QTabWidget* m_pInfoTabWidget;
    CInfoWidget* m_pInfoBorders;
    CInfoWidget* m_pInfoContains;
    CInfoTreeWidget* m_pInfoMap;
    QWidget* m_pLastFocusedWidget;
};
