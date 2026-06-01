//InfoWidget.cpp
#include "InfoWidget.h"
#include "beacon.h"
#include <QDebug> // For debugging output

CInfoWidget::CInfoWidget(QWidget* parent)
: QListWidget(parent)
{
    //connect signal for double clicking an item  which focuses the map on the item 
    connect(this, &QListWidget::itemDoubleClicked,
        this, &CInfoWidget::onItemDoubleClicked);

    //connect key press or mouse click to change selection  which results in a sound being played 
    connect(this, &QListWidget::currentItemChanged,
        this, &CInfoWidget::onCurrentItemChanged);
}

CInfoWidget::~CInfoWidget()
{
    destroyBeacons();
}

void CInfoWidget::destroyBeacons()
{
    for (int i = 0; i < this->count(); ++i)
    {
        QListWidgetItem* oldItem = this->item(i);
        if (!oldItem) continue;

        QVariant v = oldItem->data(static_cast<int>(InfoRoles::beaconRole));
        if (v.isValid())
        {
            CBeacon* oldBeacon = v.value<CBeacon*>();
            delete oldBeacon;
        }
    }
}

void CInfoWidget::PopulateList(const NRList& relResults )
{
    //Clear any previous contents of list
    destroyBeacons();
    this->clear();

    //Populate with new result data 
    for (const nearbyResult& nearbyResult : relResults )
    {
        QString name = QString::fromStdString(nearbyResult.name);
        QString distance = QString("%1").arg(nearbyResult.distance, 0, 'f', 1);
        QString bearing = QString("%1").arg(nearbyResult.bearing, 0, 'f', 1);
        QString itemText = name + ", " + distance + " kilometres at " + bearing + " degrees";

        //Create a beacon
        CBeacon* beacon = new CBeacon();
        beacon->initialise("land", SourceCategory::spatial);
        beacon->setPosition(nearbyResult.longitude, nearbyResult.latitude, 0);

        //Create a new item and populate with data 
        QListWidgetItem* item = new QListWidgetItem( itemText, this );
        item->setData(static_cast<int>(InfoRoles::DistanceRole), QVariant(nearbyResult.distance));
        item->setData(static_cast<int>(InfoRoles::BearingRole), QVariant(nearbyResult.bearing));
        item->setData(static_cast<int>(InfoRoles::midXRole), QVariant(nearbyResult.longitude));
        item->setData(static_cast<int>(InfoRoles::midYRole), QVariant(nearbyResult.latitude));
        item->setData(static_cast<int>(InfoRoles::beaconRole), QVariant::fromValue(beacon));
    }
}

void CInfoWidget::keyPressEvent(QKeyEvent* event)
{
    // Check if the Enter key (Return or Enter on numeric keypad) was pressed
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        // Check there is a selected item 
        if (this->currentItem() != nullptr)
        {
            // Call the same method that handles the double click 
            onItemDoubleClicked(currentItem() );
            // Indicate that the event was handled
            event->accept();
            return; // Stop further processing of this event
        }
    }
    // For all other keys or if conditions are not met, pass the event to the base class
    QListWidget::keyPressEvent(event);
}

void CInfoWidget::onItemDoubleClicked(QListWidgetItem* item)
{
    //Emit a signal to the main window to move map to new location
    if (item != nullptr)
    {
        double latitude = item->data(static_cast<int>(InfoRoles::midYRole)).toDouble();
        double longitude = item->data(static_cast<int>(InfoRoles::midXRole)).toDouble();
        const QGeoCoordinate coordinate(latitude, longitude );
        qDebug() << "Location selected:" << item->text() << "at" << coordinate;
        emit childLocationSelected(coordinate);
    }
}

void CInfoWidget::onCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    // if nothing selected do nothing 
    if (!current) 
        return;

    // Extract the variant payload using the custom role
    QVariant v = current->data(static_cast<int>(InfoRoles::beaconRole));

    if (v.isValid())
    {
        // Safely unpack the pointer
        CBeacon* beacon = v.value<CBeacon*>();

        // Emit the signal up the chain
        emit beaconSelected(beacon);
    }
}