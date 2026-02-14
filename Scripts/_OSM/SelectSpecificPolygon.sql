--Get a specific polygon so can do matching point analysis SELECT
SELECT
    AsText( p.polygon1 )
  FROM
    spt_NTWPolygons AS p
  WHERE
    p.id = ?;
