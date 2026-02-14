--Find the largest polygon for a country and use this as the basis for the central location of the country 
UPDATE tbl_countries AS r
  SET
    midX = X(ST_Centroid(p.polygon1)),
    midY = Y(ST_Centroid(p.polygon1))
  FROM
    spt_NTWPolygons AS p
  INNER JOIN (
  -- This subquery finds the maximum area for each elementID
  SELECT
    elementID,
    MAX(area) AS max_area
    FROM spt_NTWPolygons
    GROUP BY elementID
    ) AS c
  ON p.elementID = c.elementID AND p.area = c.max_area
  WHERE r.relationID = p.elementID;