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

#include <webots/motor.h>
#include <webots/robot.h>

#include <webots/camera.h>
#include <webots/distance_sensor.h>
#include <webots/led.h>

#include <stdio.h>
#include <stdlib.h>

#define MAX_SPEED 47.6

#define NUM_SOM 5
static const char *radar_som[NUM_SOM] = {
  "left ultrasonic sensor", "front left ultrasonic sensor", "front ultrasonic sensor", "front right ultrasonic sensor",
  "right ultrasonic sensor"};

#define NUM_INFRA 12
static const char *radar_infra[NUM_INFRA] = {
  // turret sensors
  "rear left infrared sensor", "left infrared sensor", "front left infrared sensor", "front infrared sensor",
  "front right infrared sensor", "right infrared sensor", "rear right infrared sensor", "rear infrared sensor",
  // ground sensors
  "ground left infrared sensor", "ground front left infrared sensor", "ground front right infrared sensor",
  "ground right infrared sensor"};

int main(int argc, char **argv) {
  wb_robot_init();

  int time_step = (int)wb_robot_get_basic_time_step();
  int i;

  // get and enable the ultrasonic sensors
  WbDeviceTag v_radar_som[5];
  for (i = 0; i < 5; ++i) {
    v_radar_som[i] = wb_robot_get_device(radar_som[i]);
    wb_distance_sensor_enable(v_radar_som[i], time_step);
  }

  // get and enable the infrared sensors
  WbDeviceTag v_radar_infra[12];
  for (i = 0; i < 12; ++i) {
    v_radar_infra[i] = wb_robot_get_device(radar_infra[i]);
    wb_distance_sensor_enable(v_radar_infra[i], time_step);
  }

  // get the motors and set target position to infinity (speed control)
  WbDeviceTag motor_esquerdo, motor_direito;
  motor_esquerdo = wb_robot_get_device("left wheel motor");
  motor_direito = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(motor_esquerdo, INFINITY);
  wb_motor_set_position(motor_direito, INFINITY);
  wb_motor_set_velocity(motor_esquerdo, 0.0);
  wb_motor_set_velocity(motor_direito, 0.0);

  // store the last time a message was displayed
  int last_display_second = 0;

  // main loop
  while (wb_robot_step(time_step) != -1) {
    // display some sensor data every second
    // and change randomly the led colors
    int display_second = (int)wb_robot_get_time();
    if (display_second != last_display_second) {
      last_display_second = display_second;

      printf("time = %d [s]\n", display_second);
      for (i = 0; i < 5; ++i)
        printf("- ultrasonic sensor('%s') = %f [m]\n", radar_som[i],
               wb_distance_sensor_get_value(v_radar_som[i]));
      for (i = 0; i < 12; ++i)
        printf("- infrared sensor('%s') = %f [m]\n", radar_infra[i],
               wb_distance_sensor_get_value(v_radar_infra[i]));

      for (i = 0; i < 3; ++i)
        wb_led_set(leds[i], 0xFFFFFF & rand());
    }

    // simple obstacle avoidance algorithm
    // based on the front infrared sensors
    double speed_offset = 0.2 * (MAX_SPEED - 0.03 * wb_distance_sensor_get_value(v_radar_infra[3]));
    double speed_delta =
      0.03 * wb_distance_sensor_get_value(v_radar_infra[2]) - 0.03 * wb_distance_sensor_get_value(v_radar_infra[4]);
    wb_motor_set_velocity(motor_esquerdo, speed_offset + speed_delta);
    wb_motor_set_velocity(motor_direito, speed_offset - speed_delta);
  };

  wb_robot_cleanup();

  return EXIT_SUCCESS;
}