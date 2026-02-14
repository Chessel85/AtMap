-- Get polygons matching admin level 2 in the given bounding box -- Along with the colour index so know what colour to draw it 
--Clip the polygons  to limit data points returned to application 
-- Include bounding box for relation so know if can draw the label 
SELECT
    AsText(
        ST_INTERSECTION (
            BuildMBR ( ?, ?, ?, ?, 4326 ),
            p.polygon
        )
    ) AS clippedPolygonWKT,
    COALESCE(t_name_en.value, t_name.value) AS countryName,
    r.colourIndex,
    r.minX, r.minY, r.maxX, r.maxY,
    r.labelX, r.labelY
  FROM
    spt_NTWPolygons AS p
  JOIN
    tbl_Tags AS t_admin ON p.elementID = t_admin.elementID
  JOIN
    tbl_Relations AS r ON r.RelationID = p.elementID 
  LEFT JOIN
    tbl_Tags AS t_name ON p.elementID = t_name.elementID AND t_name.key = 'name'
  LEFT JOIN
    tbl_Tags AS t_name_en ON p.elementID = t_name_en.elementID AND t_name_en.key = 'name:en'
  WHERE
    t_admin.key = 'admin_level' AND t_admin.value = '2'
    AND
    -- Use the spatial index for initial filter based on the bounding box
    p.ROWID IN (
        SELECT ROWID
        FROM SpatialIndex
        WHERE
            f_table_name = 'spt_NTWPolygons'
--            AND f_geometry_column = 'polygon'
            AND search_frame =
            BuildMBR ( ?, ?, ?, ?, 4326 )
    );