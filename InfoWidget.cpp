//InfoWidget.cpp
#include "InfoWidget.h"
#include <QDebug> // For debugging output

CInfoWidget::CInfoWidget(QWidget* parent)
: QListWidget(parent)
{
    connect(this, &QListWidget::itemDoubleClicked, this, &CInfoWidget::itemDoubleClicked);
}

void CInfoWidget::PopulateList(const NRList& relResults )
{
    //Clear the list and populate with new reuslts
    this->clear();

    for (const nearbyResult& nearbyResult : relResults )
    {
        QString name = QString::fromStdString(nearbyResult.name);
        QString distance = QString("%1").arg(nearbyResult.distance, 0, 'f', 1);
        QString bearing = QString("%1").arg(nearbyResult.bearing, 0, 'f', 1);
        QString itemText = name + ", " + distance + " kilometres at " + bearing + " degrees";

        //Create a new item and populate with data 
        QListWidgetItem* item = new QListWidgetItem( itemText, this );
        item->setData(static_cast<double>(InfoRoles::DistanceRole), QVariant(nearbyResult.distance));
        item->setData(static_cast<double>(InfoRoles::BearingRole), QVariant(nearbyResult.bearing));
        item->setData(static_cast<double>(InfoRoles::midXRole), QVariant(nearbyResult.longitude));
        item->setData(static_cast<double>(InfoRoles::midYRole), QVariant(nearbyResult.latitude));
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
            QListWidgetItem* currentItem = this->currentItem();
            itemDoubleClicked(currentItem);
            // Indicate that the event was handled
            event->accept();
            return; // Stop further processing of this event
        }
    }
    // For all other keys or if conditions are not met, pass the event to the base class
    QListWidget::keyPressEvent(event);
}

void CInfoWidget::itemDoubleClicked(QListWidgetItem* item)
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