--Attach the coastline database 
ATTACH DATABASE 'LandByCoastline-Complete.db' AS cdb;

--Create the non territorial water polygon  tables
--Using one table with four columns for now though may need to move to one table per resolution level to cope with null geometries easier 
CREATE TABLE spt_NTWPolygons (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
    borderPolygonID INTEGER,
  elementType INTEGER,
  elementID INTEGER,
  area REAL
);

--Columns for polygon geometries 
SELECT AddGeometryColumn ( 'spt_NTWPolygons', 'polygon1', 4326, 'GEOMETRY', 'XY');
SELECT AddGeometryColumn ( 'spt_NTWPolygons', 'polygon2', 4326, 'GEOMETRY', 'XY');
SELECT AddGeometryColumn ( 'spt_NTWPolygons', 'polygon3', 4326, 'GEOMETRY', 'XY');
SELECT AddGeometryColumn ( 'spt_NTWPolygons', 'polygon4', 4326, 'GEOMETRY', 'XY');

--Run a query to populate non-territorial water  polygons table with intersections of spt_Polygons and coastline data
--double spatial index
--May want to put timer on with .timer on 
--copy start
INSERT INTO spt_NTWPolygons ( borderPolygonID, elementType, elementID, polygon1, polygon2, polygon3, polygon4   )
SELECT
    p.id, 
    p.elementType,
    p.elementID,
    ST_Intersection( p.polygon1, c.lpolygon1 ),
    ST_Intersection( p.polygon2, c.lpolygon2 ) ,
    ST_Intersection( p.polygon3, c.lpolygon3 ) ,
    ST_Intersection( p.polygon4, c.lpolygon4 )  
  FROM
    spt_SimplePolygons AS p
INNER JOIN
    cdb.spt_CoastalSimplePolygons AS c
ON
    p.ROWID IN (
        SELECT ROWID FROM SpatialIndex
            WHERE f_table_name = 'spt_SimplePolygons'
            AND f_geometry_column  = 'polygon1' 
            AND search_frame = ST_Envelope( c.lpolygon1 )
    )
    AND c.ROWID IN (
        SELECT ROWID FROM SpatialIndex
            WHERE f_table_name = 'DB=cdb.spt_CoastalSimplePolygons' 
            AND f_geometry_column  = 'lpolygon1' 
            AND search_frame = ST_Envelope(p.polygon1 )
    )
    AND ST_Intersects( c.lpolygon1, p.polygon1 );

--Copy end 

--Count number of each type of geometry 
SELECT
    GeometryType(polygon) AS gt, 
    COUNT(polygon) AS number_of_geometries
  FROM
    spt_NTWPolygons
  GROUP BY
    gt;



--Get rid of any geometries or null rows 
--Check validity of geometries (see Validating polygons text file.
--Set to null any that aren't wanted 
DELETE FROM spt_NTWPolygons WHERE
  ( GeometryType( polygon ) != 'POLYGON' AND GeometryType( polygon ) != 'MULTIPOLYGON' )
     OR polygon IS NULL;

//And must recreate the spatial index 
select CreateSpatialIndex( 'spt_NTWPolygons', 'polygon1' );
select CreateSpatialIndex( 'spt_NTWPolygons', 'polygon2' );
select CreateSpatialIndex( 'spt_NTWPolygons', 'polygon3' );
select CreateSpatialIndex( 'spt_NTWPolygons', 'polygon4' );


--Can detach with
DETACH DATABASE cdb;

