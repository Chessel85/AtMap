--Retrieve names of relations that border given relation ID 
--Also takes current x and y to return distance and bearing  to each result 
--Returns middle coordinate of relations so can jump location to them 
SELECT DISTINCT 
    toRelationName,
    ST_Distance 
    (
    ST_Transform( MakePoint( ?, ?, 4326 ), 6933 ),
    ST_Transform( MakePoint( r.midX, r.midY, 4326 ), 6933 )
    ) / 1000.0 AS distance, --from metres to kilometres
    ST_Azimuth
    (
        ST_Transform( MakePoint( ?, ?,4326), 6933 ),
        ST_Transform( MakePoint( r.midX, r.midY, 4326 ), 6933 )
    ) * 180.0 / PI() AS bearing, --Radians to degrees 
    r.midX,
    r.midY 
  FROM
  tbl_Borders AS b 
  JOIN
  tbl_countries AS r  ON b.toRelationID = r.relationID 
  WHERE
    b.fromRelationID = ?
  ORDER BY bearing;