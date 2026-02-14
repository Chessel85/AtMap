-- Get points in the given bounding box 
SELECT
    COALESCE(t_name_en.value, t_name.value) AS pointName,
    X( point ),
    Y( point )
  FROM
    spt_Points AS p
  JOIN
    tbl_Tags AS t_name ON p.nodeID = t_name.elementID AND t_name.key = 'name'
  LEFT JOIN
    tbl_Tags AS t_name_en ON p.nodeID = t_name_en.elementID AND t_name_en.key = 'name:en'
  WHERE
    -- Make sure the spatial index is built to use this query 
    p.ROWID IN (
        SELECT ROWID
        FROM SpatialIndex
        WHERE
            f_table_name = 'spt_Points'
            AND f_geometry_column = 'point'
            AND search_frame = BuildMBR( ?, ?, ?, ?, 4326 )
    );
