//LayerGroup.h
#pragma once

#include <QString> 
#include <QVector>
#include "layer.h"
#include "mapViewType.h"

class CLayerGroup 
{
//Constructor
public:
    CLayerGroup(int layerGroupId, std::string layerGroupName, int layerGroupSelected, int layerGroupIsBase);
	~CLayerGroup();

//Methods
public:
    void addLayer(CLayer* pLayer);
    bool isBaseLayerGroup() const;
    void getLayers(QVector<LayerData>& layers);

//Member variables
private:
    int m_layerGroupId;
    QString m_name;
    QVector<CLayer*> m_layers;
    int m_isSelected;
    bool m_isBase;
};