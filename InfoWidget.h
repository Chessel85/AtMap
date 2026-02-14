//InfoWidget.h

#pragma once

#include <QListWidget.h>
#include <QObject.h>
#include <QKeyEvent> 
#include <QGeocoordinate.h>
#include <qvariant.h>
#include "aliases.h"

// Define roles for our custom data
enum class InfoRoles 
{
    DistanceRole = Qt::UserRole + 1,
    BearingRole,
    midXRole,
    midYRole
};

class CInfoWidget : public QListWidget
{
    Q_OBJECT

//Constructor
public:
    explicit CInfoWidget(QWidget* parent = nullptr);

//Methods
public:
    void PopulateList(const NRList& relResult);

    //Slots

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void itemDoubleClicked(QListWidgetItem* item);

signals:
    void childLocationSelected(const QGeoCoordinate& coordinate);

private:

//Member variables
private:
};
