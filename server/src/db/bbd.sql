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

CREATE FUNCTION get_device_valves(IN device_id INT)
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

CREATE FUNCTION get_active_schedules(IN device_id INT)
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
  ) AS T1 on valveID_f = valveID
  WHERE ACTIVE = TRUE;
END
$$ LANGUAGE plpgsql;

CREATE FUNCTION set_schedule_sent(IN schedule_id INT)
RETURNS VOID AS $$
BEGIN
  UPDATE bbd.schedule
  SET sent=TRUE, sent_on=CURRENT_TIMESTAMP
  WHERE scheduleID=schedule_id and sent=FALSE;
END
$$ LANGUAGE plpgsql;

INSERT INTO bbd.device(deviceID,description) VALUES(1,'asdf');
INSERT INTO bbd.valve(valveID, description, deviceID_f)VALUES(1,'test_valve1',1),(2,'test_valve2',1);
