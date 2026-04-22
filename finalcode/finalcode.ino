/********
Based on E80 Lab 7 Surface and Dive Activity Codes
Authors:
    Omar Aleman (oaleman@g.hmc.edu) '21 (contributed 2019)
    Wilson Ives (wives@g.hmc.edu) '20 (contributed in 2018)
    Christopher McElroy (cmcelroy@g.hmc.edu) '19 (contributed in 2017)  
    Josephine Wong (jowong@hmc.edu) '18 (contributed in 2016)
    Apoorva Sharma (asharma@hmc.edu) '17 (contributed in 2016)

Magnetometer code modified from https://github.com/hnguy169/RM3100-Arduino/blob/main/RM3100_Arduino_I2C/RM3100_Arduino_I2C.ino

Written for Sp26 Team 25 by Natalie Ko (nko@g.hmc.edu)                    
*/

#include <Arduino.h>
#include <Wire.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <Pinouts.h>
#include <TimingOffsets.h>
#include <SensorGPS.h>
#include <SensorIMU.h>
#include <XYStateEstimator.h>
#include <ZStateEstimator.h>
#include <ADCSampler.h>
#include <ErrorFlagSampler.h>
#include <ButtonSampler.h> // A template of a data source library
#include <MotorDriver.h>
#include <Logger.h>
#include <Printer.h>
#include <SurfaceControl.h>
#include <DepthControl.h>
#define UartSerial Serial1
#include <GPSLockLED.h>

#include <Magnetometer.h>

/////////////////////////* Global Variables *////////////////////////

MotorDriver motor_driver;
XYStateEstimator xy_state_estimator;
SurfaceControl surface_control;
ZStateEstimator z_state_estimator;
DepthControl depth_control;
SensorGPS gps;
Adafruit_GPS GPS(&UartSerial);
ADCSampler adc;
ErrorFlagSampler ef;
ButtonSampler button_sampler;
SensorIMU imu;
Logger logger;
Printer printer;
GPSLockLED led;

// loop start recorder
int loopStartTime;
int currentTime;
volatile bool EF_States[NUM_FLAGS] = {1,1,1};

Magnetometer mmt;

// magnetometer constants
uint8_t revid;
uint16_t cycleCount;
float gain;

////////////////////////* Setup *////////////////////////////////

void setup() {
  
  logger.include(&imu);
  logger.include(&gps);
  logger.include(&xy_state_estimator);
  logger.include(&surface_control);
  logger.include(&z_state_estimator);
  logger.include(&depth_control);
  logger.include(&motor_driver);
  logger.include(&adc);
  logger.include(&ef);
  logger.include(&button_sampler);
  logger.include(&mmt);
  logger.init();

  printer.init();
  ef.init();
  button_sampler.init();
  imu.init();
  UartSerial.begin(9600);
  gps.init(&GPS);
  motor_driver.init();
  led.init();

// track depth is correct, if depth is correct check waypoint tracking is correct
// motor control effort going down is too high, motor control effort going up is too low?? may change values

// change Kp = 100 or 500 -> that should make it go straight
// if doesnt work change Kr and Kl to make the efforts of RL motors match
// change average power?
// change threshold 0-127 to sm higher

  int navigateDelay = 0; // how long robot will stay at surface waypoint before continuing (ms)

  const int num_surface_waypoints = 3; // Number of ordered pairs of surface waypoints. (e.g., if surface_waypoints is {x0,y0,x1,y1} then num_surface_waypoints is 2.) Set to 0 if only doing depth control 
  // START, END, START
  double surface_waypoints [] = { 0, 0, 0, -5, 0, 0 };   // listed as x0,y0,x1,y1, ... etc.
  surface_control.init(num_surface_waypoints, surface_waypoints, navigateDelay);

  int diveDelay = 5000; // how long robot will stay at depth waypoint before continuing (ms)

  const int num_depth_waypoints = 6;
  double depth_waypoints [] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5 };  // listed as z0,z1,... etc.
  const int num_depths_check = 100;
  depth_control.init(num_depth_waypoints, depth_waypoints, num_depths_check, diveDelay);

  xy_state_estimator.init(); 
  z_state_estimator.init();

  mmt.init();
  pinMode(DRDYPin, INPUT_PULLUP); // We want to read
  WIRE.begin(); // Start I2C bus
  // Confirm this gives 0x22 if not something is wrong
  revid = mmt.readReg(REVIDReg);
  Serial.println(revid);

  delay(1000);

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

  printer.printMessage("Starting main loop",10);
  loopStartTime = millis();
  printer.lastExecutionTime            = loopStartTime - LOOP_PERIOD + PRINTER_LOOP_OFFSET ;
  imu.lastExecutionTime                = loopStartTime - LOOP_PERIOD + IMU_LOOP_OFFSET;
  adc.lastExecutionTime                = loopStartTime - LOOP_PERIOD + ADC_LOOP_OFFSET;
  ef.lastExecutionTime                 = loopStartTime - LOOP_PERIOD + ERROR_FLAG_LOOP_OFFSET;
  button_sampler.lastExecutionTime     = loopStartTime - LOOP_PERIOD + BUTTON_LOOP_OFFSET;
  xy_state_estimator.lastExecutionTime = loopStartTime - LOOP_PERIOD + XY_STATE_ESTIMATOR_LOOP_OFFSET;
  surface_control.lastExecutionTime    = loopStartTime - LOOP_PERIOD + SURFACE_CONTROL_LOOP_OFFSET;
  z_state_estimator.lastExecutionTime  = loopStartTime - LOOP_PERIOD + Z_STATE_ESTIMATOR_LOOP_OFFSET;
  depth_control.lastExecutionTime      = loopStartTime - LOOP_PERIOD + DEPTH_CONTROL_LOOP_OFFSET;
  logger.lastExecutionTime             = loopStartTime - LOOP_PERIOD + LOGGER_LOOP_OFFSET;
  mmt.lastExecutionTime                = loopStartTime - LOOP_PERIOD + MMT_LOOP_OFFSET;

  surface_control.doDepth = true;
}



//////////////////////////////* Loop */////////////////////////

void loop() {

  currentTime=millis();
    
  if ( currentTime-printer.lastExecutionTime > LOOP_PERIOD ) {
    printer.lastExecutionTime = currentTime;
    printer.printValue(0,adc.printSample());
    //printer.printValue(1,button_sampler.printState());
    //printer.printValue(1,ef.printStates());
    printer.printValue(1,logger.printState());
    printer.printValue(2,gps.printState());   
    printer.printValue(3,xy_state_estimator.printState());  
    printer.printValue(4,surface_control.printWaypointUpdate());
    printer.printValue(5,surface_control.printString());
    printer.printValue(6,z_state_estimator.printState());  
    printer.printValue(7,depth_control.printWaypointUpdate());
    printer.printValue(8,depth_control.printString());
    printer.printValue(9,motor_driver.printState());
    printer.printValue(10,imu.printRollPitchHeading());        
    printer.printValue(11,imu.printAccels());
    printer.printValue(12,mmt.printValues());
    printer.printToSerial();  // To stop printing, just comment this line out
  }

  // // /// SURFACE CONTROL FINITE STATE MACHINE///
  // if ( currentTime-surface_control.lastExecutionTime > LOOP_PERIOD ) {
  //   surface_control.lastExecutionTime = currentTime;
  //   if ( surface_control.navigateState ) { // NAVIGATE STATE //
  //     if ( !surface_control.atPoint ) { 
  //       surface_control.navigate(&xy_state_estimator.state, &gps.state, currentTime);
  //     }
  //     else if ( surface_control.complete ) { 
  //       delete[] surface_control.wayPoints; // destroy surface waypoint array from the Heap
  //     }
  //     else {
  //       surface_control.atPoint = false;   // get ready to go to the next point
  //     }
  //     motor_driver.drive(surface_control.uL,surface_control.uR,-200);
  //   }
  // }

  // if doDepth, start doing depth control
  if (surface_control.doDepth == true) {
    /* ROBOT CONTROL Finite State Machine */
    if ( currentTime-depth_control.lastExecutionTime > LOOP_PERIOD ) {
      depth_control.lastExecutionTime = currentTime;
      if ( depth_control.diveState ) {      // DIVE STATE //
        depth_control.complete = false;
        if ( !depth_control.atDepth ) {
          depth_control.dive(&z_state_estimator.state, currentTime);
          depth_control.checkBottom();
        }
        else {
          depth_control.diveState = false; 
          depth_control.surfaceState = true;
        }
        motor_driver.drive(0,0,depth_control.uV);
      }
      if ( depth_control.surfaceState ) {     // SURFACE STATE //
        if ( !depth_control.atSurface ) { 
          depth_control.surface(&z_state_estimator.state);
        }
        else if ( depth_control.complete ) { 
          delete[] depth_control.wayPoints;   // destroy depth waypoint array from the Heap
        }
        motor_driver.drive(0,0,depth_control.uV);
      }
    }
  }
  
  
  if ( currentTime-adc.lastExecutionTime > LOOP_PERIOD ) {
    adc.lastExecutionTime = currentTime;
    adc.updateSample(); 
  }

  if ( currentTime-ef.lastExecutionTime > LOOP_PERIOD ) {
    ef.lastExecutionTime = currentTime;
    attachInterrupt(digitalPinToInterrupt(ERROR_FLAG_A), EFA_Detected, LOW);
    attachInterrupt(digitalPinToInterrupt(ERROR_FLAG_B), EFB_Detected, LOW);
    attachInterrupt(digitalPinToInterrupt(ERROR_FLAG_C), EFC_Detected, LOW);
    delay(5);
    detachInterrupt(digitalPinToInterrupt(ERROR_FLAG_A));
    detachInterrupt(digitalPinToInterrupt(ERROR_FLAG_B));
    detachInterrupt(digitalPinToInterrupt(ERROR_FLAG_C));
    ef.updateStates(EF_States[0],EF_States[1],EF_States[2]);
    EF_States[0] = 1;
    EF_States[1] = 1;
    EF_States[2] = 1;
  }

  // uses the ButtonSampler library to read a button -- use this as a template for new libraries!
  if ( currentTime-button_sampler.lastExecutionTime > LOOP_PERIOD ) {
    button_sampler.lastExecutionTime = currentTime;
    button_sampler.updateState();
  }

  if ( currentTime-imu.lastExecutionTime > LOOP_PERIOD ) {
    imu.lastExecutionTime = currentTime;
    imu.read();     // blocking I2C calls
  }
 
  gps.read(&GPS); // blocking UART calls, need to check for UART data every cycle

  if ( currentTime-xy_state_estimator.lastExecutionTime > LOOP_PERIOD ) {
    xy_state_estimator.lastExecutionTime = currentTime;
    xy_state_estimator.updateState(&imu.state, &gps.state);
  }
  
  if ( currentTime-z_state_estimator.lastExecutionTime > LOOP_PERIOD ) {
    z_state_estimator.lastExecutionTime = currentTime;
    z_state_estimator.updateState(analogRead(PRESSURE_PIN));
  }

  if ( currentTime-led.lastExecutionTime > LOOP_PERIOD ) {
    led.lastExecutionTime = currentTime;
    led.flashLED(&gps.state);
  }

  if ( currentTime- logger.lastExecutionTime > LOOP_PERIOD && logger.keepLogging ) {
    logger.lastExecutionTime = currentTime;
    logger.log();
  }

  if ( currentTime- mmt.lastExecutionTime > LOOP_PERIOD ) {
    mmt.lastExecutionTime = currentTime;
    mmt.measureMMT(gain);
  }
}

void EFA_Detected(void){
  EF_States[0] = 0;
}

void EFB_Detected(void){
  EF_States[1] = 0;
}

void EFC_Detected(void){
  EF_States[2] = 0;
}
