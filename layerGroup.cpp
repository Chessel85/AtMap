//LayerGroup.cpp
#include "LayerGroup.h"

CLayerGroup::CLayerGroup(int layerGroupId, std::string layerGroupName, int layerGroupSelected, int layerGroupIsBase)
{
    m_layerGroupId= layerGroupId;
    m_name = QString::fromStdString( layerGroupName );
    m_isSelected = layerGroupSelected;
    m_isBase = layerGroupIsBase;
    }

CLayerGroup::~CLayerGroup()
{
    //Delete all layers
    for (CLayer* layer : m_layers)
        delete layer;
    m_layers.clear();
}

void CLayerGroup::addLayer(CLayer* pLayer)
{
    m_layers.push_back(pLayer);
}

bool CLayerGroup::isBaseLayerGroup() const
{
    return m_isBase;
}

void CLayerGroup::getLayers(QVector<LayerData>& layers)
{
    //Iterate through the  layers to fill up layer data instances 
    for (CLayer* layer : m_layers)
    {
        LayerData ld;
        ld.groupId = m_layerGroupId;
        ld.groupName = m_name;
        ld.isBaseGroup = m_isBase;
        layer->getLayerData(ld);
        layers.append(ld);
    }
}
