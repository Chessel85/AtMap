--Insert squares into land database to help understand distortions from transformations
--One at origin 0 0 
INSERT INTO spt_NTWPolygons  ( 
  elementType,
  elementID, 
  area,
  polygon1,
  polygon2,
  polygon3,
  polygon4
) 
VALUES ( 3, 62149, 10000, 
  ST_GeomFromText( 'POLYGON((-0.5 -0.5,0.5 -0.5,0.5 0.5,-0.5 0.5,-0.5 -0.5))', 4326 ), 
  ST_GeomFromText( 'POLYGON((-0.5 -0.5,0.5 -0.5,0.5 0.5,-0.5 0.5,-0.5 -0.5))', 4326 ),
  ST_GeomFromText( 'POLYGON((-0.5 -0.5,0.5 -0.5,0.5 0.5,-0.5 0.5,-0.5 -0.5))', 4326 ), 
  ST_GeomFromText( 'POLYGON((-0.5 -0.5,0.5 -0.5,0.5 0.5,-0.5 0.5,-0.5 -0.5))', 4326 ) 
);

--One at 70 degrees north
INSERT INTO spt_NTWPolygons  ( 
  elementType,
  elementID, 
  area,
  polygon1,
  polygon2,
  polygon3,
  polygon4
) 
VALUES ( 3, 62149, 10000, 
  ST_GeomFromText( 'POLYGON((-0.5 69.5,0.5 69.5,0.5 70.5,-0.5 70.5,-0.5 69.5))', 4326 ), 
  ST_GeomFromText( 'POLYGON((-0.5 69.5,0.5 69.5,0.5 70.5,-0.5 70.5,-0.5 69.5))', 4326 ), 
  ST_GeomFromText( 'POLYGON((-0.5 69.5,0.5 69.5,0.5 70.5,-0.5 70.5,-0.5 69.5))', 4326 ), 
  ST_GeomFromText( 'POLYGON((-0.5 69.5,0.5 69.5,0.5 70.5,-0.5 70.5,-0.5 69.5))', 4326 ) 
);

delete from spt_NTWPolygons where id=776856 OR id=776857; 