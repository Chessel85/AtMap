--Select name of polygon matching coordinates preferring the name:en over just name key 
--Get relation also as will be used for getting info about it 
SELECT
    COALESCE(t_en.value, t_name.value, 'Unknown') AS value,
    r.areaCalculated AS area,
    po.elementID AS relationID
  FROM
    spt_Polygons AS p
  LEFT JOIN tbl_polygonOwners AS po ON p.polygonId = po.polygonId 
  LEFT JOIN tbl_layerPolygons AS lp ON p.polygonId = lp.polygonId 
  LEFT JOIN tbl_countries AS r ON r.relationId = po.elementId   
  LEFT JOIN
    tbl_Tags AS t_en ON po.elementID = t_en.elementID AND t_en.key = 'name:en'
  LEFT JOIN
    tbl_Tags AS t_name ON po.elementID = t_name.elementID AND t_name.key = 'name'
  WHERE
    ( lp.layerId = 1 OR lp.layerId = 4  ) AND 
    p.polygonId IN (
        SELECT p2.polygonId
        FROM spt_polygons AS p2
        WHERE ROWID IN (
            SELECT ROWID
            FROM SpatialIndex
            WHERE
                f_table_name = 'spt_polygons' 
                AND f_geometry_column = 'polygon1'
                AND search_frame = MakePoint(?, ? )
        )
        AND ST_Contains( p2.polygon1, MakePoint( ?, ? ))
    )
 ORDER BY area   ASC;