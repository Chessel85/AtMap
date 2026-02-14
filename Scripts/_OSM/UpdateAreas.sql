--Calculate area from polygon  for all polygons 
--Convert to SRID 6933 global cylindrical projection to get answer in kilometres squared 
UPDATE spt_NTWpolygons
SET area = st_area( ST_Transform( polygon1 , 6933) )  / 1000000;

--And update the relations table summing all constituent polygons 
UPDATE tbl_countries AS r 
  SET 
  areaCalculated = 
  ( SELECT SUM( p.area ) 
            FROM spt_NTWPolygons AS p 
            WHERE p.elementID = r.relationID );
