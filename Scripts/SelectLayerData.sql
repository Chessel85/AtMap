--Retrieve the layers including their layer groups 
SELECT
    lg.layerGroupId,
    lg.layerGroupName,
    lg.selected,
    lg.isBase,
    l.layerId,
    l.layerName,
    gc.categoryName,
    l.selected,
    l.displayOrder,
    l.display
  FROM tbl_layerGroups AS lg
  JOIN tbl_layers AS l ON l.layerGroupID = lg.layerGroupId
  JOIN ref_geometryCategories AS gc ON gc.categoryId = l.geomCategoryId
  ORDER BY lg.layerGroupId ASC;    
