#include <stdlib.h>
#include <Arduino.h>
#include "dbg.h"

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
        Serial.print((uint8_t) msg[i]);
        if(i<size-1){
          Serial.print(' ');
        }
      }
      Serial.println("]");
}
