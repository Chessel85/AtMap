--update a layer selected state 
UPDATE tbl_Layers 
  SET selected = :state  
  WHERE layerId = :layerId;