CREATE TABLE spt_CoastalSimplePolygons  (
    pk_uid INTEGER PRIMARY KEY AUTOINCREMENT,
    fid INTEGER,
    area REAL
);
SELECT AddGeometryColumn( 'spt_CoastalSimplePolygons', 'lpolygon1', 4326, 'GEOMETRY', 'XY');
SELECT AddGeometryColumn( 'spt_CoastalSimplePolygons', 'lpolygon2', 4326, 'GEOMETRY', 'XY');
SELECT AddGeometryColumn( 'spt_CoastalSimplePolygons', 'lpolygon3', 4326, 'GEOMETRY', 'XY');
SELECT AddGeometryColumn( 'spt_CoastalSimplePolygons', 'lpolygon4', 4326, 'GEOMETRY', 'XY');

ATTACH DATABASE 'LandByCoastline-Complete.db' AS cdb;

INSERT INTO spt_CoastalSimplePolygons  ( fid, area, lpolygon1, lpolygon2, lpolygon3, lpolygon4 )
SELECT
 fid, area, lpolygon1, lpolygon2, lpolygon3, lpolygon4 
  FROM cdb.spt_CoastalSimplePolygons;

SELECT CreateSpatialIndex( 'spt_CoastalSimplePolygons', 'lpolygon1' );
SELECT CreateSpatialIndex( 'spt_CoastalSimplePolygons', 'lpolygon2' );
SELECT CreateSpatialIndex( 'spt_CoastalSimplePolygons', 'lpolygon3' );
SELECT CreateSpatialIndex( 'spt_CoastalSimplePolygons', 'lpolygon4' );
