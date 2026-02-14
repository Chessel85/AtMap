-- Get cities in the given bounding box
-- Always return city if it is a capital city
-- Include city if the population is greater or equal to given value 
SELECT
    COALESCE(t_name_en.value, t_name.value) AS pointName,
    p.nodeID,
    X(point),
    Y(point)
  FROM
    spt_Points AS p
    JOIN tbl_Tags AS t_name ON p.nodeID = t_name.elementID AND t_name.key = 'name'
    LEFT JOIN tbl_Tags AS t_name_en ON p.nodeID = t_name_en.elementID AND t_name_en.key = 'name:en'
    LEFT JOIN tbl_Tags AS t_capital ON p.nodeID = t_capital.elementID AND t_capital.key = 'capital'
    LEFT JOIN tbl_Tags AS t_population ON p.nodeID = t_population.elementID AND t_population.key = 'population'
  WHERE 
    p.ROWID IN (
        SELECT ROWID
        FROM SpatialIndex
        WHERE
            f_table_name = 'spt_Points'
            AND f_geometry_column = 'point'
            AND search_frame = BuildMBR( ?, ?, ?, ?, 4326)
    )
    AND (
        t_capital.value = 'yes'
        OR COALESCE(CAST(t_population.value AS INTEGER), 0) >= ? 
    )
    AND t_name.elementID IN (
        SELECT elementID
            FROM tbl_Tags
            WHERE key = 'place' AND value = 'city'
    );