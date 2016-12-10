/*
Description:
This is a library for Texas Instruments DAC1220
http://www.ti.com/lit/ds/symlink/dac1220.pdf

Version:
V0.1

Release Date:
2016-11-16

Author:
Peng Wei          info@sandboxelectronics.com

Lisence:
*/
#include <SPI.h>
#include <dac1220.h>
#include <OneWire.h>
#include <Wire.h>

DAC1220  MyDAC;
uint32_t Millis = 0;
uint8_t  Buf[10];
uint8_t  Len = 0;
uint32_t code1 = 0;

OneWire  ds(6);  // on pin 10
float DS_TEMP;


void setup() {
    MyDAC.begin();
    Serial.begin(9600);
    delay(1000);
    Serial.println("Ready to receive target voltage. Examples:");
    Serial.println("1.0234");
    Serial.println("4.5");
    Serial.println("2");
    Serial.println("------------------------------------------");
}

void loop() {
    float v;
    
    if (millis() - Millis > 10) {
        if (Len) {
            if (parseFloat(Buf, Len, &v)) {
                MyDAC.writeV(v);
                getTemp();
                Serial.print("V1 = ");
                Serial.print(v, 6);
                Serial.print(" ");
                Serial.print("V2 = ");
                Serial.print(v*2, 6);
                Serial.print(" ");
                code1 = (uint32_t)(v/5 * 0xFFFFF + 0.5);
                
                Serial.print("CD1 = ");
                Serial.print(code1,6);
                Serial.print(" ");
                Serial.print("HX = ");
                Serial.print(code1,HEX);

                Serial.print(" ");
                Serial.print("TMP C = ");
                Serial.println(DS_TEMP,4);
                
                
                
            }
        }

        Len = 0;
    }
    
    while(Serial.available()) {
        if (Len < 10) {
            Buf[Len++] = Serial.read();
        } else {
            Serial.read();
        }

        Millis = millis();
    }
}

uint8_t parseFloat(uint8_t *pbuf, uint8_t len, float *f) {
    uint8_t  i;
    uint8_t  c;
    uint8_t  period = false;
    uint8_t  n = 0;
    uint32_t m = 0;
    uint32_t divider = 1;
    
    for (i=0; i<len; i++) {
        c = pbuf[i];
        
        if (c == '.') {
            if (period == false) {
                period = true;
                n = len - i - 1;
            } else {
                return false;
            }
        } else if (pbuf[i] >= 0x30 && pbuf[i] <= 0x39) {
            m = m * 10 + (c - 0x30);
        } else {
            return false;
        }
    }

    for (i=0; i<n; i++) {
        divider *= 10;
    }

   *f = (float)m / divider;
    return true;
}



void getTemp() {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    //Serial.println("No more addresses.");
    //Serial.println();
    ds.reset_search();
    delay(2);
    return;
  }
  
  ///Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
  ///  Serial.write(' ');
  ///  Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  ///Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      ///Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      ///Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      ///Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      ///Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  ///Serial.print("  Data = ");
  ///Serial.print(present,HEX);
  ///Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    ///Serial.print(data[i], HEX);
    ///Serial.print(" ");
  }
  ///Serial.print(" CRC=");
  ///Serial.print(OneWire::crc8(data, 8), HEX);
  ///Serial.println();

  // convert the data to actual temperature

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  DS_TEMP = celsius;
  fahrenheit = celsius * 1.8 + 32.0;
  //Serial.print("  Temperature = ");
  //Serial.print(celsius);
  //Serial.print(" Celsius, ");
  //Serial.print(fahrenheit);
  //Serial.println(" Fahrenheit");
}











