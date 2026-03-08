--Select polygon matching coordinates 
SELECT
    p.name,
  AsText( p.polygon ),
    MbrMinX( polygon ),    
    MbrMinY( polygon ),
    MbrMaxX( polygon ),
    MbrMaxY( polygon )
  FROM
    spt_Polygons AS p
    JOIN tbl_layers AS l ON l.layerId = p.layerId 
  WHERE
    l.selected = 1  
    AND p.polygon IS NOT NULL 
    AND p.ROWID IN (
        SELECT ROWID FROM SpatialIndex WHERE
                f_table_name = 'spt_polygons' 
                AND f_geometry_column = 'polygon'
                AND search_frame = MakePoint( :x, :y )
    )
        AND ST_Contains( p.polygon, MakePoint(  :x, :y ))
 ORDER BY l.displayOrder DESC, p.area   ASC;