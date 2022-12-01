//----------------------NÃO ALTERAR----------------------//
#include <iostream>
#include <webots/Robot.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/Motor.hpp>
#include <webots/Device.hpp>
#include <math.h>

#define TIME_STEP 32
#define MAX_SPEED 12

using namespace webots;

int main()
{
  Robot *robot = new Robot();
  
  Motor *right_motor = robot->getMotor("right-motor"); //Motor esquerdo
  right_motor->setPosition(INFINITY);
  right_motor->setVelocity(0.0);

  while(robot->step(TIME_STEP) != -1) {  
    right_motor->setVelocity(4);
  }
  
  delete robot;

  return 0;
}
