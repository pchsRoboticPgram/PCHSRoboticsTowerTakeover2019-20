void setDrive(int power, float drivePos, float turnAngle, float rate = 1, bool slowdown = false){
  //- left + right

  //reset enc
  MidDrive.resetRotation();
  //reset PID values
  Drive.drivePID.changePID(8, 0.5, 2);

  if(rate != 1){
    Drive.drivePID.changePID(8/rate, 0.5/rate, 2/rate);
  }

  if(slowdown){
    Drive.drivePID.changePID(5/rate, 0.3125/rate, 1.25/rate);
  }

  Drive.DesPower = power;             //set power
  Drive.desiredAng = turnAngle*1.1;  //convert encoder to angle *8.44
  Drive.desiredPos = drivePos;        //set drive pos
}

void setArm(int power, int pos){
  Arm.DesPower = power;       //set arm powr/ pos
  Arm.desiredPos = pos;
}

void setSwivel(int power, int pos){
  Intake.swivelSpeed = power;
  Intake.swiveldesPos = pos;
}

void setRoller(int power, int pos){
  Intake.rollerReset();
  Intake.rollerSpeed = power;
  Intake.rollerRot = pos;
  Intake.rollerState = true;
}

void pickUp(int times, int frequency = 800, float dist = 8.5){
  int i;
  for(i = 0; i < times; i++){
    setDrive(90, dist, 0);
    wait(frequency);  //900 5.5secs 700 4 secs 800 
  }
}

void moveTo(float x, float y, int power, bool slowdown = false){
  _line followLine;
  
  //set points for the line the robot has to follow
  followLine.p1.x = Drive.sPos.x; //start
  followLine.p1.y = Drive.sPos.y;

  followLine.p2.x = x;  //end
  followLine.p2.y = y;

  //set desired pos
  Drive.desPos.x = x;
  Drive.desPos.y = y;

  //find distance and lenght of line
  Drive.followDist = lineLength(followLine);
  Drive.followAng = lineAngle(followLine);
}



