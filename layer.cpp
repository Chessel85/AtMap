//Layer.cpp
#include "Layer.h"

CLayer::CLayer(int layerId, const std::string layerName, int layerSelected, const std::string geometryCategoryName, int displayOrder, int display)
{
    m_layerId = layerId;
    m_name = QString::fromStdString(layerName);
    m_selected = layerSelected;
    m_geometryCategoryName = QString::fromStdString(geometryCategoryName);
    m_displayOrder = displayOrder;
    m_displayable = display;
    }

CLayer::~CLayer()
{
}

void CLayer::getLayerData(LayerData& ld )
{
    ld.layerId = m_layerId;
    ld.layerName = m_name;
    ld.isSelected = m_selected;
    ld.displayOrder = m_displayOrder;
    ld.display = m_displayable;
}