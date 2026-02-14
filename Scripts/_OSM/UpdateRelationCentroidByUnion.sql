--Aggregate the polygons for each relation
--Calculate the centroid for each 
UPDATE tbl_Relations AS r
  SET
    midX = c.midX,
    midY = c.midY
  FROM 
    (
      -- This subquery calculates the centroid for each relationID
      SELECT
        p.elementID AS relationID,
        X(ST_Centroid( ST_Union( p.polygon1 ))) AS midX,
        Y(ST_Centroid( ST_Union( p.polygon1))) AS midY
    FROM spt_NTWPolygons AS p
    GROUP BY p.elementID
  ) AS c
    WHERE r.relationID = c.relationID;

