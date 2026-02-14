--Return 1 if given point is in a land polygon or 0 otherwise 
SELECT   CASE WHEN EXISTS (
  SELECT 1 
  FROM spt_polygons  AS l 
  LEFT JOIN tbl_layerPolygons AS lp ON  lp.polygonId = l.polygonId 
  WHERE  
    ( lp.layerId = 1  OR lp.layerId = 4 )
    AND l.ROWID IN (
    SELECT ROWID FROM SpatialIndex WHERE (
      f_table_name = 'spt_polygons' 
      AND f_geometry_column = 'polygon1'
      AND  search_frame = MakePoint( ?,? )
    ) 
  )
 AND ST_Contains( l.polygon1 , MakePoint(  ?,? ))
  )
  THEN 1 ELSE 0 END AS PointInside;
