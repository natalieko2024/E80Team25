#ifndef __MMT_H__
#define __MMT_H__

// General
#define MMTAdr 0x20 // Slave address for magnetometer, Pin2 and Pin4 are low
#define DRDYPin 31 // Data Ready Pin: D14 -> A0, SDA: 17, SCL:16

// Registers
#define REVIDReg 0x36 // Address for the Revid internal register
#define POLLReg 0x00 // Address for the Poll internal register
#define CMMReg 0x01 // Address for the CMM internal register
#define STATUSReg 0x34 // Address for the Status internal register
#define CCX1Reg 0x04 // Address for Cycle Count X1 internal register
#define CCX0Reg 0x05 // Address for the Cycle Count X0 internal register

#define WIRE Wire1
// Variables
#define CC 200 // Cycle count - lower is more samples but less resolution -> theres a formula to change if needed
#define single 0 // 0 = continuous, 1 = single
#define DRDY 1 // 0 = use register, 1 = use DRDYPin

#include <Arduino.h> // Need for basic high/low stuff etc.
#include "DataSource.h"
#include <Wire.h> // Wire library to read/write at specific addresses

typedef struct {
    float MMTmagX;
    float MMTmagY;
    float MMTmagZ;
    float MMTmagTotal;
} magnetometer_state;

class Magnetometer : public DataSource {
public:
    // Pass the CSV header strings to the DataSource constructor
    Magnetometer();
        
    void init(void);
    uint8_t readReg(uint8_t addr);
    void writeReg(uint8_t addr, uint8_t data);
    void changeCycleCount(uint16_t newCC);
    String printValues(void);
    void measureMMT(float gain);
    //double convertuT(float gain, uint8_t x2, uint8_t x1, uint8_t x0, uint8_t y2, uint8_t y1, uint8_t y0, uint8_t z2, uint8_t z1, uint8_t z0);

    int lastExecutionTime = -1;
    long x = 0;
    long y = 0;
    long z = 0;
    uint8_t x2=0, x1=0, x0=0, y2=0, y1=0, y0=0, z2=0, z1=0, z0=0;

    // Need this
    size_t writeDataBytes(unsigned char * buffer, size_t idx) override;

    magnetometer_state stateMMT;
};

#endif