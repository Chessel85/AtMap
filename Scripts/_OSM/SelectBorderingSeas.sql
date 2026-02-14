--Retrieve names of bordering seas from passed in pkID 
--Also takes current x and y to return distance and bearing  to each result 
--Returns middle coordinate of seas so can jump location to them 
SELECT
    b.toSeaName,
    ST_Distance 
    (
    ST_Transform( MakePoint( ?, ?, 4326 ), 6933 ),
    ST_Transform( MakePoint( s.longitude, s.latitude, 4326 ), 6933 )
    ) / 1000.0, --from metres to kilometres
    ST_Azimuth
    (
        ST_Transform( MakePoint( ?, ?,4326), 6933 ),
        ST_Transform( MakePoint( s.longitude, s.latitude , 4326 ), 6933 )
    ) * 180.0 / PI(), --Radians to degrees 
    s.longitude,
    s.latitude 
  FROM
  spt_IHOSeas AS s 
  JOIN
  tbl_WaterBorders AS b  ON b.toPkID = s.pk_uid  
  WHERE
    b.fromPkID = ?;