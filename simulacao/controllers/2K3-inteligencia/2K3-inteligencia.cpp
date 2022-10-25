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
  DistanceSensor *_us[5];
  char usn[5][8] = { "us0", "us1", "us2", "us3", "us4" };
  for (int i = 0; i < 5; i++) {
    _us[i] = robot->getDistanceSensor(usn[i]); 
    _us[i]->enable(TIME_STEP);
  }
  
  // Definição de variáveis
  // double maior_infrav = 0.0;
  // std::string maior_infran = "";
  // int concluido = 1;

  std::string tarefa = "radar";
  float tempo_inicio_tarefa = 0;
  float tempo_tolerancia = 0;

  //-------------------------------------------------------//
  
  while(robot->step(TIME_STEP) != -1) {

    const float tempo = float(robot->getTime());

    // valores do senssor ultrassônico
    double usv[5];
    for (int i = 0; i < 5; i++) {
      usv[i] = _us[i]->getValue();
      std::cout << usv[i] << " - " << usn[i] << "\n";
    }
    // Definição de variáveis
    double m_usv = 1500.0;
    std::string m_usn = "";
    // encontra a menor distância
    for (int i = 0; i < 5; i++) {
      if (usv[i] < m_usv && usv[i] > 1) {
        m_usv = usv[i];
        m_usn = usn[i];
      }
      // std::cout << usn[i] << " - " << usv[i] << "\n";
    }

    // Verifica se a menor distância está dentro da arena
    if (m_usv < 1200 && m_usv > 1) {
      if (m_usn == "us3") {
        std::cout << "girar_us2_centro iniciado" << "\n";
        tarefa = "girar_us3_centro";
        tempo_inicio_tarefa = tempo;
      }
      std::cout << m_usn << " >> " << m_usv << "\n";
    }

    if (tarefa == "radar") {
      std::cout << "radar" << "\n";
      roda_direita ->setVelocity(7);
      roda_esquerda->setVelocity(-7);
    } else if (tarefa == "seguir") {
      std::cout << "seguir" << "\n";
      roda_direita ->setVelocity(MAX_SPEED*0.9);
      roda_esquerda->setVelocity(MAX_SPEED*0.9);  
      if (m_usn == "us2") {
        tempo_tolerancia = tempo;
      } else if ((tempo - tempo_tolerancia) > 1.5) {
        std::cout << "iniciando radar" << "\n";
        tarefa = "radar";
        tempo_inicio_tarefa = tempo;
      }
    } else if (tarefa == "girar_us3_centro") {
      if ((tempo - tempo_inicio_tarefa) > 0.18) {
        std::cout << "girar_us2_centro concluido" << "\n";
        tarefa = "radar";
        tempo_inicio_tarefa = tempo;
      } else if (m_usn == "us2") {
        tarefa = "seguir";
        tempo_inicio_tarefa = tempo;
        tempo_tolerancia = tempo;
      } else {
        roda_direita ->setVelocity(-MAX_SPEED*0.9);
        roda_esquerda->setVelocity(MAX_SPEED*0.9);
      }
    }
    
    /*
    const double infravL = infraL->getValue();
    const double infravR = infraR->getValue();
    */

    /* 
    if (infravL > infravR) {
      maior_infrav = infravL;
      maior_infran = "infravL";
    } else {
      maior_infrav = infravR;
      maior_infran = "infravR";
    }
    */

    std::cout << "-----------------------------------------------" << "\n";
    
  }
  
//----------------------NÃO ALTERAR----------------------//
  delete robot;
//-------------------------------------------------------//

  return 0;
}
