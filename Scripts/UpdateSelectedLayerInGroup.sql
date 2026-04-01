--Set all the layers in the :layer group to 0 except for :layer itself 
UPDATE tbl_layers
  SET selected = (layerId = :layerId)
  WHERE layerGroupId = (
    SELECT layerGroupId 
    FROM tbl_layers 
    WHERE layerId = :layerId
);