/*
 * Copyright 1996-2020 Cyberbotics Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Description:  A really simple controller which moves the MyBot robot
 *               and avoids the walls
 */

#include <stdio.h>
#include <webots/distance_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>

#define SPEED 6
#define TIME_STEP 64

int main() {
  wb_robot_init(); /* necessary to initialize webots stuff */

  /* Get and enable the distance sensors. */
  WbDeviceTag ds0 = wb_robot_get_device("ds0");
  WbDeviceTag ds1 = wb_robot_get_device("ds1");
  WbDeviceTag right_ground_ir = wb_robot_get_device("right_ground_infrared");
  WbDeviceTag left_ground_ir = wb_robot_get_device("left_ground_infrared");
  wb_distance_sensor_enable(ds0, TIME_STEP);
  wb_distance_sensor_enable(ds1, TIME_STEP);
  wb_distance_sensor_enable(right_ground_ir, TIME_STEP);
  wb_distance_sensor_enable(left_ground_ir, TIME_STEP);

  /* get a handler to the motors and set target position to infinity (speed control). */
  WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
  WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);

  while (wb_robot_step(TIME_STEP) != -1) {
    double ds0_value = wb_distance_sensor_get_value(ds0);
    double ds1_value = wb_distance_sensor_get_value(ds1);

    // sensor de linha ( 952 para preto e 1024 para branco )
    double right_ground_ir_value = wb_distance_sensor_get_value(right_ground_ir);
    double left_ground_ir_value = wb_distance_sensor_get_value(left_ground_ir);

    double left_speed, right_speed;
    if (ds1_value > 500) {

      if (ds0_value > 500) {
        left_speed = -SPEED;
        right_speed = -SPEED / 2;
      } else {

        left_speed = -ds1_value / 100;
        right_speed = (ds0_value / 100) + 0.5;
      }
    } else if (ds0_value > 500) {
      left_speed = (ds1_value / 100) + 0.5;
      right_speed = -ds0_value / 100;
    } else {

      left_speed = SPEED;
      right_speed = SPEED;
    }

    printf("ds0: %f\n", ds0_value);

    wb_motor_set_velocity(left_motor, left_speed);
    wb_motor_set_velocity(right_motor, right_speed);
  }
  while (wb_robot_step(TIME_STEP) != -1) {
    double ds0_value = wb_distance_sensor_get_value(ds0);
    double ds1_value = wb_distance_sensor_get_value(ds1);

    // sensor de linha ( 952 para preto e 1024 para branco )
    double right_ground_ir_value = wb_distance_sensor_get_value(right_ground_ir);
    double left_ground_ir_value = wb_distance_sensor_get_value(left_ground_ir);

    double left_speed, right_speed;
    if (ds1_value > 500) {

      if (ds0_value > 500) {
        left_speed = -SPEED;
        right_speed = -SPEED / 2;
      } else {

        left_speed = -ds1_value / 100;
        right_speed = (ds0_value / 100) + 0.5;
      }
    } else if (ds0_value > 500) {
      left_speed = (ds1_value / 100) + 0.5;
      right_speed = -ds0_value / 100;
    } else {

      left_speed = SPEED;
      right_speed = SPEED;
    }

    printf("ds0: %f\n", ds0_value);
    printf("ds1: %f\n", ds1_value);

    wb_motor_set_velocity(left_motor, left_speed);
    wb_motor_set_velocity(right_motor, right_speed);
  }

  return 0;
}
