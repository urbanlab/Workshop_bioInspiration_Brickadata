#define N_ANALOG_INPUTS (int)14

#define N_SLAVES (int)2
#define N_ARDUINO (int)(N_SLAVES+1)
#define N_IN (int)7

//#define MASTER
#define SLAVE
#define SLAVE_INDEX 1

#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#ifdef MASTER
#include "Master.h";
Master APP_MASTER;
#endif

#ifdef SLAVE
#include "Slave.h";
Slave APP_SLAVE;
#endif

void setup() 
{   
  if ( N_IN*2 > N_ANALOG_INPUTS )
  {
    Serial.println("ERROR : NOT ENOUGH ANALOG_INPUTS");
    delay(10000);
  }

  Serial.begin(115200);
  while (!Serial) continue;

  #ifdef MASTER
  APP_MASTER.setup();
  #endif

  #ifdef SLAVE
  APP_SLAVE.setup(SLAVE_INDEX);
  #endif
}

void loop() 
{
  #ifdef MASTER
  APP_MASTER.loop();
  #endif

  #ifdef SLAVE
  APP_SLAVE.loop();
  #endif

  delay(100);
}
