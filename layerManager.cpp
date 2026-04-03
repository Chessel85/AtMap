//LayerManager.cpp
#include "LayerManager.h"

CLayerManager::CLayerManager()
{
}

CLayerManager::~CLayerManager()
{
    destroyLayerGroups();
}

void CLayerManager::addLayerGroup(CLayerGroup* pLayerGroup)
{
    m_layerGroups.push_back(pLayerGroup);
}

void CLayerManager::getLayers(QVector<LayerData>& layers)
{
    //Iterate through layer groups and copy data into struct 
    for (CLayerGroup* layerGroup : m_layerGroups)
    {
        //Populate with layer data 
        layerGroup->getLayers(layers);
    }
}

void CLayerManager::destroyLayerGroups()
{
    //Delete all the layer groups
    for (CLayerGroup* layerGroup : m_layerGroups)
        delete layerGroup;
    m_layerGroups.clear();
}
