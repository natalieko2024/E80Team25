#ifndef __SURFACECONTROL_H__
#define __SURFACECONTROL_H__

#define SUCCESS_RADIUS 2.0 // success radius in meters

#include <Arduino.h>
#include "MotorDriver.h"
#include "XYStateEstimator.h"
extern MotorDriver motorDriver;

class SurfaceControl : public DataSource
{
public:
  SurfaceControl(void);

  // defines the waypoints used for Surface Control
  void init(const int totalWayPoints_in, double * wayPoints_in, int navigateDelay_in);

  // sets the right and left motor efforts using P-Control
  void navigate(xy_state_t * state, gps_state_t * gps_state_p, int currentTime_in);

  String printString(void);

  String printWaypointUpdate(void);

  // from DataSource
  size_t writeDataBytes(unsigned char * buffer, size_t idx);

  int lastExecutionTime = -1;

  // control fields
  float yaw_des;         // desired yaw
  float yaw;             // current yaw
  float yaw_error;       // difference between current and desired yaw
  float dist;            // distance to waypoint
  float u;               // control effort
  float Kp=10.0;         // proportional control gain
  float Kr=1.0;          // right motor gain correction; change if motors are not running at same speed
  float Kl=1.0;          // left motor gain correction; change if motors are not running at same speed
  float avgPower = 50.0; // average forward thrust, change to 50 from 10 per 4.5 instructions
  float uR = uR*Kr;              // right motor effort
  float uL = uL*Kl;              // left motor effort

  // uR and uL bounds

  // if (uR > 127) uR = 127; //uR control values
  // if (uR < 0) uR = 0;

  // if (uL > 127) uL = 127; //uL control values
  // if (uL < 0) uL = 0;



  bool navigateState = 1;
  bool atPoint;
  bool complete = 0;

  int totalWayPoints;
  double * wayPoints;

private:

  // updates the current waypoint if necessary
  void updatePoint(float x, float y);

  int getWayPoint(int dim);

  const int stateDims = 2;  // Number of dimensions in the state vector (x,y)
  int currentWayPoint = 0;
  bool gpsAcquired;
  
  int navigateDelay;
  int delayStartTime = 0;
  int currentTime;
  bool delayed;
};



#endif
