-- Get polygons in the given bounding box 
-- Along with the colour index so know what colour to draw it 
--Clip the polygons  to limit data points returned to application 
-- Include bounding box for polygon so know if can draw the label 
SELECT
    AsText(
        ST_Intersection(
            BuildMBR(:west, :south, :east, :north, 4326),
            p.polygon
        )
    ),
    p.name,
    p.mapColour,
    MbrMinX( p.polygon ), 
    MbrMinY( p.polygon ), 
    MbrMaxX( p.polygon ), 
    MbrMaxY( p.polygon ), 
    p.labelX, 
    p.labelY
  FROM 
    spt_polygons AS p
    JOIN tbl_layers AS l ON l.layerId = p.layerId 
  WHERE 
    l.selected = 1 
    AND p.polygon IS NOT NULL
    -- Spatial Index Filter
    AND p.ROWID IN (
        SELECT ROWID FROM SpatialIndex 
        WHERE 
            f_table_name = 'spt_polygons' 
            AND f_geometry_column = 'polygon'
            AND search_frame = BuildMBR(:west, :south, :east, :north, 4326)
    )
  AND ST_Intersects( polygon, BuildMBR(:west, :south, :east, :north, 4326) );