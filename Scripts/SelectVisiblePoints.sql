-- Get points in the given bounding box
SELECT
    pt.name,
    X( pt.pt ),
    Y( pt.pt )
  FROM
    spt_Points AS pt
    JOIN tbl_layers AS l ON l.layerId = pt.layerId 
  WHERE 
    l.selected = 1  AND
    ? > pt.minZoom  AND 
    pt.ROWID IN (
        SELECT ROWID
        FROM SpatialIndex
        WHERE
            f_table_name = 'spt_points'
            AND f_geometry_column = 'pt'
            AND search_frame = BuildMBR( ?, ?, ?, ?, 4326)
    );