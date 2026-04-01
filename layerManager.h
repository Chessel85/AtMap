//LayerManager.h
#pragma once

#include <qobject.h>
#include <QVector>
#include "layerGroup.h"
#include "mapViewType.h"

class CLayerManager : public QObject 
{
    Q_OBJECT

//Constructor
public:
	CLayerManager();
	~CLayerManager();

//Methods
public:
    void addLayerGroup(CLayerGroup* pLayerGroup);
    void getLayers(QVector<LayerData>& layers);

//Member variables
private:
    QVector<CLayerGroup*> m_layerGroups;
};