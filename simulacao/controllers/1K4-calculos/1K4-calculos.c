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
#include <string.h> 

#define MAX_SPEED 47.6

#define NUM_SOM 5
static const char *radar_som_nomes[NUM_SOM] = {
  "left ultrasonic sensor", "front left ultrasonic sensor", "front ultrasonic sensor", "front right ultrasonic sensor",
  "right ultrasonic sensor"};

#define NUM_INFRA 12
static const char *radar_infra_nomes[NUM_INFRA] = {
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
  WbDeviceTag radar_som[5];
  for (i = 0; i < 5; ++i) {
    radar_som[i] = wb_robot_get_device(radar_som_nomes[i]);
    wb_distance_sensor_enable(radar_som[i], time_step);
  }

  // get and enable the infrared sensors
  WbDeviceTag radar_infra[12];
  for (i = 0; i < 12; ++i) {
    radar_infra[i] = wb_robot_get_device(radar_infra_nomes[i]);
    wb_distance_sensor_enable(radar_infra[i], time_step);
  }

  // get the motors and set target position to infinity (speed control)
  WbDeviceTag motor_esquerdo, motor_direito;
  motor_esquerdo = wb_robot_get_device("left wheel motor");
  motor_direito = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(motor_esquerdo, INFINITY);
  wb_motor_set_position(motor_direito, INFINITY);
  wb_motor_set_velocity(motor_esquerdo, 0.0);
  wb_motor_set_velocity(motor_direito, 0.0);

  int last_display_second = 0;
  double velocidade_direita = MAX_SPEED * 0.9;
  double velocidade_esquerda = MAX_SPEED * 0.9;
  double radar_infra_valores[12];

  // main loop
  while (wb_robot_step(time_step) != -1) {

    // dados dos sensores
    double maior_infra = 0.0;
    for (i = 0; i < 12; ++i) {
      radar_infra_valores[i] = wb_distance_sensor_get_value(radar_infra[i]);
      if (maior_infra < radar_infra_valores[i]) { maior_infra = i; }
    }

    if (strcmp(radar_infra_nomes[maior_infra], "front ultrasonic sensor") == 0) {
      velocidade_direita = MAX_SPEED * 0.9;
      velocidade_esquerda = MAX_SPEED * 0.9;
    } else if (strcmp(radar_infra_nomes[maior_infra], "rear left infrared sensor") == 0) {
      velocidade_direita = MAX_SPEED * 0.9;
      velocidade_esquerda = MAX_SPEED * -0.4;
    } else if (strcmp(radar_infra_nomes[maior_infra], "left infrared sensor") == 0) {
      velocidade_direita = MAX_SPEED * 0.9;
      velocidade_esquerda = MAX_SPEED * 0.1;
    } else if (strcmp(radar_infra_nomes[maior_infra], "front left infrared sensor") == 0) {
      velocidade_direita = MAX_SPEED * 0.9;
      velocidade_esquerda = MAX_SPEED * 0.4;
    } else if (strcmp(radar_infra_nomes[maior_infra], "front right infrared sensor") == 0) {
      velocidade_direita = MAX_SPEED * 0.4;
      velocidade_esquerda = MAX_SPEED * 0.9;
    } else if (strcmp(radar_infra_nomes[maior_infra], "right infrared sensor") == 0) {
      velocidade_direita = MAX_SPEED * 0.1;
      velocidade_esquerda = MAX_SPEED * 0.9;
    } else if (strcmp(radar_infra_nomes[maior_infra], "rear right infrared sensor") == 0) {
      velocidade_direita = MAX_SPEED * -0.4;
      velocidade_esquerda = MAX_SPEED * 0.9;
    } else if (strcmp(radar_infra_nomes[maior_infra], "rear infrared sensor") == 0) {
      velocidade_direita = MAX_SPEED * -0.5;
      velocidade_esquerda = MAX_SPEED * 0.9;
    }
    
    // print values
    printf("time = %f [s]\n", wb_robot_get_time())
    for (i = 0; i < 12; ++i)
      printf("%f - %s\n", radar_infra_valores[i], radar_infra_nomes[i]);

    wb_motor_set_velocity(motor_esquerdo, velocidade_esquerda);
    wb_motor_set_velocity(motor_direito, velocidade_direita);

  };

  wb_robot_cleanup();

  return EXIT_SUCCESS;
}