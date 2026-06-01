//InfoTreeWidget.h

#pragma once

#include <QTreeWidget>
#include <QKeyEvent> 
#include <QGeocoordinate.h>
#include <qvariant.h>
#include "aliases.h"

// Define roles for our custom data
enum class InfoTreeRoles 
{
    DistanceRole = Qt::UserRole + 1,
    BearingRole,
    midXRole,
    midYRole,
    beaconRole
};

//Predefines
class CBeacon;

class CInfoTreeWidget : public QTreeWidget 
{
    Q_OBJECT

//Constructor
public:
    explicit CInfoTreeWidget(QWidget* parent = nullptr);
    ~CInfoTreeWidget() override;

//Methods
public:
    void PopulateList(const GLList& layerResults );

    //Overrides
protected:
    void keyPressEvent(QKeyEvent* event) override;

    //Slots
protected:
    void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void onItemDoubleClicked(QTreeWidgetItem* item);

signals:
    void childLocationSelected(const QGeoCoordinate& coordinate);
    void beaconSelected(CBeacon* beacon);

private:
    void destroyBeacons();
    void deleteBeaconsRecursively(QTreeWidgetItem* item);

//Member variables
private:
};
