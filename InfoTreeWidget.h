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
    midYRole
};

class CInfoTreeWidget : public QTreeWidget 
{
    Q_OBJECT

//Constructor
public:
    explicit CInfoTreeWidget(QWidget* parent = nullptr);

//Methods
public:
    void PopulateList(const GLList& layerResults );

    //Slots

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void itemDoubleClicked(QTreeWidgetItem* item);

signals:
    void childLocationSelected(const QGeoCoordinate& coordinate);

private:

//Member variables
private:
};
