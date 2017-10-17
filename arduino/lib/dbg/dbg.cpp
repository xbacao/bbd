#include <stdlib.h>
#include <Arduino.h>
#include "dbg.h"

#ifdef DEBUG

void dbg_print_serial_buffer(const char* file, int line, const char* msg,
    const char* buffer, size_t size){
  Serial.print(file);
  Serial.print(":");
  Serial.println(line);
  Serial.print(msg);
  Serial.print(" : [");
  for(size_t i=0;i<size;i++){
    if(!isalpha(buffer[i]) && !isdigit(buffer[i])){
      Serial.print("\\");
      Serial.print((uint8_t) buffer[i]);
      Serial.print("\\");
    }
    else{
      Serial.print(buffer[i]);
    }

  }
  Serial.println("]");
}

void dbg_print_sock_buffer(const char* file, int line, const char* msg,
    const char* buffer, size_t size){
  Serial.print(file);
  Serial.print(":");
  Serial.println(line);
  Serial.print(msg);
  Serial.print(" : [");
  for(size_t i=0;i<size;i++){
    Serial.print((uint8_t) buffer[i]);
    if(i<size-1){
      Serial.print(' ');
    }
  }
  Serial.println("]");
}

void dbg_print_error(const char* file, int line, int code){
  Serial.print("ERROR in ");
  Serial.print(file);
  Serial.print(":");
  Serial.print(line);
  Serial.print(" code ");
  Serial.println(code);
}

void dbg_print_val(const char* file, int line, const char* msg, uint16_t val){
  Serial.print("NOTIFY in ");
  Serial.print(file);
  Serial.print(":");
  Serial.print(line);
  Serial.print(" msg:");
  Serial.print(msg);
  Serial.print(" val ");
  Serial.println(val);
}
#endif
