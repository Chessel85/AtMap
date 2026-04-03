//mapViewType.h
#pragma once
//Struct for holding layer data as a data transfer object
struct LayerData
{
    int groupId;
    QString groupName;
    int groupSelected;
    int isBaseGroup;
    int layerId;
    QString layerName;
    bool isSelected;
    int displayOrder;
    bool display;

//Comparison operator 
bool operator==(const LayerData& other) const 
{
    return layerId == other.layerId;
}
};