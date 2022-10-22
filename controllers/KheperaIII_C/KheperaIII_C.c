//----------------------NÃO ALTERAR----------------------//
#include <stdio.h>
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
    const double infraLV = wb_distance_sensor_get_value(infraL);
    const double infraLR = wb_distance_sensor_get_value(infraR);
    
    
    
    if ( infraLV > 2000 || infraLR > 2000) {
      if ( infraLV > infraLR ) {
        wb_motor_set_velocity(roda_direita, 0);
        wb_motor_set_velocity(roda_esquerda, 10);
      } else {
        wb_motor_set_velocity(roda_direita, 10);
        wb_motor_set_velocity(roda_esquerda, 0);
      }
    } else {
      wb_motor_set_velocity(roda_direita, 10);
      wb_motor_set_velocity(roda_esquerda, 10);
    }
  }
  
  //----------------------NÃO ALTERAR----------------------// 
  wb_robot_cleanup();
  //-------------------------------------------------------//
   
  return 0;
}
