INSERT INTO sensors(sensor_id,location_id) VALUES ( 'b8:27:eb:be:d2:1f', (SELECT location_id FROM locations WHERE location_name = 'WebPi'));
INSERT INTO sensors(sensor_id,location_id) VALUES ( 'b8:27:eb:34:45:08', (SELECT location_id FROM locations WHERE location_name = 'HiFiPi'));
INSERT INTO sensors(sensor_id,location_id) VALUES ( 'b8:27:eb:2d:53:ed', (SELECT location_id FROM locations WHERE location_name = 'rx433Pi'));
INSERT INTO sensors(sensor_id,location_id) VALUES ( '497', (SELECT location_id FROM locations WHERE location_name = 'Kitchen'));
INSERT INTO sensors(sensor_id,location_id) VALUES ( '43A', (SELECT location_id FROM locations WHERE location_name = 'Study'));
INSERT INTO sensors(sensor_id,location_id) VALUES ( '485', (SELECT location_id FROM locations WHERE location_name = 'Freezer'));
INSERT INTO sensors(sensor_id,location_id) VALUES ( '479', (SELECT location_id FROM locations WHERE location_name = 'Side_Gate'));
