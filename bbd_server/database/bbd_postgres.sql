SET TIME ZONE 'Europe/Lisbon';

CREATE SCHEMA bbd;

CREATE TABLE bbd.arduino (
  arduinoID INT NOT NULL,
  description VARCHAR(100),
  PRIMARY KEY(arduinoID)
);

CREATE TABLE bbd.valve (
  valveID INT NOT NULL,
  description VARCHAR(100),
  arduinoID_f INT NOT NULL,
  PRIMARY KEY (valveID)
);
ALTER TABLE bbd.valve ADD CONSTRAINT fk_arduino FOREIGN KEY (arduinoID_f) REFERENCES bbd.arduino(arduinoID)
ON DELETE CASCADE;

CREATE TABLE bbd.schedule (
  scheduleID SERIAL NOT NULL,
  valveID_f INT NOT NULL,
  description VARCHAR(100),
  created_on TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  start_time SMALLINT NOT NULL CHECK(start_time BETWEEN 0 AND 1440),
  stop_time SMALLINT NOT NULL CHECK(start_time BETWEEN 0 AND 1440),
  active BOOLEAN NOT NULL DEFAULT FALSE,
  deactivate_request BOOLEAN NOT NULL DEFAULT FALSE,
  active_start TIMESTAMP NULL DEFAULT NULL,
  active_end TIMESTAMP NULL DEFAULT NULL,
  PRIMARY KEY (scheduleID)
);
ALTER TABLE bbd.schedule ADD CONSTRAINT fk_valve FOREIGN KEY (valveID_f) REFERENCES bbd.valve(valveID)
ON DELETE CASCADE;

-- CREATE FUNCTION amount_of_active_schedules(IN arduino_ID INT) RETURNS INT AS $$
-- BEGIN
--   SELECT count(*) as n_active_schedules
--   FROM bbd.schedule INNER JOIN (
--     SELECT valveID
--     FROM bbd.valve
--     WHERE arduinoID_f = arduino_ID
--   ) AS T1 on valveID_f = valveID
--   WHERE ACTIVE = TRUE;
-- END
-- $$ LANGUAGE plpgsql;

CREATE FUNCTION get_active_schedules(IN arduino_ID INT)
RETURNS TABLE(
  scheduleID_o INT,
  valveID_o INT,
  start_time_o SMALLINT,
  stop_time_o SMALLINT
) AS $$
BEGIN
  RETURN QUERY
  SELECT scheduleID, valveID, start_time, stop_time
  FROM bbd.schedule INNER JOIN (
    SELECT valveID
    FROM bbd.valve
    WHERE arduinoID_f = arduino_ID
  ) AS T1 on valveID_f = valveID
  WHERE ACTIVE = TRUE;
END
$$ LANGUAGE plpgsql;

-- CREATE FUNCTION get_last_schedule(IN arduino_ID INT) RETURNS TABLE(valveID INT, scheduleID INT, start_time SMALLINT, stop_time SMALLINT) AS $$
-- BEGIN
--   SELECT valveID, scheduleID, start_time, stop_time
--   FROM bbd.schedule_entry INNER JOIN (
--     SELECT scheduleID, valveID_f as valveID
--     FROM bbd.schedule INNER JOIN(
--       SELECT MAX(scheduleID) as max_sche_id
--       FROM bbd.schedule INNER JOIN(
--         SELECT *
--         FROM bbd.valve
--         WHERE arduinoID_f=arduino_ID
--       ) AS T1 ON valveID_f=valveID
--       GROUP BY valveID
--     ) AS T2 ON scheduleID=max_sche_id
--     WHERE scheduleID=max_sche_id) AS T3 ON scheduleID_f=scheduleID;
-- END
-- $$ LANGUAGE plpgsql;
--
-- CREATE FUNCTION set_schedule_active(IN scheID INT) RETURNS VOID AS $$
-- BEGIN
--   UPDATE bbd.schedule SET active=0,active_end=CURRENT_TIMESTAMP WHERE active=1;
--   UPDATE bbd.schedule SET active=1,active_start=CURRENT_TIMESTAMP WHERE scheduleID=scheID;
-- END
-- $$ LANGUAGE plpgsql;


-- CREATE FUNCTION new_valve_schedule(IN scheID INT, IN descr VARCHAR(100)) RETURNS INT AS $$
-- BEGIN
--   INSERT INTO bbd.schedule(valveID_f, description) VALUES (scheID, descr) RETURNING scheduleID;
-- END
-- $$ LANGUAGE plpgsql;


-- CREATE FUNCTION new_schedule_entry(IN scheID INT, IN start_time TIME, IN stop_time TIME) RETURNS VOID AS $$
-- BEGIN
--   INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES (scheID, start_time, stop_time);
-- END
-- $$ LANGUAGE plpgsql;

INSERT INTO bbd.arduino(arduinoID,description) VALUES(1,'asdf');
INSERT INTO bbd.valve(valveID, description, arduinoID_f)VALUES(1,'asdf',1),(2,'asdf',1);

INSERT INTO bbd.schedule(valveID_f, start_time, stop_time, active, deactivate_request) VALUES
  (1, 60, 120, TRUE, FALSE);

-- INSERT INTO bbd.arduino VALUES(1, "asdf");
-- INSERT INTO bbd.valve VALUES(1,"asdf",1);
-- INSERT INTO bbd.valve VALUES(2,"asdf",1);
--
-- INSERT INTO bbd.schedule(valveID_f, start_time, stop_time, active, deactivate_request) VALUES
--   (1, 60, 120, TRUE, FALSE);
--
-- INSERT INTO bbd.schedule(valveID_f, description) VALUES (1, "AAAAA");
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(1, CURTIME(),CURTIME());
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(1, CURTIME(),CURTIME());
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(1, CURTIME(),CURTIME());
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(1, CURTIME(),CURTIME());
--
-- INSERT INTO bbd.schedule(valveID_f, description) VALUES (1, "TEST SENT");
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(2, CURTIME(),CURTIME());
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(2, CURTIME(),CURTIME());
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(2, CURTIME(),CURTIME());
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(2, CURTIME(),CURTIME());
--
-- INSERT INTO bbd.schedule(valveID_f, description) VALUES (2, "TEST SENT");
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(3, CURTIME(),CURTIME());
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(3, CURTIME(),CURTIME());
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(3, CURTIME(),CURTIME());
-- INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(3, CURTIME(),CURTIME());
-- SELECT set_schedule_active(1);
-- SELECT set_schedule_active(2);
--
-- SELECT get_valve_schedule(1);
--
-- SELECT set_schedule_sent(1);
--
--
-- SELECT new_valve_schedule(1, "WTFFF");
--
-- SELECT new_schedule_entry(2, CURTIME(), CURTIME());
-- SELECT new_schedule_entry(2, CURTIME(), CURTIME());
