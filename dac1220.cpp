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

#include <Arduino.h>
#include "SPI.h"
#include "dac1220.h"

DAC1220::DAC1220() {
//    DAC1220(RESOLUTION_20BIT, 6);
    Resolution = RESOLUTION_20BIT;
    CSPin = 5;
}

DAC1220::DAC1220(uint8_t resolution, uint8_t cs) {
    if (resolution == RESOLUTION_16BIT) {
        Resolution = RESOLUTION_16BIT;
    } else {
        Resolution = RESOLUTION_20BIT;
    }

    CSPin = cs;
}


void DAC1220::begin() {
    reset();
}


void DAC1220::reset() {
    SPI.end();
    pinMode(CSPin, OUTPUT);
    digitalWrite(CSPin, LOW);

    //Reset DAC
    pinMode(24, OUTPUT);
    digitalWrite(24, LOW);
    delay(1);
    digitalWrite(24, HIGH);
    delayMicroseconds(240); //First high period (600 clocks)
    digitalWrite(24, LOW);
    delayMicroseconds(5);
    digitalWrite(24, HIGH);
    delayMicroseconds(480); //Second high period (1200 clocks)
    digitalWrite(24, LOW);
    delayMicroseconds(5);
    digitalWrite(24, HIGH);
    delayMicroseconds(960); //Second high period (2400 clocks)
    digitalWrite(24, LOW);
    delay(1);

    SPI.begin();
    SPI.setDataMode(SPI_MODE1);
    SPI.setClockDivider(240);
    SPI.setBitOrder(MSBFIRST);

    //Start Self-Calibration
    SPI.transfer(0x05);

    if (Resolution == RESOLUTION_16BIT) {
        SPI.transfer(0x21); //16-bit resolution
    } else {
        SPI.transfer(0xA1); //20-bit resolution
    }

    digitalWrite(CSPin, HIGH);

    //Wait 500ms for Self-Calibration to complete
    delay(500);
}


void DAC1220::writeV(float v) {
    uint32_t code;

    if (v < 0) {
        v = 0;
    } else if (v > 5) {
        v = 5;
    }

    if (Resolution == RESOLUTION_20BIT) {
        code = (uint32_t)(v/5 * 0xFFFFF + 0.5);
        writeCode(code << 4);
  if (code > 0xFFFFF) {
   code = 0xFFFFF;
  }
    } else if (Resolution == RESOLUTION_16BIT) {
        code = (uint32_t)(v/5 * 0xFFFF + 0.5);
  if (code > 0xFFFF) {
   code = 0xFFFF;
  }
        writeCode(code << 8);
    }
}


void DAC1220::writeCode(uint32_t code) {
    digitalWrite(CSPin, LOW);
    SPI.transfer(0x40);
    SPI.transfer((code&0x00FF0000)>>16);
    SPI.transfer((code&0x0000FF00)>>8);
    SPI.transfer((code&0x000000FF));
    digitalWrite(CSPin, HIGH);
}
