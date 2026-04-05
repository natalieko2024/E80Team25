#include <Arduino.h>
#include <Wire.h>

#define RM3100Address 0x20 // Hexadecimal slave address for RM3100 with Pin 2 and Pin 4 set to LOW

//pin definitions
#define PIN_DRDY 14 //Set pin D9 to be the Data Ready Pin

//internal register values without the R/W bit
#define RM3100_REVID_REG 0x36 // Hexadecimal address for the Revid internal register
#define RM3100_POLL_REG 0x00 // Hexadecimal address for the Poll internal register
#define RM3100_CMM_REG 0x01 // Hexadecimal address for the CMM internal register
#define RM3100_STATUS_REG 0x34 // Hexadecimal address for the Status internal register
#define RM3100_CCX1_REG 0x04 // Hexadecimal address for Cycle Count X1 internal register
#define RM3100_CCX0_REG 0x05 // Hexadecimal address for the Cycle Count X0 internal register

//options
#define initialCC 200 // Set the cycle count
#define singleMode 0 //0 = use continuous measurement mode; 1 = use single measurement mode
#define useDRDYPin 1 //0 = not using DRDYPin ; 1 = using DRDYPin to wait for data

uint8_t revid;
uint16_t cycleCount;
float gain;

#define WIRE Wire1

void setup() {
  pinMode(PIN_DRDY, INPUT);
  WIRE.begin(); // Initiate the WIRE library
  Serial.begin(9600); //set baud rate to 9600
  delay(100);

  revid = readReg(RM3100_REVID_REG);
  
  Serial.print("REVID ID = 0x"); //REVID ID should be 0x22
  Serial.println(revid, HEX);

  changeCycleCount(initialCC); //change the cycle count; default = 200 (lower cycle count = higher data rates but lower resolution)

  cycleCount = readReg(RM3100_CCX1_REG);
  cycleCount = (cycleCount << 8) | readReg(RM3100_CCX0_REG);

  Serial.print("Cycle Counts = "); //display cycle count
  Serial.println(cycleCount);

  gain = (0.3671 * (float)cycleCount) + 1.5; //linear equation to calculate the gain from cycle count

  Serial.print("Gain = "); //display gain; default gain should be around 75 for the default cycle count of 200
  Serial.println(gain);

  if (singleMode){
    //set up single measurement mode
    writeReg(RM3100_CMM_REG, 0);
    writeReg(RM3100_POLL_REG, 0x70);
  }
  else{
    // Enable transmission to take continuous measurement with Alarm functions off
    writeReg(RM3100_CMM_REG, 0x79);
  }
}

void loop() {
  long x = 0;
  long y = 0;
  long z = 0;
  uint8_t x2,x1,x0,y2,y1,y0,z2,z1,z0;

  //wait until data is ready using 1 of two methods (chosen in options at top of code)
  if(useDRDYPin){ 
    while(digitalRead(PIN_DRDY) == LOW); //check RDRY pin
  }
  else{
    while((readReg(RM3100_STATUS_REG) & 0x80) != 0x80); //read internal status register
  }

  WIRE.beginTransmission(RM3100Address);
  WIRE.write(0x24); //request from the first measurement results register
  WIRE.endTransmission();

  // Request 9 bytes from the measurement results registers
  WIRE.requestFrom(RM3100Address, 9);
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

  //special bit manipulation since there is not a 24 bit signed int data type
  if (x2 & 0x80){
      x = 0xFF;
  }
  if (y2 & 0x80){
      y = 0xFF;
  }
  if (z2 & 0x80){
      z = 0xFF;
  }

  //format results into single 32 bit signed value
  x = (x * 256 * 256 * 256) | (int32_t)(x2) * 256 * 256 | (uint16_t)(x1) * 256 | x0;
  y = (y * 256 * 256 * 256) | (int32_t)(y2) * 256 * 256 | (uint16_t)(y1) * 256 | y0;
  z = (z * 256 * 256 * 256) | (int32_t)(z2) * 256 * 256 | (uint16_t)(z1) * 256 | z0;

  //calculate magnitude of results
  double uT = sqrt(pow(((float)(x)/gain),2) + pow(((float)(y)/gain),2)+ pow(((float)(z)/gain),2));

  //display results
  Serial.print("Data in counts:");
  Serial.print("   X:");
  Serial.print(x);
  Serial.print("   Y:");
  Serial.print(y);
  Serial.print("   Z:");
  Serial.println(z);

  Serial.print("Data in microTesla(uT):");
  Serial.print("   X:");
  Serial.print((float)(x)/gain);
  Serial.print("   Y:");
  Serial.print((float)(y)/gain);
  Serial.print("   Z:");
  Serial.println((float)(z)/gain);

  //Magnitude should be around 45 uT (+/- 15 uT)
  Serial.print("Magnitude(uT):");
  Serial.println(uT);
  Serial.println();     
}

//addr is the 7 bit value of the register's address (without the R/W bit)
uint8_t readReg(uint8_t addr){
  uint8_t data = 0;
  
  // Enable transmission to specific which register to read from
  WIRE.beginTransmission(RM3100Address);
  WIRE.write(addr); //request from the REVID register
  WIRE.endTransmission();

  delay(100);

  // Request 1 byte from the register specified earlier
  WIRE.requestFrom(RM3100Address, 1);
  if(WIRE.available() == 1) {
    data = WIRE.read();
  }
  return data;
}

//addr is the 7 bit (No r/w bit) value of the internal register's address, data is 8 bit data being written
void writeReg(uint8_t addr, uint8_t data){
  WIRE.beginTransmission(RM3100Address);
  WIRE.write(addr);
  WIRE.write(data);
  WIRE.endTransmission();
}

//newCC is the new cycle count value (16 bits) to change the data acquisition
void changeCycleCount(uint16_t newCC){
  uint8_t CCMSB = (newCC & 0xFF00) >> 8; //get the most significant byte
  uint8_t CCLSB = newCC & 0xFF; //get the least significant byte
  
  WIRE.beginTransmission(RM3100Address);
  WIRE.write(RM3100_CCX1_REG);
  WIRE.write(CCMSB);  //write new cycle count to ccx1
  WIRE.write(CCLSB);  //write new cycle count to ccx0
  WIRE.write(CCMSB);  //write new cycle count to ccy1
  WIRE.write(CCLSB);  //write new cycle count to ccy0
  WIRE.write(CCMSB);  //write new cycle count to ccz1
  WIRE.write(CCLSB);  //write new cycle count to ccz0     
  WIRE.endTransmission();  
}