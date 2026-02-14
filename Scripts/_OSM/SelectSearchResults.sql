--select values matching search text preferring key of name:en but using name if it is not available 
SELECT
    COALESCE( t_en.value, t.value ) AS returnName,
    ( MbrMaxX( p.polygon1 ) - MbrMinX( p.polygon1 ) ) / 2.0 + MbrMinX( p.polygon1 ),
    ( MbrMaxY( p.polygon1 ) - MbrMinY( p.polygon1 ) ) / 2.0 + MbrMinY( p.polygon1 )
  FROM tbl_tags AS t 
  LEFT JOIN tbl_Tags as t_en ON t.elementID = t_en.elementID AND t_en.key='name:en'
  LEFT JOIN tbl_polygonOwners AS po ON po.elementId = t.elementId AND po.elementType = t.elementType 
  LEFT JOIN spt_polygons AS p ON po.polygonId = p.polygonId 
  WHERE
    t.key='name'
    AND (
        ( t_en.value IS NOT NULL AND t_en.value LIKE ? || '%' ) -- Prefer name:en if it exists and matches
        OR
        ( t_en.value IS NULL AND t.value LIKE ? || '%' )       -- Otherwise, check name if name:en is null
    )
ORDER BY returnName
LIMIT 30;