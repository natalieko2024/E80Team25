// Natalie Ko E80 Sp26
// Modified from: https://github.com/hnguy169/RM3100-Arduino/blob/main/RM3100_Arduino_I2C/RM3100_Arduino_I2C.ino

#include <Arduino.h> // Need for basic high/low stuff etc.
#include <Wire.h> // Wire library to read/write at specific addresses
#include <Logger.h>
#include <Printer.h>
#include <TimingOffsets.h>
#include <Magnetometer.h>

// #define WIRE Wire1 // Wire1 for Teensy SDA 17 and SCL 16 bc 18/19 taken by IMU

// // Variables
// #define CC 200 // Cycle count - lower is more samples but less resolution -> theres a formula to change if needed
// #define single 0 // 0 = continuous, 1 = single
// #define DRDY 1 // 0 = use register, 1 = use DRDYPin

// // General
// #define MMTAdr 0x20 // Slave address for magnetometer, Pin2 and Pin4 are low
// #define DRDYPin 14 // Data Ready Pin: D14 -> A0, SDA: 17, SCL:16

// // Registers
// #define REVIDReg 0x36 // Address for the Revid internal register
// #define POLLReg 0x00 // Address for the Poll internal register
// #define CMMReg 0x01 // Address for the CMM internal register
// #define STATUSReg 0x34 // Address for the Status internal register
// #define CCX1Reg 0x04 // Address for Cycle Count X1 internal register
// #define CCX0Reg 0x05 // Address for the Cycle Count X0 internal register

// Constants
uint8_t revid;
uint16_t cycleCount;
float gain;
// loop start recorder
int loopStartTime;
int currentTime;

Logger logger;
Printer printer;
Magnetometer mmt;

void setup() {
  // put your setup code here, to run once:
  logger.include(&mmt);
  printer.init();
  mmt.init();

  pinMode(DRDYPin, INPUT_PULLUP); // We want to read
  WIRE.begin(); // Start I2C bus
  Serial.begin(9600); // Baud rate for serial monitor
  delay(100);

  // Confirm this gives 0x22 if not something is wrong
  revid = mmt.readReg(REVIDReg);

  // Give CC to register so its right
  mmt.changeCycleCount(CC);
  cycleCount = mmt.readReg(CCX1Reg);
  cycleCount = (cycleCount << 8) | mmt.readReg(CCX0Reg);

  // Gain depends on cycle count apparently -> check this is 75 for Cc=200
  gain = (0.3671 * (float)cycleCount) + 1.5;

  // Set measurement mode
  if (single) {
    // Single measurement mode
    mmt.writeReg(CMMReg, 0);
    mmt.writeReg(POLLReg, 0x70);
  } else {
    //Continuous measurement mode
    mmt.writeReg(CMMReg, 0x79);
  }

  Serial.print("REVID ID = 0x"); 
  Serial.println(revid, HEX); 
  Serial.print("Cycle Counts = ");
  Serial.println(cycleCount);
  Serial.print("Gain = ");
  Serial.println(gain);

  printer.printMessage("Starting main loop",10);
  loopStartTime = millis();
  printer.lastExecutionTime            = loopStartTime - LOOP_PERIOD + PRINTER_LOOP_OFFSET ;
  logger.lastExecutionTime             = loopStartTime - LOOP_PERIOD + LOGGER_LOOP_OFFSET;

}

void loop() {
  // put your main code here, to run repeatedly:
  // long x = 0;
  // long y = 0;
  // long z = 0;
  uint8_t x2=0, x1=0, x0=0, y2=0, y1=0, y0=0, z2=0, z1=0, z0=0;
  double uT;

  x2, x1, x0, y2, y1, y0, z2, z1, z0 = mmt.measureMMT();
  uT = mmt.convertuT(gain, x2, x1, x0, y2, y1, y0, z2, z1, z0);
  // // When data is ready
  // if (DRDY) {
  //   // Data ready when DRDYPin is low
  //   while(digitalRead(DRDYPin) == LOW);
  // } else {
  //   // Data ready by checking status register
  //   while((mmt.readReg(STATUSReg) & 0x80) != 0x80);
  // }

  // // Request a measurement
  // WIRE.beginTransmission(MMTAdr);
  // WIRE.write(0x24);
  // WIRE.endTransmission();

  // // Request 9 bytes of measurement data
  // WIRE.requestFrom(MMTAdr, 9);
  // if(WIRE.available() == 9) {
  //   x2 = WIRE.read();
  //   x1 = WIRE.read();
  //   x0 = WIRE.read();
    
  //   y2 = WIRE.read();
  //   y1 = WIRE.read();
  //   y0 = WIRE.read();
    
  //   z2 = WIRE.read();
  //   z1 = WIRE.read();
  //   z0 = WIRE.read();
  // }

  // // Bit manipulation necessary to actually read values (no 24 bit signed int data type)
  // if (x2 & 0x80){
  //     x = 0xFF;
  // }
  // if (y2 & 0x80){
  //     y = 0xFF;
  // }
  // if (z2 & 0x80){
  //     z = 0xFF;
  // }

  // // Reformat to 32 bit signed datatypes
  // x = (x * 256 * 256 * 256) | (int32_t)(x2) * 256 * 256 | (uint16_t)(x1) * 256 | x0;
  // y = (y * 256 * 256 * 256) | (int32_t)(y2) * 256 * 256 | (uint16_t)(y1) * 256 | y0;
  // z = (z * 256 * 256 * 256) | (int32_t)(z2) * 256 * 256 | (uint16_t)(z1) * 256 | z0;

  // // Magnitude of x y z readings
  // double uT = sqrt(pow(((float)(x)/gain),2) + pow(((float)(y)/gain),2)+ pow(((float)(z)/gain),2));

  //display results
  // Serial.print("Data in microTesla(uT):");
  // Serial.print("   X:");
  // Serial.print((float)(x)/gain);
  // Serial.print("   Y:");
  // Serial.print((float)(y)/gain);
  // Serial.print("   Z:");
  // Serial.println((float)(z)/gain);
  // Serial.println(); 

  // // With NOTHING near it it should be in the range of 45 uT (+/- 15 uT)
  // Serial.print("Magnitude(uT):");
  // Serial.println(uT);
  // Serial.println(); 

  currentTime=millis();
    
  if ( currentTime-printer.lastExecutionTime > LOOP_PERIOD ) {
    printer.lastExecutionTime = currentTime;
    printer.printValue(0,mmt.printValues());  
    printer.printToSerial();  // To stop printing, just comment this line out
  }

}

// //addr is the 7 bit value of the register's address (without the R/W bit)
// uint8_t readReg(uint8_t addr){
//   uint8_t data = 0;
  
//   // Enable transmission to specific which register to read from
//   WIRE.beginTransmission(MMTAdr);
//   WIRE.write(addr); //request from the REVID register
//   WIRE.endTransmission();

//   delay(100);

//   // Request 1 byte from the register specified earlier
//   WIRE.requestFrom(MMTAdr, 1);
//   if(WIRE.available() == 1) {
//     data = WIRE.read();
//   }
//   return data;
// }

// //addr is the 7 bit (No r/w bit) value of the internal register's address, data is 8 bit data being written
// void writeReg(uint8_t addr, uint8_t data){
//   WIRE.beginTransmission(MMTAdr);
//   WIRE.write(addr);
//   WIRE.write(data);
//   WIRE.endTransmission();
// }

// // Calculate new 16 bit cycle count
// void changeCycleCount(uint16_t newCC){
//   uint8_t CCMSB = (newCC & 0xFF00) >> 8; //get the most significant byte
//   uint8_t CCLSB = newCC & 0xFF; //get the least significant byte
  
//   WIRE.beginTransmission(MMTAdr);
//   WIRE.write(CCX1Reg);
//   WIRE.write(CCMSB);  //write new cycle count to ccx1
//   WIRE.write(CCLSB);  //write new cycle count to ccx0
//   WIRE.write(CCMSB);  //write new cycle count to ccy1
//   WIRE.write(CCLSB);  //write new cycle count to ccy0
//   WIRE.write(CCMSB);  //write new cycle count to ccz1
//   WIRE.write(CCLSB);  //write new cycle count to ccz0     
//   WIRE.endTransmission();  
// }
