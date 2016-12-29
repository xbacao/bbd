
DROP SCHEMA IF EXISTS bbd;
CREATE SCHEMA IF NOT EXISTS bbd;
USE bbd ;

CREATE TABLE IF NOT EXISTS bbd.valve (
  valveID INT NOT NULL,
  description VARCHAR(100),
  PRIMARY KEY (valveID)
);

CREATE TABLE IF NOT EXISTS bbd.schedule (
  scheduleID INT NOT NULL AUTO_INCREMENT,
  valveID_f INT NOT NULL,
  description VARCHAR(100),
  sent BIT(1) NOT NULL DEFAULT 0,
  created_on TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, 
  PRIMARY KEY (scheduleID)
);
ALTER TABLE bbd.schedule ADD FOREIGN KEY fk_valve(valveID_f) REFERENCES bbd.valve(valveID)
ON DELETE CASCADE;

CREATE TABLE IF NOT EXISTS bbd.schedule_entry (
  entryID INT NOT NULL AUTO_INCREMENT,
  scheduleID_f INT NOT NULL,
  start_time TIME NOT NULL,
  stop_time TIME NOT NULL,
  PRIMARY KEY (entryID)
);
ALTER TABLE bbd.schedule_entry ADD FOREIGN KEY fk_schedule(scheduleID_f) REFERENCES bbd.schedule (scheduleID)
ON DELETE CASCADE;

DELIMITER $$
DROP PROCEDURE IF EXISTS get_valve_schedule;
CREATE PROCEDURE get_valve_schedule(IN valveID INT)
BEGIN
  SELECT *
  FROM bbd.schedule_entry INNER JOIN (
    SELECT scheduleID
    FROM bbd.schedule INNER JOIN (
      SELECT @sche_id := MAX(scheduleID) 
      FROM bbd.schedule 
      WHERE valveID_f=valveID) AS T1 ON scheduleID=@sche_id
    WHERE sent=0) AS T2 ON scheduleID_f=scheduleID;
END $$
DELIMITER ;

/*
--TEST PROC--
INSERT INTO bbd.valve VALUES(1,"asdf");
INSERT INTO bbd.schedule(valveID_f, description) VALUES (1, "AAAAA");
INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(1, CURTIME(),CURTIME());
INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(1, CURTIME(),CURTIME());
INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(1, CURTIME(),CURTIME());
INSERT INTO bbd.schedule_entry(scheduleID_f, start_time, stop_time) VALUES(1, CURTIME(),CURTIME());

CALL get_valve_schedule(1);
*/


