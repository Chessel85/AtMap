--Retrieve names of points that are contained within given relation ID 
--Also takes current x and y to return distance and bearing  to each result 
SELECT
    COALESCE(t_name_en.value, t_name.value) AS pointName,
    ST_Distance 
    (
        MakePoint( ?, ?, 4326 ),
        p.point,
        0
    ) / 1000.0 AS distance, --from metres to kilometres
    ST_Azimuth
    (
        ST_Transform( MakePoint( ?, ?,4326), 6933 ),
        ST_Transform( p.point, 6933 )
    ) * 180.0 / PI() AS bearing, --Radians to degrees 
    p.nodeID,
    X(point),
    Y(point)
  FROM
    spt_Points AS p
  JOIN spt_NTWPolygons AS poly 
    JOIN tbl_Tags AS t_name ON p.nodeID = t_name.elementID AND t_name.key = 'name'
    LEFT JOIN tbl_Tags AS t_name_en ON p.nodeID = t_name_en.elementID AND t_name_en.key = 'name:en'
  WHERE 
    poly.elementID = ? 
    AND p.ROWID IN (
        SELECT ROWID
        FROM SpatialIndex
        WHERE
            f_table_name = 'spt_Points'
            AND f_geometry_column = 'point'
            AND search_frame = poly.polygon1 
    )
    AND ST_Contains( poly.polygon1, p.point ) 
    ORDER BY distance;
