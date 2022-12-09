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
    MOVER,
    RECUAR,
    BUSCAR,
    ATACAR,
    PARAR,
    FORCAR
};
enum MODO
{
    MODO_1,
    MODO_2,
    MODO_3, 
    MODO_4
};
int state = MOVER;
int last_state = 0;
float max_speed = 20;

void recuar(int dir, float tempo_inicio_tarefa, float tempo, double *speed_l, double *speed_r);
int verifica_linha(double right_ground_ir_value, double left_ground_ir_value, int dir);
void verifica_oponente(double *lidar_value, int *menor_lidar);
double speed_converter(int speed, int real_speed);
void estrategia(int controle, float tempo_inicio_tarefa, float tempo, double *speed_l, double *speed_r);

int main()
{
    wb_robot_init(); /* necessary to initialize webots stuff */
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
    double right_ground_ir_value = 0, left_ground_ir_value = 0, speed_l = 0, speed_r = 0, real_speed_l = 0, real_speed_r = 0;
    double initial_velocity = 1;
    float tempo, tempo_inicio_tarefa;
    int dir, controle = MODO_4;
    int menor_lidar;
    while (wb_robot_step(TIME_STEP) != -1)
    {
        tempo = wb_robot_get_time();
        for (int i = 0; i < 7; i++)
        {
            lidar_value[i] = wb_distance_sensor_get_value(lidar[i]);
        }
        // sensor de linha ( 952 para preto e 1024 para branco )
        right_ground_ir_value = wb_distance_sensor_get_value(right_ground_ir);
        left_ground_ir_value = wb_distance_sensor_get_value(left_ground_ir);
        last_state = state;
        dir = verifica_linha(right_ground_ir_value, left_ground_ir_value, dir);
        verifica_oponente(lidar_value, &menor_lidar);
        if ((state == RECUAR && last_state != RECUAR) || (state == MOVER && last_state != MOVER))
            tempo_inicio_tarefa = wb_robot_get_time();
        switch (state)
        {
            case MOVER:
                estrategia(controle, tempo_inicio_tarefa, tempo, &speed_l, &speed_r);
                break;
            case RECUAR:
                recuar(dir, tempo_inicio_tarefa, tempo, &speed_l, &speed_r);
                break;
            case ATACAR:
                // 7 sensores, 3 para esquerda em diferentes angulos, 3 para direita, um centralizado. 
                // Se for o sensor central que capta a menor distancia, faz um movimento retilineo, caso não
                // Faz o movimento mais curvo ou menos curvo em direção ao oponente quanto mais ou menos angulado for
                // o sensor. 
                speed_l = max_speed * (1 + ((menor_lidar - 3) / 1.5)); 
                speed_r = max_speed * (1 - ((menor_lidar - 3) / 1.5));
                break;
            case PARAR:
                speed_l = 0;
                speed_r = 0;
                break;
            case BUSCAR:
                if (dir == LEFT)
                {
                    speed_l = 10;
                    speed_r = 15;
                }
                else
                {
                    speed_l = 15;
                    speed_r = 10;
                }
                break;
            case FORCAR:
                speed_l = 40;
                speed_r = 40;
                break;
        }
        real_speed_l = speed_converter(speed_l, real_speed_l);
        real_speed_r = speed_converter(speed_r, real_speed_r);
        wb_motor_set_velocity(left_motor, real_speed_l);
        wb_motor_set_velocity(right_motor, real_speed_r);
    }
    return 0;
}

//! Funcao de conversao de velocidade, após o calculo de velocidade a partir de qual lidar tem a menor distancia,
// essa funcao faz com que a aceleraçao ou desaceleracao ocorra de maneira progressiva, recebendo a velocidade 
// anterior do motor e a velocidade calculada pela formula no ataque.
// Caso a velocidade calculada seja maior que a velocidade anterior somada a 5, ou menor que a velocidade anterior 
// subtraida em 5, aumentará ou diminuirá somente em 5 a velocidade. Caso não, colocará a velocidade calculada.
/*!
  \param speed velocidade calculada no estado ataque
  \param real_speed velocidade anterior real colocada no motor
  \return velocidade a ser colocada no motor
*/
double speed_converter(int speed, int real_speed)
{
    if (speed > real_speed + 5)
    {
        return (real_speed + 5);
    }
    else if (speed < real_speed - 5)
    {
        return (real_speed - 5);
    }
    else
    {
        return speed;
    }
}

//! Funcao de verificacao de oponente,
// verifica se algum lidar captou um sinal valido e coloca em modo ataque
// Retorna de informação qual lidar captou a menor distancia para definir o movimento de ataque
/*!
  \param lidar_value ponteiro para o vetor de leituras dos sensores lidar
  \param lidar_value ponteiro para qual sensor apresentou a menor distancia
*/
void verifica_oponente(double *lidar_value, int *menor_lidar)
{
    // Encontrar a menor distância
    double menor_dist = 950.0;
    *menor_lidar = -1;
    for (int i = 0; i < 7; i++)
    {
        if (lidar_value[i] < menor_dist && lidar_value[i] > 1)
        {
            menor_dist = lidar_value[i];
            *menor_lidar = i;
        }
    }
    if (menor_dist < 10 && *menor_lidar > 2 && *menor_lidar < 5)
        state = FORCAR;
    else if (menor_dist < 950)
        state = ATACAR;
}
//! Funcao de verificacao de linha, le qual sensor identificou a linha e indica que direcao 
// seguir para comecar o recuo
/*!
  \param dir um valor inteiro definido para a direcao
  \param right_ground_ir_value leitura do sensor de linha direito
  \param left_ground_ir_value leitura do sensor de linha esquerdo
*/
int verifica_linha(double right_ground_ir_value, double left_ground_ir_value, int dir)
{
    if (state != RECUAR)
    {
        // Borda detectada no sensor da esquerda
        if (left_ground_ir_value > 1000)
        {
            // Faz recuo pela direita
            state = RECUAR;
            dir = RIGHT;
        }
        // Borda detectada no sensor da direita
        else if (right_ground_ir_value > 1000)
        {
            // Faz recuo pela esquerda
            state = RECUAR;
            dir = LEFT;
        }
    }
    return dir;
}

//! Funcao de estrategia para primeira movimentacao
/*!
  \param dir um valor inteiro definido para a direcao
  \param speed_l referencia a velocidade do motor esquerdo
  \param speed_r referencia a velocidade do motor direito
*/
estrategia(int controle, float tempo_inicio_tarefa, float tempo, double *speed_l, double *speed_r)
{
    switch(controle)
    {
        case MODO_1:
            *speed_l = max_speed;
            *speed_r = max_speed;
        break;
        case MODO_2:
            *speed_l = 30;
            *speed_r = 15;
        break;
        case MODO_3:
            *speed_l = 15;
            *speed_r = 30;
        break;
        case MODO_4:
            if (tempo < (tempo_inicio_tarefa + 1))
            {
                *speed_l = max_speed;
                *speed_r = max_speed;
            }
            else  if (tempo < (tempo_inicio_tarefa + 1.5))
            {
                *speed_l = max_speed;
                *speed_r = 0;
            }
            else
            {
                *speed_l = max_speed;
                *speed_r = max_speed;
            }
    }
}

//! Funcao de recuo, da ré, depois gira para o lado oposto do sensor que detectou a borda e volta para o modo busca.
/*!
  \param dir um valor inteiro definido para a direcao
  \param tempo_inicio_tarefa float para o valor do tempo em que se iniciou o recuo
  \param tempo float para o tempo decorrido de simulacao
  \param speed_l referencia a velocidade do motor esquerdo
  \param speed_r referencia a velocidade do motor direito
*/
void recuar(int dir, float tempo_inicio_tarefa, float tempo, double *speed_l, double *speed_r)
{
    if (tempo < (tempo_inicio_tarefa + 0.3))
    {
        *speed_l = -max_speed;
        *speed_r = -max_speed;
    }
    else if (tempo < (tempo_inicio_tarefa + 0.7))
    {
        if (dir == RIGHT)
        {
            *speed_l = max_speed;
            *speed_r = -max_speed;
        }
        else
        {
            *speed_l = -max_speed;
            *speed_r = max_speed;
        }
    }
    else
    {
        state = BUSCAR;
    }
}