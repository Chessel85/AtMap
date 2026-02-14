--Retrieve names of points that are contained within given polygon ID 
--Also takes current x and y to return distance and bearing  to each result 
SELECT
    c.name,
    c.featureClass,
    c.longitude,
    c.latitude,
    ST_Distance 
    (
        MakePoint( :x, :y, 4326 ),
        MakePoint( c.longitude, c.latitude, 4326 ),
        0
    ) / 1000.0 AS distance, --from metres to kilometres
    ST_Azimuth
    (
        ST_Transform( MakePoint( :x, :y,4326), 6933 ),
        ST_Transform( MakePoint( c.longitude, c.latitude, 4326 ), 6933 )
    ) * 180.0 / PI() AS bearing, --Radians to degrees 
    ( SELECT categoryId FROM ref_geometryCategories WHERE categoryName  = 'point' ),
    c.containedId
  FROM  tbl_contains AS c 
  WHERE 
    c.polygonId  = :polygonId  
    ORDER BY distance;
