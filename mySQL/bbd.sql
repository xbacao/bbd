CREATE SCHEMA IF NOT EXISTS bbd;
USE bbd ;

DROP TABLE IF EXISTS bbd.valve;
CREATE TABLE IF NOT EXISTS bbd.valve (
  valveID INT NOT NULL,
  description VARCHAR(100),
  PRIMARY KEY (valveID)
)

DROP TABLE IF EXISTS bbd.schedule;
CREATE TABLE IF NOT EXISTS bbd.schedule (
  scheduleID INT NOT NULL AUTO_INCREMENT,
  valveID INT NOT NULL,
  description VARCHAR(100),
  sent BIT NOT NULL DEFAULT 0,
  created_on TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, 
  PRIMARY KEY (scheduleID),
  CONSTRAINT fk_valve FOREIGN KEY valveID REFERENCES bbd.valve (valveID) ON DELETE CASCADE)
)

DROP TABLE IF EXISTS bbd.schedule_entry;
CREATE TABLE IF NOT EXISTS bbd.schedule_entry (
  entryID INT NOT NULL AUTO_INCREMENT,
  scheduleID INT NOT NULL,
  start_time TIME NOT NULL,
  stop_time TIME NOT NULL,
  PRIMARY KEY (entryID),
  CONSTRAINT fk_schedule FOREIGN KEY scheduleID REFERENCES bbd.schedule (scheduleID) ON DELETE CASCADE)
)

DELIMITER $$
DROP PROCEDURE IF EXISTS get_valve_schedule(IN valveID INT)
BEGIN
	SELECT @sche_id := MAX(scheduleID) FROM (SELECT * FROM bbd.schedule WHERE valveID=valveID) as T1;
  SELECT @sent := MAX(sent) FROM bbd.schedule WHERE scheduleID=@sche_id;
  IF  @sent = 0 THEN
    SELECT * FROM bbd.schedule_entry WHERE scheduleID=@sche_id;
  END IF;
	
END $$
DELIMITER ;




