//----------------------NÃO ALTERAR----------------------//
#include <iostream>
#include <webots/Robot.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/Motor.hpp>
#include <webots/Device.hpp>

#define TIME_STEP 32
#define MAX_SPEED 12

using namespace webots;

int main()
{
  Robot *robot = new Robot();
  
  // Definindo os motores
  Motor *roda_esquerda = robot->getMotor("left wheel motor"); //Motor esquerdo
  Motor *roda_direita = robot->getMotor("right wheel motor"); //Motor direito
  roda_esquerda->setPosition(INFINITY);
  roda_direita->setPosition(INFINITY);
  roda_esquerda->setVelocity(0.0);
  roda_direita->setVelocity(0.0);
  
  // Definição dos sensores infravermelhos inferiores
  DistanceSensor *infraL = robot->getDistanceSensor("ds10"); //Sensor infravermelho esquerdo
  DistanceSensor *infraR = robot->getDistanceSensor("ds9"); //Sensor infravermelho direito
  infraL->enable(TIME_STEP);
  infraR->enable(TIME_STEP);
  
  // Definição dos sensores ultrassônicos
  DistanceSensor *us01 = robot->getDistanceSensor("us1"); //Sensor ultrassônico lateral esquerdo
  DistanceSensor *us02 = robot->getDistanceSensor("us2"); //Sensor ultrassônico frontal
  DistanceSensor *us03 = robot->getDistanceSensor("us3"); //Sensor ultrassônico lateral direito
  us01->enable(TIME_STEP);
  us02->enable(TIME_STEP);
  us03->enable(TIME_STEP);
  
  // Definição de variáveis
  double maior_usv = 0.0;
  std::string maior_usn = "";
  double maior_infrav = 0.0;
  std::string maior_infran = "";
  int concluido = 1;

  //-------------------------------------------------------//
  
  while(robot->step(TIME_STEP) != -1)
  {
    const double usv01 = us01->getValue();
    const double usv02 = us02->getValue();
    const double usv03 = us03->getValue();

    const double infravL = infraL->getValue();
    const double infravR = infraR->getValue();

    const float time = float(robot->getTime());

    if (usv01 > usv02 && usv01 > usv03) {
      maior_usv = usv01;
      maior_usn = "usv01";
    } else if (usv02 > usv03) {
      maior_usv = usv02;
      maior_usn = "usv02";
    } else {
      maior_usv = usv03;
      maior_usn = "usv03";
    }

    if (infravL > infravR) {
      maior_infrav = infravL;
      maior_infran = "infravL";
    } else {
      maior_infrav = infravR;
      maior_infran = "infravR";
    }

    if (maior_usv && 10 < maior_usv && maior_usv < 870 ) {
      std::cout << "localizado: " << maior_usv << " - " <<  maior_usn << "\n";
    }

    roda_direita->setVelocity(4);
    roda_esquerda->setVelocity(-4);
    
  }
  
//----------------------NÃO ALTERAR----------------------//
  delete robot;
//-------------------------------------------------------//

  return 0;
}
