//InfoTreeWidget.cpp
#include "InfoTreeWidget.h"
#include <QMap>
#include <QDebug> // For debugging output

CInfoTreeWidget::CInfoTreeWidget(QWidget* parent)
: QTreeWidget(parent)
{
    //Need to handle double click on an item to move to the location 
    connect(this, &QTreeWidget::itemDoubleClicked, this, &CInfoTreeWidget::itemDoubleClicked);
}

void CInfoTreeWidget::PopulateList(const GLList& layerResults)
{
    //Capture the open and closed state of each top level node
    QMap<QString, bool> savedExpandedState;
    for (int i = 0; i < topLevelItemCount(); i++) 
    {
        QTreeWidgetItem* item = topLevelItem(i);
        savedExpandedState[item->text(0)] = item->isExpanded();
    }
    //Clear the list and populate with new results 
    clear();

    //Iterate through the layers and create a new parent node for each layer and then populate that node 
    for (const geoLayer& layer : layerResults)
    {
        QString nodeName = QString::fromStdString(layer.layerName);
        //Create a new item and populate with data 
        QTreeWidgetItem* topNode = new QTreeWidgetItem(this);
        topNode->setText(0, nodeName);
        addTopLevelItem(topNode);

        //Retrieve previous expanded state 
        bool shouldExpand = savedExpandedState[nodeName];
        topNode->setExpanded(shouldExpand);

        //Iterate through the entries for this layer and populate top node 
        for (const nearbyResult& nrResult : layer.geoResults)
        {
            QString name = QString::fromStdString( nrResult.name );
            QString distance = QString("%1").arg(nrResult.distance, 0, 'f', 1);
            QString bearing = QString("%1").arg(nrResult.bearing, 0, 'f', 1);
            QString itemText = name + ", " + distance + " kilometres at " + bearing + " degrees";

            //Create a new item and populate with data 
            QTreeWidgetItem* item = new QTreeWidgetItem(topNode);
            item->setText(0, itemText);

            //Associate distance, bearing and coordinnates with item 
            item->setData(0, static_cast<double>(InfoTreeRoles::DistanceRole), QVariant(nrResult.distance));
            item->setData(0, static_cast<double>(InfoTreeRoles::BearingRole), QVariant(nrResult.bearing));
            item->setData(0, static_cast<double>(InfoTreeRoles::midXRole), QVariant(nrResult.longitude));
            item->setData(0, static_cast<double>(InfoTreeRoles::midYRole), QVariant(nrResult.latitude));

            topNode->addChild(item);
        }
    }
}

void CInfoTreeWidget::keyPressEvent(QKeyEvent* event)
{
    // Check if the Enter key (Return or Enter on numeric keypad) was pressed
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        // Check there is a selected item 
        if (this->currentItem() != nullptr)
        {
            // Call the same method that handles the double click 
            QTreeWidgetItem* currentItem = this->currentItem();
            itemDoubleClicked(currentItem);
            // Indicate that the event was handled
            event->accept();
            return; // Stop further processing of this event
        }
    }
    // For all other keys or if conditions are not met, pass the event to the base class
    QTreeWidget::keyPressEvent(event);
}

void CInfoTreeWidget::itemDoubleClicked(QTreeWidgetItem* item)
{
    //Emit a signal to the main window to move map to new location
    if (item != nullptr)
    {
        double latitude = item->data(0,static_cast<double>(InfoTreeRoles::midYRole)).toDouble();
        double longitude = item->data(0,static_cast<double>(InfoTreeRoles::midXRole)).toDouble();
        const QGeoCoordinate coordinate(latitude, longitude );
        qDebug() << "Location selected:" << item->text(0) << "at" << coordinate;
        emit childLocationSelected(coordinate);
    }
}