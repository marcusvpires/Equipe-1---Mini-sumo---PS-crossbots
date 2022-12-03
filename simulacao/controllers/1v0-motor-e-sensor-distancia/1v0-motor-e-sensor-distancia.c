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
enum direcao {LEFT, RIGHT};
enum STATE {ANDAR, RECUAR, PROCURA, ATACA};
enum RECUO_STATE {RE, DELAY, ROTACIONAR, VOLTA_ANDAR, PARAR};
int recuo_state = RE;

int recuar(int dir, WbDeviceTag left_motor, WbDeviceTag right_motor, float tempo_inicio_tarefa, float tempo);

int main() {
    wb_robot_init(); /* necessary to initialize webots stuff */
    printf ("Robo inicializado\n");
    /* Get and enable the distance sensors. */
    char lidar_tag[7][8] = {"lidar 1", "lidar 2", "lidar 3", "lidar 4", "lidar 5", "lidar 6", "lidar 7"};
    WbDeviceTag lidar[7];
    for (int i=0; i<7; i++)
    {
        lidar[i] = wb_robot_get_device(lidar_tag[i]);
        wb_distance_sensor_enable(lidar[i], TIME_STEP);
    }
    // WbDeviceTag lidar_1 = wb_robot_get_device("lidar 1");
    // WbDeviceTag lidar_2 = wb_robot_get_device("lidar 2");
    // WbDeviceTag lidar_3 = wb_robot_get_device("lidar 3");
    // WbDeviceTag lidar_4 = wb_robot_get_device("lidar 4");
    // WbDeviceTag lidar_5 = wb_robot_get_device("lidar 5");
    // WbDeviceTag lidar_6 = wb_robot_get_device("lidar 6");
    // WbDeviceTag lidar_7 = wb_robot_get_device("lidar 7");
    // wb_distance_sensor_enable(lidar_1, TIME_STEP);
    // wb_distance_sensor_enable(lidar_2, TIME_STEP);
    // wb_distance_sensor_enable(lidar_3, TIME_STEP);
    // wb_distance_sensor_enable(lidar_4, TIME_STEP);
    // wb_distance_sensor_enable(lidar_5, TIME_STEP);
    // wb_distance_sensor_enable(lidar_6, TIME_STEP);
    // wb_distance_sensor_enable(lidar_7, TIME_STEP);

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
    double lidar_1_value, lidar_2_value, lidar_3_value, lidar_4_value, lidar_5_value, lidar_6_value, lidar_7_value;
    double lidar_value[7];
    double right_ground_ir_value = 0, left_ground_ir_value = 0;
    double initial_velocity = 1;
    float tempo, tempo_inicio_tarefa;
    int dir; 
    int state = ANDAR;
    double left_speed = 0, right_speed = 0;
    while (wb_robot_step(TIME_STEP) != -1) 
    {
        tempo = wb_robot_get_time();
        for (int i=0; i<7; i++)
        {
            lidar_value[i] = wb_distance_sensor_get_value(lidar[i]);
        }
        // lidar_1_value = wb_distance_sensor_get_value(lidar_1);
        // lidar_2_value = wb_distance_sensor_get_value(lidar_2);
        // lidar_3_value = wb_distance_sensor_get_value(lidar_3);
        // lidar_4_value = wb_distance_sensor_get_value(lidar_4);
        // lidar_5_value = wb_distance_sensor_get_value(lidar_5);
        // lidar_6_value = wb_distance_sensor_get_value(lidar_6);
        // lidar_7_value = wb_distance_sensor_get_value(lidar_7);

        // sensor de linha ( 952 para preto e 1024 para branco )
        right_ground_ir_value = wb_distance_sensor_get_value(right_ground_ir);
        left_ground_ir_value = wb_distance_sensor_get_value(left_ground_ir);
        // printf("right_ground_ir_value = %f left_ground_ir_value = %f\n", right_ground_ir_value, left_ground_ir_value);
        switch(state)
        {
            case ANDAR:
                // if (initial_velocity < 60) {
                //     initial_velocity = initial_velocity * 2;
                //     if (initial_velocity > 60) {
                //             initial_velocity = 60;
                //         }
                // }
                wb_motor_set_velocity(left_motor, 10);
                wb_motor_set_velocity(right_motor, 10);
                // Borda detectada no sensor da esquerda
                if (left_ground_ir_value > 1020) {
                    // Faz recuo pela direita
                    // printf("Borda esquerda detectada\n");
                    state = RECUAR;
                    recuo_state = RE;
                    tempo_inicio_tarefa = wb_robot_get_time();
                    dir = RIGHT;
                }
                // Borda detectada no sensor da direita
                else if (right_ground_ir_value > 1020) {
                    // Faz recuo pela esquerda
                    // printf("Borda direita detectada\n");
                    state = RECUAR;
                    recuo_state = RE;
                    tempo_inicio_tarefa = wb_robot_get_time();
                    dir = LEFT;
                }
                break;
            case RECUAR:
                if(recuar(dir, left_motor, right_motor, tempo_inicio_tarefa, tempo) == 1)
                    state = ANDAR;
                break;
        }    
    }
  return 0;
}

int recuar(int dir, WbDeviceTag left_motor, WbDeviceTag right_motor, float tempo_inicio_tarefa, float tempo)
{
    switch(recuo_state)
    {
        case RE:
            printf("Ré\n");
            wb_motor_set_velocity(left_motor, -5);
            wb_motor_set_velocity(right_motor, -5);
            recuo_state = DELAY;
            // recuo_state = PARAR;
            break;
        case ROTACIONAR:
            printf("Rotacionando\n");
            if (dir == LEFT) {
                wb_motor_set_velocity(left_motor, -5);
                wb_motor_set_velocity(right_motor, 5);
                recuo_state = DELAY;
            }
            else {
                wb_motor_set_velocity(left_motor,   5);
                wb_motor_set_velocity(right_motor, -5);
                recuo_state = DELAY;
            }
            break;
        case DELAY:
            printf("Delay\n");
            if (tempo > tempo_inicio_tarefa + 0.9) // delay
                recuo_state = VOLTA_ANDAR;
            else if (tempo > tempo_inicio_tarefa + 0.3)
                recuo_state = ROTACIONAR;
            break;
        case VOLTA_ANDAR:
            return 1;
        case PARAR:
            wb_motor_set_velocity(left_motor, 0);
            wb_motor_set_velocity(right_motor, 0);
            break;
    }
}