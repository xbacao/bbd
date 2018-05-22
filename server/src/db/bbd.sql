SET TIME ZONE 'Europe/Lisbon';

CREATE SCHEMA bbd;

CREATE TABLE bbd.device (
  deviceID INT NOT NULL,
  description VARCHAR(100),
  PRIMARY KEY(deviceID)
);

CREATE TABLE bbd.valve (
  valveID INT NOT NULL,
  description VARCHAR(100),
  deviceID_f INT NOT NULL,
  PRIMARY KEY (valveID)
);
ALTER TABLE bbd.valve ADD CONSTRAINT fk_device FOREIGN KEY (deviceID_f) REFERENCES bbd.device(deviceID)
ON DELETE CASCADE;

CREATE TABLE bbd.schedule (
  scheduleID SERIAL NOT NULL,
  valveID_f INT NOT NULL,
  description VARCHAR(100),
  created_on TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  start_time SMALLINT NOT NULL CHECK(start_time BETWEEN 0 AND 1440),
  stop_time SMALLINT NOT NULL CHECK(start_time BETWEEN 0 AND 1440),
  sent BOOLEAN NOT NULL DEFAULT FALSE,
  sent_on TIMESTAMP NULL,
  active BOOLEAN NOT NULL DEFAULT TRUE,
  active_start TIMESTAMP NULL DEFAULT NULL,
  active_end TIMESTAMP NULL DEFAULT NULL,
  PRIMARY KEY (scheduleID)
);
ALTER TABLE bbd.schedule ADD CONSTRAINT fk_valve FOREIGN KEY (valveID_f) REFERENCES bbd.valve(valveID)
ON DELETE CASCADE;

----------------------receiver----------------------

CREATE FUNCTION bbd.rcv_get_device_valves(IN device_id INT)
RETURNS TABLE(
  valveID_o INT
) AS $$
BEGIN
  RETURN QUERY
  SELECT valveID
  FROM bbd.valve
  WHERE deviceID_f = device_id;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION bbd.rcv_get_active_schedules(IN device_id INT)
RETURNS TABLE(
  scheduleID_o INT,
  valveID_o INT,
  start_time_o SMALLINT,
  stop_time_o SMALLINT,
  sent_o BOOLEAN
) AS $$
BEGIN
  RETURN QUERY
  SELECT scheduleID, valveID, start_time, stop_time, sent
  FROM bbd.schedule INNER JOIN (
    SELECT valveID
    FROM bbd.valve
    WHERE deviceID_f = device_id
  ) AS T1 ON valveID_f = valveID
  WHERE ACTIVE = TRUE;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION bbd.rcv_set_schedule_sent(IN schedule_id INT)
RETURNS VOID AS $$
BEGIN
  UPDATE bbd.schedule
  SET sent=TRUE, sent_on=CURRENT_TIMESTAMP
  WHERE scheduleID=schedule_id and sent=FALSE;
END
$$ LANGUAGE plpgsql;

INSERT INTO bbd.device(deviceID,description) VALUES(1,'device_teste_1');
INSERT INTO bbd.device(deviceID,description) VALUES(2,'device_teste_2');
INSERT INTO bbd.valve(valveID, description, deviceID_f)VALUES(1,'test_valve1',1),(2,'test_valve2',1),(3,'test_valve3',2);
----------------------web----------------------
CREATE FUNCTION bbd.web_get_devices()
RETURNS TABLE(
  id INT,
  description VARCHAR(100),
  n_valves BIGINT
) AS $$
BEGIN
  RETURN QUERY
  SELECT deviceID as id, device.description, count(valveID)
  FROM bbd.device LEFT OUTER JOIN bbd.valve ON device.deviceID=valve.deviceID_f
  GROUP BY deviceID
  ORDER BY deviceID ASC;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION bbd.web_get_valves()
RETURNS TABLE(
  id INT,
  description VARCHAR(100),
  deviceid INT
) AS $$
BEGIN
  RETURN QUERY
  SELECT *
  FROM bbd.valve
  ORDER BY id ASC;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION bbd.web_get_valves(IN device_id INT)
RETURNS TABLE(
  id INT,
  description VARCHAR(100),
  deviceid INT
) AS $$
BEGIN
  RETURN QUERY
  SELECT *
  FROM bbd.valve
  WHERE deviceID_f=device_id
  ORDER BY id ASC;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION bbd.web_get_schedules()
RETURNS TABLE(
  scheduleID_o INT,
  valveID_f_o INT,
  description_o VARCHAR(100),
  created_on_o TIMESTAMP,
  start_time_o SMALLINT,
  stop_time_o SMALLINT,
  sent_o BOOLEAN,
  sent_on_o TIMESTAMP,
  active_o BOOLEAN,
  active_start_o TIMESTAMP,
  active_end_o TIMESTAMP
) AS $$
BEGIN
  RETURN QUERY
  SELECT *
  FROM bbd.schedule
  ORDER BY scheduleID DESC;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION bbd.web_get_schedules(IN valveid INT)
RETURNS TABLE(
  scheduleID_o INT,
  valveID_f_o INT,
  description_o VARCHAR(100),
  created_on_o TIMESTAMP,
  start_time_o SMALLINT,
  stop_time_o SMALLINT,
  sent_o BOOLEAN,
  sent_on_o TIMESTAMP,
  active_o BOOLEAN,
  active_start_o TIMESTAMP,
  active_end_o TIMESTAMP
) AS $$
BEGIN
  RETURN QUERY
  SELECT *
  FROM bbd.schedule
  WHERE valveID_f=valveid
  ORDER BY scheduleID DESC;
END
$$ LANGUAGE plpgsql;
