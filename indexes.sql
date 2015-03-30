CREATE INDEX location_index ON readings(location_id);
CREATE INDEX time_index ON readings(timestamp);
CREATE INDEX loc_meas_index ON readings(location_id,desc);
