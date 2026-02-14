SELECT
    Count(
        ST_Intersection(
            BuildMBR(-14.7871 , 44.3521 , 11.7871 , 59.1908, 4326),
            p.polygon4
        )
    ),
    -- Names are pulled from the SOVEREIGN Parent (Layer 1)
    COALESCE(MAX(t_name_en.value), MAX(t_name.value)),
    r.colourIndex,
    r.minX, r.minY, r.maxX, r.maxY,
    r.labelX, r.labelY
  FROM 
    spt_polygons AS p
    -- 1. Ensure this is a Level 6 Polygon (Land-only)
    INNER JOIN tbl_layerPolygons AS lp_child ON lp_child.polygonId = p.polygonId AND lp_child.layerId = 4
    
    -- 2. Link this geometry to its Sovereign Parent (Layer 1)
    INNER JOIN tbl_links AS l ON p.polygonId = l.geomId2 AND l.linkType = 1
    INNER JOIN tbl_layerPolygons AS lp_parent ON l.geomId1 = lp_parent.polygonId AND lp_parent.layerId = 1
    
    -- 3. Get the Relation ID of the Sovereign Nation
    INNER JOIN tbl_polygonOwners AS po_parent ON lp_parent.polygonId = po_parent.polygonId
    
    -- 4. Get Sovereign Metadata and Names
    INNER JOIN tbl_countries AS r ON r.RelationID = po_parent.elementID
    LEFT JOIN tbl_Tags AS t_name ON po_parent.elementID = t_name.elementID AND t_name.key = 'name'
    LEFT JOIN tbl_Tags AS t_name_en ON po_parent.elementID = t_name_en.elementID AND t_name_en.key = 'name:en'

  WHERE
    p.polygon4 IS NOT NULL
    AND 4 <= p.displayMin 
    -- Spatial Index Filter
    AND p.ROWID IN (
        SELECT ROWID FROM SpatialIndex 
        WHERE f_table_name = 'spt_polygons' AND f_geometry_column = 'polygon4'
        AND search_frame = BuildMBR( -14.7871 , 44.3521 , 11.7871 , 59.1908,4326)
    )
  GROUP BY
    p.polygonId,
    r.minX, r.minY, r.maxX, r.maxY;