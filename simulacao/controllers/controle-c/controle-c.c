//----------------------NÃO ALTERAR----------------------//
#include <stdio.h>
#include <string.h>
#include <webots/distance_sensor.h>
#include <webots/robot.h>
#include <webots/motor.h>
#include <webots/device.h>

#define TIME_STEP 32
#define MAX_SPEED 12
//-------------------------------------------------------//

int main()
{
  //----------------------NÃO ALTERAR----------------------//
  wb_robot_init();

  int num = 0, tarefa = 0;

  //Definindo os motores
  WbDeviceTag roda_esquerda, roda_direita;
  roda_esquerda = wb_robot_get_device("left wheel motor"); //Motor esquerdo
  roda_direita = wb_robot_get_device("right wheel motor"); //Motor direito
  wb_motor_set_position(roda_esquerda, INFINITY);
  wb_motor_set_position(roda_direita, INFINITY);
  wb_motor_set_velocity(roda_esquerda, 0.0);
  wb_motor_set_velocity(roda_direita, 0.0);
  
  //Definição dos sensores infravermelhos inferiores
  WbDeviceTag infraL, infraR; 
  infraL = wb_robot_get_device("ds10"); //Sensor infravermelho esquerdo
  infraR = wb_robot_get_device("ds9"); //Sensor infravermelho direito
  wb_distance_sensor_enable(infraL, TIME_STEP);
  wb_distance_sensor_enable(infraR, TIME_STEP);
  
  //Definição dos sensores ultrassônicos
  WbDeviceTag us01, us02, us03;
  us01 = wb_robot_get_device("us1"); //Sensor ultrassônico lateral esquerdo
  us02 = wb_robot_get_device("us2"); //Sensor ultrassônico frontal
  us03 = wb_robot_get_device("us3"); //Sensor ultrassônico lateral direito
  wb_distance_sensor_enable(us01, TIME_STEP);
  wb_distance_sensor_enable(us02, TIME_STEP);
  wb_distance_sensor_enable(us03, TIME_STEP);
  //-------------------------------------------------------//
  
  while(wb_robot_step(TIME_STEP) != -1) //Insira dentro desse laço while o código que rodará continuamente (estilo loop do arduino)
  {

    const double _infraL = wb_distance_sensor_get_value(infraL);
    const double _infraR = wb_distance_sensor_get_value(infraR);
  
    
    // Verificação de sensores

    // fim da arena
    if (tarefa != 1) {
      if (_infraL > 2000 || _infraR > 2000) {
        tarefa = 1;
      }
    }

    // Execução de tarefas

    // busca
    if (tarefa == 0) {
      wb_motor_set_velocity(roda_direita, 10.0);
      wb_motor_set_velocity(roda_esquerda, 10.0);
    } 
    
    // volta
    if (tarefa == 1 && num < 20) {
      wb_motor_set_velocity(roda_direita, 10.0);
      wb_motor_set_velocity(roda_esquerda, -10.0);
      num += 1;
      
    } else {
      num = 0;
      tarefa = 0;
    }

  }
  
  //----------------------NÃO ALTERAR----------------------// 
  wb_robot_cleanup();
  //-------------------------------------------------------//
   
  return 0;
}
