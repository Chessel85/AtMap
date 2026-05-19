//InfoWidget.h

#pragma once

#include <QListWidget>
#include <QKeyEvent> 
#include <QGeoCoordinate>
#include <QVariant>
#include "aliases.h"

// Define roles for our custom data
enum class InfoRoles 
{
    DistanceRole = Qt::UserRole + 1,
    BearingRole,
    midXRole,
    midYRole,
    beaconRole
};

//Predefines
class CBeacon;

class CInfoWidget : public QListWidget
{
    Q_OBJECT

//Constructor
public:
    explicit CInfoWidget(QWidget* parent = nullptr);
    ~CInfoWidget() override;

//Methods
public:
    void PopulateList(const NRList& relResult);

//Overrides
protected:
    void keyPressEvent(QKeyEvent* event) override;

    //Slots
protected:
    void onCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void onItemDoubleClicked(QListWidgetItem* item);

signals:
    void childLocationSelected(const QGeoCoordinate& coordinate);
    void beaconSelected(CBeacon* beacon);

private:
    void destroyBeacons();

//Member variables
private:
};
