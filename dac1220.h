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

#ifndef _DAC1220_H_
#define _DAC1220_H_

#define RESOLUTION_16BIT (0x01)
#define RESOLUTION_20BIT (0x02)

class DAC1220 {
    public:
        DAC1220();
        DAC1220(uint8_t resolution, uint8_t cs);
        void begin();
        void reset();
        void writeV(float v);
        void writeCode(uint32_t code);

    private:
        uint8_t Resolution;
        uint8_t CSPin;
};

#endif







