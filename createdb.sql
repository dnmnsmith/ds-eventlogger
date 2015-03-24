CREATE TABLE locations( location_id INTEGER PRIMARY KEY NOT NULL, location_name TEXT NOT NULL );

CREATE TABLE readings( 
timestamp DATETIME, 
location_id INTEGER NOT NULL REFERENCES locations( location_id ), 
desc TEXT NOT NULL,
value REAL NOT NULL );

CREATE TABLE sensors(
sensor_id TEXT PRIMARY KEY NOT NULL,
location_id INTEGER NOT NULL REFERENCES locations( location_id ) );
