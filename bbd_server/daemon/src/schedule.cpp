#include "schedule.h"


std::ostream& operator<< (std::ostream& os, const schedule& sche){
  os << "{id:["<<sche.schedule_id<<"], valve_id:["<<sche.valve_id<<"], start:["<<sche.start/60<<":"
    <<sche.start%60<<"], stop:["<<sche.stop/60<<":"<<sche.stop%60<<"]}";
  return os;
}
