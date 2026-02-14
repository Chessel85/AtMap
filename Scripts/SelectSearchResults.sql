--select values matching search text 
SELECT name, labelX, labelY
  FROM (
    -- First, get matches from polygons
    SELECT 
        p.name, 
        p.labelX, 
        p.labelY 
    FROM spt_polygons AS p
    JOIN tbl_layers AS l ON l.layerId = p.layerId
    WHERE l.selected = 1 AND p.name LIKE :searchTerm || '%'

    UNION ALL

    -- Second, get matches from points
    SELECT 
        pt.name, 
        pt.longitude AS labelX, 
        pt.latitude AS labelY  
    FROM spt_points AS pt
    JOIN tbl_layers AS l ON l.layerId = pt.layerId
    WHERE l.selected = 1 AND pt.name LIKE :searchTerm || '%'
) 
ORDER BY name 
LIMIT 30;