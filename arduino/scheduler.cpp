#include <scheduler.h>
#include <arduino_cfg.h>
#include <Arduino.h> //erase

void set_active_schedules(schedule* sches, uint16_t n_sches){
  Serial.println("**SCHEDULES SET**");
  for(uint16_t i=0;i<n_sches;i++){
    Serial.print(sches[i].schedule_id);
    Serial.print(" ");
    Serial.print(sches[i].valve_id);
    Serial.print(" ");
    Serial.print(sches[i].start);
    Serial.print(" ");
    Serial.println(sches[i].stop);
  }
}
