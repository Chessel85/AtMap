--Get polygons in the given bounding box 
--Clip the polygons  to limit data points returned to application 
SELECT
    -- Get the WKT of the (potentially clipped) merged polygon
    AsText(
        ST_INTERSECTION (
            BuildMBR ( ?, ?, ?, ?, 4326 ),
            p.lpolygon
        )
    ) AS clipped_polygon_wkt
  FROM
    spt_SimplePolygons AS p
  WHERE
    -- Use the spatial index for initial filter based on the bounding box
    p.ROWID IN (
        SELECT ROWID
        FROM SpatialIndex
        WHERE
            f_table_name = 'spt_SimplePolygons'
            AND f_geometry_column = 'lpolygon'
            AND search_frame =
            BuildMbr( ?, ?, ?, ?, 4326 )
    );
