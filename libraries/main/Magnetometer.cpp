#include "Magnetometer.h"
#include "Printer.h"
extern Printer printer;

Magnetometer::Magnetometer() : DataSource("MMTmagX,MMTmagY,MMTmagZ,MMTmagTotal", "float,float,float,float") {}

void Magnetometer::init(void) {
    Serial.print("Initializing magnetometer");
}

//addr is the 7 bit value of the register's address (without the R/W bit)
uint8_t Magnetometer::readReg(uint8_t addr){
  uint8_t data = 0;
  
  // Enable transmission to specific which register to read from
  WIRE.beginTransmission(MMTAdr);
  WIRE.write(addr); //request from the REVID register
  WIRE.endTransmission();

  delay(100);

  // Request 1 byte from the register specified earlier
  WIRE.requestFrom(MMTAdr, 1);
  if(WIRE.available() == 1) {
    data = WIRE.read();
  }
  return data;
}

//addr is the 7 bit (No r/w bit) value of the internal register's address, data is 8 bit data being written
void Magnetometer::writeReg(uint8_t addr, uint8_t data){
  WIRE.beginTransmission(MMTAdr);
  WIRE.write(addr);
  WIRE.write(data);
  WIRE.endTransmission();
}

// Calculate new 16 bit cycle count
void Magnetometer::changeCycleCount(uint16_t newCC){
  uint8_t CCMSB = (newCC & 0xFF00) >> 8; //get the most significant byte
  uint8_t CCLSB = newCC & 0xFF; //get the least significant byte
  
  WIRE.beginTransmission(MMTAdr);
  WIRE.write(CCX1Reg);
  WIRE.write(CCMSB);  //write new cycle count to ccx1
  WIRE.write(CCLSB);  //write new cycle count to ccx0
  WIRE.write(CCMSB);  //write new cycle count to ccy1
  WIRE.write(CCLSB);  //write new cycle count to ccy0
  WIRE.write(CCMSB);  //write new cycle count to ccz1
  WIRE.write(CCLSB);  //write new cycle count to ccz0     
  WIRE.endTransmission();  
}

String Magnetometer::printValues(void) {
  String printString = "Magnetometer:"; 
  printString += " X: ";
  printString += String(stateMMT.MMTmagX);
  printString += "[uTesla],";
  printString += " Y: "; 
  printString += String(stateMMT.MMTmagY);
  printString += "[uTesla],";
  printString += " Z: ";
  printString += String(stateMMT.MMTmagZ);
  printString += "[uTesla]";
  printString += " Magnitude: ";
  printString += String(stateMMT.MMTmagTotal);
  printString += "[uTesla]";
  return printString; 
}

void Magnetometer::measureMMT(float gain) {
  x = 0;
  y = 0;
  z = 0;

  // When data is ready
  if (DRDY) {
    // Data ready when DRDYPin is low
    while(digitalRead(DRDYPin) == LOW);
  } else {
    // Data ready by checking status register
    while((readReg(STATUSReg) & 0x80) != 0x80);
  }

  // Request a measurement
  WIRE.beginTransmission(MMTAdr);
  WIRE.write(0x24);
  WIRE.endTransmission();

  // Request 9 bytes of measurement data
  WIRE.requestFrom(MMTAdr, 9);
  if(WIRE.available() == 9) {
    x2 = WIRE.read();
    x1 = WIRE.read();
    x0 = WIRE.read();
    
    y2 = WIRE.read();
    y1 = WIRE.read();
    y0 = WIRE.read();
    
    z2 = WIRE.read();
    z1 = WIRE.read();
    z0 = WIRE.read();
  }

  // Bit manipulation necessary to actually read values (no 24 bit signed int data type)
  if (x2 & 0x80){
      x = 0xFF;
  }
  if (y2 & 0x80){
      y = 0xFF;
  }
  if (z2 & 0x80){
      z = 0xFF;
  }

  // Reformat to 32 bit signed datatypes
  x = (x * 256 * 256 * 256) | (int32_t)(x2) * 256 * 256 | (uint16_t)(x1) * 256 | x0;
  y = (y * 256 * 256 * 256) | (int32_t)(y2) * 256 * 256 | (uint16_t)(y1) * 256 | y0;
  z = (z * 256 * 256 * 256) | (int32_t)(z2) * 256 * 256 | (uint16_t)(z1) * 256 | z0;

  stateMMT.MMTmagX = (float)(x)/gain;
  stateMMT.MMTmagY = (float)(y)/gain;
  stateMMT.MMTmagZ = (float)(z)/gain;

  // Magnitude of x y z readings
  stateMMT.MMTmagTotal = sqrt(pow(((float)(x)/gain),2) + pow(((float)(y)/gain),2)+ pow(((float)(z)/gain),2));
}

// Nathan said this was missing(?)
size_t Magnetometer::writeDataBytes(unsigned char * buffer, size_t idx) {
  float * data_slot = (float *) &buffer[idx];
  data_slot[0] = state.MMT.MMTmagX;
  data_slot[1] = state.MMT.MMTmagY;
  data_slot[2] = state.MMT.MMTmagZ;
  data_slot[3] = state.MMT.MMTmagTotal;

  return idx + 4*sizeof(float);
}