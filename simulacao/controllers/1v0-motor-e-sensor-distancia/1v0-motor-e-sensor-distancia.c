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
  WbDeviceTag lidar_1 = wb_robot_get_device("lidar 1");
  WbDeviceTag lidar_2 = wb_robot_get_device("lidar 2");
  WbDeviceTag lidar_3 = wb_robot_get_device("lidar 3");
  WbDeviceTag lidar_4 = wb_robot_get_device("lidar 4");
  WbDeviceTag lidar_5 = wb_robot_get_device("lidar 5");
  WbDeviceTag lidar_6 = wb_robot_get_device("lidar 6");
  WbDeviceTag lidar_7 = wb_robot_get_device("lidar 7");
  wb_distance_sensor_enable(lidar_1, TIME_STEP);
  wb_distance_sensor_enable(lidar_2, TIME_STEP);
  wb_distance_sensor_enable(lidar_3, TIME_STEP);
  wb_distance_sensor_enable(lidar_4, TIME_STEP);
  wb_distance_sensor_enable(lidar_5, TIME_STEP);
  wb_distance_sensor_enable(lidar_6, TIME_STEP);
  wb_distance_sensor_enable(lidar_7, TIME_STEP);

  WbDeviceTag right_ground_ir = wb_robot_get_device("right_ground_infrared");
  WbDeviceTag left_ground_ir = wb_robot_get_device("left_ground_infrared");
  wb_distance_sensor_enable(right_ground_ir, TIME_STEP);
  wb_distance_sensor_enable(left_ground_ir, TIME_STEP);

  /* get a handler to the motors and set target position to infinity (speed control). */
  WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
  WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);
  
  double initial_velocity = 1;

  while (wb_robot_step(TIME_STEP) != -1) {
    double lidar_1_value = wb_distance_sensor_get_value(lidar_1);
    double lidar_2_value = wb_distance_sensor_get_value(lidar_2);
    double lidar_3_value = wb_distance_sensor_get_value(lidar_3);
    double lidar_4_value = wb_distance_sensor_get_value(lidar_4);
    double lidar_5_value = wb_distance_sensor_get_value(lidar_5);
    double lidar_6_value = wb_distance_sensor_get_value(lidar_6);
    double lidar_7_value = wb_distance_sensor_get_value(lidar_7);

    // sensor de linha ( 952 para preto e 1024 para branco )
    double right_ground_ir_value = wb_distance_sensor_get_value(right_ground_ir);
    double left_ground_ir_value = wb_distance_sensor_get_value(left_ground_ir);
    
    if (initial_velocity < 60) {
      initial_velocity = initial_velocity * 2;
      if (initial_velocity > 60) {
        initial_velocity = 60;
      }
    }

    double left_speed, right_speed;
    left_speed = 60;
    right_speed = 60;

    wb_motor_set_velocity(left_motor, left_speed);
    wb_motor_set_velocity(right_motor, right_speed);
  }
  return 0;
}
