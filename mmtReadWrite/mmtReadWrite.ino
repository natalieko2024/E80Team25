// Natalie Ko E80 Sp26
// Modified from: https://github.com/hnguy169/RM3100-Arduino/blob/main/RM3100_Arduino_I2C/RM3100_Arduino_I2C.ino

#include <Wire.h> //Wire library to read/write at specific addresses

// General
#define MMTAdr 0x20 // Slave address for magnetometer, Pin2 and Pin4 are low
#define DrdyPin 20 // Data Ready Pin: 20, SDA: 18, SCK:19

// Registers
#define RevidReg 0x36 // Address for the Revid internal register
#define PollReg 0x00 // Address for the Poll internal register
#define CmmReg 0x01 // Address for the CMM internal register
#define StatusReg 0x34 // Address for the Status internal register
#define CcX1Reg 0x04 // Address for Cycle Count X1 internal register
#define CcX0Reg 0x05 // Address for the Cycle Count X0 internal register

// Constants
#define CC 200 // Cycle count - lower is more samples but less resolution -> theres a formula to change if needed
uint8_t revid;
uint16_t cycleCount;
float gain;

void setup() {
  // put your setup code here, to run once:
  pinMode(DrdyPin, INPUT); // We want to read
  Wire.begin(); // Start I2C bus
  Serial.begin(9600); // Baud rate
  delay(100);

  // Confirm this gives 0x22 if not something is wrong
  revid = readReg(RevidReg);
  Serial.print("REVID ID = 0x"); 
  Serial.println(revid, HEX); 

  // Give CC to register so its right
  changeCycleCount(initialCC);
  cycleCount = readReg(CcX1Reg);
  cycleCount = (cycleCount << 8) | readReg(CcX0Reg);
  Serial.print("Cycle Counts = ");
  Serial.println(cycleCount);

  // Gain depends on cycle count apparently -> check this is 75 for Cc=200
  gain = (0.3671 * (float)cycleCount) + 1.5;
  Serial.print("Gain = ");
  Serial.println(gain);

  // Single measurement mode (there's other code for continuous)
  writeReg(CmmReg, 0);
  writeReg(PollReg, 0x70);
}

void loop() {
  // put your main code here, to run repeatedly:
  long x = 0;
  long y = 0;
  long z = 0;
  uint8_t x2,x1,x0,y2,y1,y0,z2,z1,z0;

  // When data is ready
  while(digitalRead(DrdyPin) == LOW);

  Wire.beginTransmission(MMTAdr);
  Wire.write(0x24); //request from the first measurement results register
  Wire.endTransmission();

  // Request 9 bytes from the measurement results registers
  Wire.requestFrom(MMTAdr, 9);
  if(Wire.available() == 9) {
    x2 = Wire.read();
    x1 = Wire.read();
    x0 = Wire.read();
    
    y2 = Wire.read();
    y1 = Wire.read();
    y0 = Wire.read();
    
    z2 = Wire.read();
    z1 = Wire.read();
    z0 = Wire.read();
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

  //calculate magnitude of results
  double uT = sqrt(pow(((float)(x)/gain),2) + pow(((float)(y)/gain),2)+ pow(((float)(z)/gain),2));

  //display results
  Serial.print("Data in microTesla(uT):");
  Serial.print("   X:");
  Serial.print((float)(x)/gain);
  Serial.print("   Y:");
  Serial.print((float)(y)/gain);
  Serial.print("   Z:");
  Serial.println((float)(z)/gain);
  Serial.println(); 

  // Check in the range of 45 uT (+/- 15 uT)
  Serial.print("Magnitude(uT):");
  Serial.println(uT);
  Serial.println();     
}

//addr is the 7 bit value of the register's address (without the R/W bit)
uint8_t readReg(uint8_t addr){
  uint8_t data = 0;
  
  // Enable transmission to specific which register to read from
  Wire.beginTransmission(MMTAdr);
  Wire.write(addr); //request from the REVID register
  Wire.endTransmission();

  delay(100);

  // Request 1 byte from the register specified earlier
  Wire.requestFrom(MMTAdr, 1);
  if(Wire.available() == 1) {
    data = Wire.read();
  }
  return data;
}

//addr is the 7 bit (No r/w bit) value of the internal register's address, data is 8 bit data being written
void writeReg(uint8_t addr, uint8_t data){
  Wire.beginTransmission(MMTAdr);
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission();
}

// Calculate new 16 bit cycle count
void changeCycleCount(uint16_t newCC){
  uint8_t CCMSB = (newCC & 0xFF00) >> 8; //get the most significant byte
  uint8_t CCLSB = newCC & 0xFF; //get the least significant byte
  
  Wire.beginTransmission(MMTAdr);
  Wire.write(CcX1Reg);
  Wire.write(CCMSB);  //write new cycle count to ccx1
  Wire.write(CCLSB);  //write new cycle count to ccx0
  Wire.write(CCMSB);  //write new cycle count to ccy1
  Wire.write(CCLSB);  //write new cycle count to ccy0
  Wire.write(CCMSB);  //write new cycle count to ccz1
  Wire.write(CCLSB);  //write new cycle count to ccz0     
  Wire.endTransmission();  
}
