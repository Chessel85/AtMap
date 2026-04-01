//Layer.h
#pragma once

#include <qobject.h>
#include <QString> 
#include "mapViewType.h"


class CLayer 
{
//Constructor
public:
    CLayer(int layerId, const std::string layerName, int layerSelected, const std::string geometryCategoryName, int displayOrder, int display);
	~CLayer();

//Methods
public:
    void getLayerData(LayerData& ld );

//Member variables
private:
    int m_layerId;
    QString m_name;
    int m_layerGroupIndex;
    QString m_geometryCategoryName;
    int m_displayOrder;
    bool m_selected;
    bool m_displayable;
};