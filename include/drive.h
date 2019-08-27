#include <cmath>

using namespace vex;
using namespace std;

/***********************
Drive Subsystem
***********************/

class drive {
public:
  _pos sPos;
  _vector initPos;

  PID drivePID = PID(8, 0.5, 2);
  PID turnPID = PID(1.55, 0.08375, 0.45);
  PID correctionPID = PID(0.7, 0.04375, 0.175);

  float bckToMid = 9.0;
  float sideToMid = 7.5;

  float angToEnc = 0.8064;

  // circumfrence of wheel / encoder ticks per rotation
  float EncToInches = (3.3125 * 3.1415926535) / 900.0;
  float QEncToInches = (3.3125 * 3.1415926535) / 360.0;

  // change in angle
  float DeltaAngle = 0;

  // for robot position tracking
  float L = 0;
  float R = 0;
  float B = 0;

  float halfAng = 0;
  float hRL = 0;
  float hB = 0;

  float lstLft = 0;
  float lstRgt = 0;
  float lstBck = 0;
  float lstAng = 0;

  // desired drive positions for auto
  float desiredPos;
  float desiredAng;
  float initAng;

  float followDist;
  float followAng;

  bool turning;
  bool isTurnTo;

  // desired drive power
  int DesPower;

  void trackPos() {
    L = getLeftPosInches() - lstLft; // change in encoder
    R = getRightPosInches() - lstRgt;
    B = getBckPosInches() - lstBck;
    DeltaAngle = degToRad(getRoboAng()) - lstAng; // find change in robot angle

    lstLft = getLeftPosInches(); // record last pos
    lstRgt = getRightPosInches();
    lstBck = getBckPosInches();
    lstAng = degToRad(getRoboAng());

    if (DeltaAngle) {
      float radiusRL = R / DeltaAngle; // find the radius of the circle the
                                       // robot travels around using right enc
      float radiusB = B / DeltaAngle; // same as ^^ except the back tracking enc

      halfAng = (DeltaAngle / 2.0); // find half the angle traveled
      float sinHA = sin(halfAng);   // find the sin of half the angle

      hRL = ((radiusRL + sideToMid) * sinHA) * 2.0; // find change in Y
      hB = ((radiusB + bckToMid) * sinHA) * 2.0;    // find change in X
    } 
    else {
      halfAng = 0; // reset half
      hRL = R;
      hB = B;
    }

    float EndAng = (halfAng + degToRad(getRoboAng())); // find ending angle

    float sinEA = sin(EndAng); // calculate sin cos of ending angle
    float cosEA = cos(EndAng);

    // use ^^ values to adjust robot position on X Y coords
    sPos.y += hRL * cosEA;
    sPos.x += hRL * sinEA;

    sPos.y += hB * -sinEA; //-sin(x) = sin(-x)
    sPos.x += hB * cosEA;  // cos(x) = cos(-x)
  }

  // current postion of drive side (left)
  int getLeftPosition() {
    // average of front and back
    return LftDrive.rotation(rotationUnits::raw);
  }

  // get current position of mid wheel
  int getMidPosition() {
    return MidDrive.rotation(rotationUnits::raw); 
  }

  // current postion of drive side (right)
  int getRightPosition() {
    // average of front and back
    return RgtDrive.rotation(rotationUnits::raw);
  }

  // current psotion of drive side (back) for position tracking
  int getBckPosition() { return bckEnc.rotation(rotationUnits::raw); }

  // current psotion of drive side (back) in inches for position tracking
  float getBckPosInches() {
    return bckEnc.rotation(rotationUnits::raw) * QEncToInches;
  }

  // get current position of mid wheel in inches
  float getMidPosInches() {
    return MidDrive.rotation(rotationUnits::raw) * EncToInches;
  }

  // current postion of drive side (left) in icnhes
  float getLeftPosInches() {
    // convert to inches
    return -lftEnc.rotation(rotationUnits::raw) * QEncToInches;
  }

  // current postion of drive side (right) in icnhes
  float getRightPosInches() {
    // convert to inches
    return -rgtEnc.rotation(rotationUnits::raw) * QEncToInches;
  }

  //find angle of robot through gyro 
  float getRoboAng(){
    return roboGyro.value(rotationUnits::raw)/10;
  }

  void resetRobotPos() {
    // reset angle
    sPos.Ang = 0;

    // reset last enc values
    lstBck = 0;
    lstLft = 0;
    lstRgt = 0;

    // reset current pos
    sPos.x = 0;
    sPos.y = 0;
  }

  // clear drive values
  void reset() {
    RgtDrive.resetRotation(); // reset drive sensors
    LftDrive.resetRotation();

    rgtEnc.resetRotation();
    lftEnc.resetRotation();

    bckEnc.resetRotation();
    MidDrive.resetRotation();

    turning = false;

    desiredPos = 0; // reset desired positions
    desiredAng = 0;
    DesPower = 0;
    resetRobotPos();
  }

  // set drive speed
  void move_drive(int lftPow, int rgtPow) {
    int midPow;
    // limit power to 100%
    if (lftPow > 100) {
      lftPow = 100;
    } else if (lftPow < -100) {
      lftPow = -100;
    }

    if (rgtPow > 100) {
      rgtPow = 100;
    } else if (rgtPow < -100) {
      rgtPow = -100;
    }

    //prevent the middle motor from spinning when robot is turning
    if ((rgtPow < 0 && lftPow > 0) || (rgtPow > 0 && lftPow < 0)) {
      midPow = 0;
    } else {
      midPow = (rgtPow + lftPow) / 2;
    }

    // set speed to motor
    RgtDrive.spin(directionType::fwd, rgtPow, velocityUnits::pct);
    LftDrive.spin(directionType::fwd, lftPow, velocityUnits::pct);
    MidDrive.spin(directionType::fwd, midPow, velocityUnits::pct);
  }

  void stopDrive() {
    RgtDrive.stop(brakeType::brake);
    LftDrive.stop(brakeType::brake);
  }
};
