--Retrieve names of bordering polygons from a given polygon id 
--Also takes current x and y to return distance and bearing  to each result 
--Returns central coordinate of relations so can jump location to them 
SELECT DISTINCT 
    b.name2,
    p.area,
    p.labelX,
    p.labelY, 
    ST_Distance 
    (
    ST_Transform( MakePoint( :x, :y, 4326 ), 6933 ),
    ST_Transform( MakePoint( p.labelX, p.labelY, 4326 ), 6933 )
    ) / 1000.0 AS distance, --from metres to kilometres
    ST_Azimuth
    (
        ST_Transform( MakePoint( :x, :y,4326), 6933 ),
        ST_Transform( MakePoint( p.labelX, p.labelY, 4326 ), 6933 )
    ) * 180.0 / PI() AS bearing, --Radians to degrees 
    p.polygonId
  FROM
  tbl_Borders AS b 
  JOIN spt_polygons AS p  ON b.polygon2Id = p.polygonId 
  WHERE
    b.polygon1Id = :polygonId 
  ORDER BY bearing;