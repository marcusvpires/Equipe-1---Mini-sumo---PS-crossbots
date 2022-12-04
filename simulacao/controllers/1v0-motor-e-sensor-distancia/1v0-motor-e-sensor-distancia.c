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
enum direcao
{
    LEFT,
    RIGHT
};
enum STATE
{
    ANDAR,
    RECUAR,
    PROCURA,
    ATACAR,
    PARAR, 
    DELAY
};
enum RECUO_STATE
{
    RE,
    RECUO_DELAY,
    ROTACIONAR,
    VOLTA_ANDAR,
};
enum ATAQUE_STATE
{
    MODO_1,
    MODO_2,
    MODO_3,
    MODO_4,
    MODO_5,
    MODO_6,
    MODO_7, 
    ATAQUE_DELAY
};
int last_ataque_state;
int ataque_state;
int recuo_state = RE;
int last_recuo_state;
int state = ANDAR;
int last_state;

void recuar(int dir, WbDeviceTag left_motor, WbDeviceTag right_motor, float tempo_inicio_tarefa, float tempo);
int verifica_linha(double right_ground_ir_value, double left_ground_ir_value, int dir);
void verifica_oponente(double *lidar_value);
void ataque (WbDeviceTag left_motor, WbDeviceTag right_motor, float tempo_inicio_tarefa, float tempo);

int main()
{
    wb_robot_init(); /* necessary to initialize webots stuff */
    printf("Robo inicializado\n");
    /* Get and enable the distance sensors. */
    // Vetores para os sensores Lidar
    char lidar_tag[7][8] = {"lidar 1", "lidar 2", "lidar 3", "lidar 4", "lidar 5", "lidar 6", "lidar 7"};
    WbDeviceTag lidar[7];
    for (int i = 0; i < 7; i++)
    {
        lidar[i] = wb_robot_get_device(lidar_tag[i]);
        wb_distance_sensor_enable(lidar[i], TIME_STEP);
    }

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
    double lidar_value[7];
    double right_ground_ir_value = 0, left_ground_ir_value = 0;
    double initial_velocity = 1;
    float tempo, tempo_inicio_tarefa;
    int dir;
    double left_speed = 0, right_speed = 0;
    while (wb_robot_step(TIME_STEP) != -1)
    {
        tempo = wb_robot_get_time();
        for (int i = 0; i < 7; i++)
        {
            lidar_value[i] = wb_distance_sensor_get_value(lidar[i]);
            printf("Lidar %d: %f\n", i+1, lidar_value[i]);
        }
        // sensor de linha ( 952 para preto e 1024 para branco )
        right_ground_ir_value = wb_distance_sensor_get_value(right_ground_ir);
        left_ground_ir_value = wb_distance_sensor_get_value(left_ground_ir);
        printf("State: %d   Tempo: %f\n", state, tempo);
        // printf("right_ground_ir_value = %f left_ground_ir_value = %f\n", right_ground_ir_value, left_ground_ir_value);
        switch (state)
        {
            case ANDAR:
                if(last_state == ATACAR)
                {
                    wb_motor_set_velocity(left_motor, 20);
                    wb_motor_set_velocity(right_motor, 20);
                    last_state = ANDAR;
                    // tempo_inicio_tarefa = wb_robot_get_time();
                    // state = DELAY;
                }
                wb_motor_set_velocity(left_motor, 10);
                wb_motor_set_velocity(right_motor, 10);
                dir = verifica_linha(right_ground_ir_value, left_ground_ir_value, dir);
                verifica_oponente(lidar_value);
                if (state == RECUAR || tempo == ATACAR)
                    tempo_inicio_tarefa = wb_robot_get_time();
                break;
            case RECUAR:
                recuar(dir, left_motor, right_motor, tempo_inicio_tarefa, tempo);
                break;
            case ATACAR:
                ataque(left_motor, right_motor, tempo_inicio_tarefa, tempo);
                break;
            case PARAR:
                printf("Parando!\n");
                wb_motor_set_velocity(left_motor, 0);
                wb_motor_set_velocity(right_motor, 0);
            case DELAY:
                if (tempo > (tempo_inicio_tarefa + 0.1)) // delay
                {
                    state = ANDAR;
                }
            break;

        }
    }
    return 0;
}
//! Funcao de ataque, analisa qual sensor captou a menor distancia e a partir disso roda uma estrategia
// cada estrategia esta numa maquina de estado ao qual estado vai ser setado dependendo de qual sensor captou a 
// menor distancia
/*!
  \param left_motor WbDeviceTag para o motor da roda esquerda
  \param right_motor WbDeviceTag para o motor da roda direita
  \param tempo_inicio_tarefa float para o valor do tempo em que se iniciou o recuo
  \param tempo float para o tempo decorrido de simulacao
*/
void ataque (WbDeviceTag left_motor, WbDeviceTag right_motor, float tempo_inicio_tarefa, float tempo)
{
    printf("ataque_state: %d, tempo: %f   tempo_inicio_tarefa: %f\n", ataque_state, tempo, tempo_inicio_tarefa);
    last_state = ATACAR;
    switch(ataque_state)
    {
        case MODO_1:
            printf("Modo 1!\n");
            wb_motor_set_velocity(left_motor, -20);
            wb_motor_set_velocity(right_motor, 20);
            last_ataque_state = ataque_state;
            ataque_state = ATAQUE_DELAY;
            break;
        case MODO_2:
            printf("Modo 2!\n");
            wb_motor_set_velocity(left_motor, -20);
            wb_motor_set_velocity(right_motor, 20);
            last_ataque_state = ataque_state;
            ataque_state = ATAQUE_DELAY;
            break;
        case MODO_3:
            printf("Modo 3!\n");
            wb_motor_set_velocity(left_motor, -20);
            wb_motor_set_velocity(right_motor, 20);
            last_ataque_state = ataque_state;
            ataque_state = ATAQUE_DELAY;
            state = ANDAR;
            break;
        case MODO_4:
            printf("Modo 4!\n");
            state = ANDAR;
            break;
        case MODO_5:
            printf("Modo 5!\n");
            wb_motor_set_velocity(left_motor, 20);
            wb_motor_set_velocity(right_motor, -20);
            last_ataque_state = ataque_state;
            ataque_state = ATAQUE_DELAY;
            state = ANDAR;
            break;
        case MODO_6:
            printf("Modo 6!\n");
            wb_motor_set_velocity(left_motor, 20);
            wb_motor_set_velocity(right_motor, -20);
            last_ataque_state = ataque_state;
            ataque_state = ATAQUE_DELAY;
            break;
        case MODO_7:
            printf("Modo 7!\n");
            wb_motor_set_velocity(left_motor, 20);
            wb_motor_set_velocity(right_motor, -20);
            last_ataque_state = ataque_state;
            ataque_state = ATAQUE_DELAY;
            break;
        case ATAQUE_DELAY:
            if(last_ataque_state == MODO_1 || last_ataque_state == MODO_6)
            {
                if (tempo > (tempo_inicio_tarefa + 0.6)) // delay
                {
                    state = ANDAR;
                }
            }
            else if(last_ataque_state == MODO_2 || last_ataque_state == MODO_5)
            {
                if (tempo > (tempo_inicio_tarefa + 0.3)) // delay
                {
                    state = ANDAR;
                }
            }
            else if(last_ataque_state == MODO_3 || last_ataque_state == MODO_7)
            {
                if (tempo > (tempo_inicio_tarefa + 0.15)) // delay
                {
                    state = ANDAR;
                }
            }
            break;
    }
}
//! Funcao de verificacao de oponente, 
// verifica se algum lidar captou um sinal valido e coloca em modo ataque
/*!
  \param lidar_value ponteiro para o vetor de leituras dos sensores lidar
*/
void verifica_oponente(double *lidar_value)
{
     // Encontrar a menor distância
    double menor_dist = 900.0;
    for (int i = 0; i < 7; i++) {
      if (lidar_value[i] < menor_dist && lidar_value[i] > 1) {
        menor_dist = lidar_value[i];
        state = ATACAR;
        ataque_state = i;
      }
    }
}
//! Funcao de verificacao de linha, le qual sensor identificou a linha e indica que direcao seguir para comecar o recuo
/*!
  \param dir um valor inteiro definido para a direcao
  \param right_ground_ir_value leitura do sensor de linha direito
  \param left_ground_ir_value leitura do sensor de linha esquerdo

*/
int verifica_linha(double right_ground_ir_value, double left_ground_ir_value, int dir)
{
    // Borda detectada no sensor da esquerda
    if (left_ground_ir_value > 1020)
    {
        // Faz recuo pela direita
        printf("Borda esquerda detectada\n");
        state = RECUAR;
        recuo_state = RE;
        dir = RIGHT;
    }
    // Borda detectada no sensor da direita
    else if (right_ground_ir_value > 1020)
    {
        // Faz recuo pela esquerda
        printf("Borda direita detectada\n");
        state = RECUAR;
        recuo_state = RE;
        dir = LEFT;
    }
    return dir;
}

//! Funcao de recuo, da ré e depois rotaciona
/*!
  \param dir um valor inteiro definido para a direcao
  \param left_motor WbDeviceTag para o motor da roda esquerda
  \param right_motor WbDeviceTag para o motor da roda direita
  \param tempo_inicio_tarefa float para o valor do tempo em que se iniciou o recuo
  \param tempo float para o tempo decorrido de simulacao
*/
void recuar(int dir, WbDeviceTag left_motor, WbDeviceTag right_motor, float tempo_inicio_tarefa, float tempo)
{
    // printf("recuo_state: %d, tempo: %f   tempo_inicio_tarefa: %f dir: %d\n", recuo_state, tempo, tempo_inicio_tarefa, dir);
    switch (recuo_state)
    {
        case RE:
            // printf("Ré, tempo: %f e tempo_inicio_tarefa %f\n", tempo, tempo_inicio_tarefa);
            wb_motor_set_velocity(left_motor, -10);
            wb_motor_set_velocity(right_motor, -10);
            last_recuo_state = RE;
            recuo_state = RECUO_DELAY;
            // recuo_state = PARAR;
            break;
        case ROTACIONAR:
            if (dir)
            {
                wb_motor_set_velocity(left_motor, -10);
                wb_motor_set_velocity(right_motor, 10);
                last_recuo_state = ROTACIONAR;
                recuo_state = RECUO_DELAY;
            }
            else
            {
                wb_motor_set_velocity(left_motor, 10);
                wb_motor_set_velocity(right_motor, -10);
                last_recuo_state = ROTACIONAR;
                recuo_state = RECUO_DELAY;
            }
            break;
        case RECUO_DELAY:
            if (tempo > (tempo_inicio_tarefa + 1.5)) // delay
            {
                state = ANDAR;
            }
            else if (tempo > (tempo_inicio_tarefa + 0.8) && last_recuo_state != ROTACIONAR)
            {
                recuo_state = ROTACIONAR;
            }
            break;
        // case VOLTA_ANDAR:
        //     printf("Voltando a andar!\n");
        //     return 1;
    }
}