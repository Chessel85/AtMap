-- Get polygons in the given bounding box 
-- Along with the colour index so know what colour to draw it 
--Clip the polygons  to limit data points returned to application 
-- Include bounding box for relation so know if can draw the label 
SELECT
    AsText(
        ST_INTERSECTION (
            BuildMBR ( ?, ?, ?, ?, 4326 ),
            p.polygon1
        )
    ),
    COALESCE(t_name_en.value, t_name.value) AS countryName,
    r.colourIndex,
    r.minX, r.minY, r.maxX, r.maxY,
    r.labelX, r.labelY
  FROM
    spt_polygons AS p
  LEFT JOIN tbl_polygonOwners AS po on po.polygonId = p.polygonId AND po.elementType = 3 
  LEFT JOIN tbl_layerPolygons AS lp ON  lp.polygonId = p.polygonId 
  LEFT JOIN
    tbl_Tags AS t_admin ON po.elementID = t_admin.elementID
  LEFT JOIN
    tbl_countries AS r ON r.RelationID = po.elementID  AND po.elementType = 3 
  LEFT JOIN
    tbl_Tags AS t_name ON po.elementID = t_name.elementID AND t_name.key = 'name'
  LEFT JOIN
    tbl_Tags AS t_name_en ON po.elementID = t_name_en.elementID AND t_name_en.key = 'name:en'
  WHERE
-- not filtering by admin level     ( t_admin.key = 'admin_level' AND t_admin.value = '2' ) AND 
    ( lp.layerId = 1 OR   lp.layerId = 4 ) 
    AND
    p.polygon1 IS NOT NULL 
    AND  :zoomBand <= p.displayLessThan 
    AND 
    -- Use the spatial index for initial filter based on the bounding box
    p.ROWID IN (
        SELECT ROWID
        FROM SpatialIndex
        WHERE
            f_table_name = 'spt_polygons'
            AND f_geometry_column = 'polygon1'
            AND search_frame =
            BuildMBR ( ?, ?, ?, ?, 4326 )
    )
    AND
    ( GeometryType( p.polygon1 ) = 'POLYGON' OR GeometryType( p.polygon1 ) = 'MULTIPOLYGON' );