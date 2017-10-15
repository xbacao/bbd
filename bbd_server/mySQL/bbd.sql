GRANT USAGE ON *.* TO 'bbduser'@'localhost';
DROP USER 'bbduser'@'localhost';
CREATE USER 'bbduser'@'localhost' IDENTIFIED BY 'morcao123';

DROP SCHEMA IF EXISTS bbd;
CREATE SCHEMA bbd;

USE bbd ;

CREATE TABLE IF NOT EXISTS bbd.arduino (
  arduinoID INT NOT NULL,
  description VARCHAR(100),
  PRIMARY KEY(arduinoID)
);

CREATE TABLE IF NOT EXISTS bbd.valve (
  valveID INT NOT NULL,
  description VARCHAR(100),
  arduinoID_f INT NOT NULL,
  PRIMARY KEY (valveID)
);
ALTER TABLE bbd.valve ADD FOREIGN KEY fk_arduino(arduinoID_f) REFERENCES bbd.arduino(arduinoID)
ON DELETE CASCADE;

CREATE TABLE IF NOT EXISTS bbd.schedule (
  scheduleID INT NOT NULL AUTO_INCREMENT,
  valveID_f INT NOT NULL,
  description VARCHAR(100),
  created_on TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  start_time TIME NOT NULL,
  stop_time TIME NOT NULL,
  active BOOLEAN NOT NULL DEFAULT FALSE,
  active_start TIMESTAMP NULL DEFAULT NULL,
  active_end TIMESTAMP NULL DEFAULT NULL,
  PRIMARY KEY (scheduleID)
);
ALTER TABLE bbd.schedule ADD FOREIGN KEY fk_valve(valveID_f) REFERENCES bbd.valve(valveID)
ON DELETE CASCADE;

DROP PROCEDURE IF EXISTS get_unsent_schedule;
DELIMITER $$
CREATE PROCEDURE get_unsent_schedule(IN arduino_ID INT)
BEGIN
SELECT valveID, scheduleID, start_time, stop_time
FROM bbd.schedule_entry INNER JOIN (
  SELECT scheduleID, valveID_f as valveID
  FROM bbd.schedule INNER JOIN(
    SELECT MAX(scheduleID) as max_sche_id
    FROM bbd.schedule INNER JOIN(
      SELECT *
      FROM bbd.valve
      WHERE arduinoID_f=arduino_ID
    ) AS T1 ON valveID_f=valveID
    GROUP BY valveID
  ) AS T2 ON scheduleID=max_sche_id
  WHERE scheduleID=max_sche_id and active_start IS NULL) AS T3 ON scheduleID_f=scheduleID;
END $$
DELIMITER ;

DROP PROCEDURE IF EXISTS get_last_schedule;
DELIMITER $$
CREATE PROCEDURE get_last_schedule(IN arduino_ID INT)
BEGIN
  SELECT valveID, scheduleID, start_time, stop_time
  FROM bbd.schedule_entry INNER JOIN (
    SELECT scheduleID, valveID_f as valveID
    FROM bbd.schedule INNER JOIN(
      SELECT MAX(scheduleID) as max_sche_id
      FROM bbd.schedule INNER JOIN(
        SELECT *
        FROM bbd.valve
        WHERE arduinoID_f=arduino_ID
      ) AS T1 ON valveID_f=valveID
      GROUP BY valveID
    ) AS T2 ON scheduleID=max_sche_id
    WHERE scheduleID=max_sche_id) AS T3 ON scheduleID_f=scheduleID;
END $$
DELIMITER ;

DROP PROCEDURE IF EXISTS set_schedule_active;
DELIMITER $$
CREATE PROCEDURE set_schedule_active(IN scheID INT)
BEGIN
  UPDATE bbd.schedule SET active=0,active_end=CURRENT_TIMESTAMP WHERE active=1;
  UPDATE bbd.schedule SET active=1,active_start=CURRENT_TIMESTAMP WHERE scheduleID=scheID;
END $$
DELIMITER ;

DROP PROCEDURE IF EXISTS new_valve_schedule;
DELIMITER $$
CREATE PROCEDURE new_valve_schedule(IN scheID INT, IN descr VARCHAR(100))
BEGIN
  INSERT INTO bbd.schedule(valveID_f, description) VALUES (scheID, descr);
  SELECT LAST_INSERT_ID() as scheduleID;
END $$
DELIMITER ;

DROP PROCEDURE IF EXISTS new_schedule_entry;
DELIMITER $$
CREATE PROCEDURE new_schedule_entry(IN scheID INT, IN start_time TIME, IN stop_time TIME)
BEGIN
  INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES (scheID, start_time, stop_time);
END $$
DELIMITER ;

GRANT SELECT,INSERT,UPDATE,DELETE,CREATE,ALTER,INDEX ON bbd.* TO 'bbduser'@'localhost';

GRANT EXECUTE ON PROCEDURE bbd.get_unsent_schedule TO 'bbduser'@'localhost';
GRANT EXECUTE ON PROCEDURE bbd.get_last_schedule TO 'bbduser'@'localhost';
GRANT EXECUTE ON PROCEDURE bbd.set_schedule_active TO 'bbduser'@'localhost';
GRANT EXECUTE ON PROCEDURE bbd.new_valve_schedule TO 'bbduser'@'localhost';
GRANT EXECUTE ON PROCEDURE bbd.new_schedule_entry TO 'bbduser'@'localhost';

/*TEST PROC*/
INSERT INTO bbd.arduino VALUES(1, "asdf");
INSERT INTO bbd.valve VALUES(1,"asdf",1);
INSERT INTO bbd.valve VALUES(2,"asdf",1);
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
-- CALL set_schedule_active(1);
-- CALL set_schedule_active(2);
/*
CALL get_valve_schedule(1);

CALL set_schedule_sent(1);


CALL new_valve_schedule(1, "WTFFF");

CALL new_schedule_entry(2, CURTIME(), CURTIME());
CALL new_schedule_entry(2, CURTIME(), CURTIME());

*/
