//----------------------NÃO ALTERAR----------------------//
#include <iostream>
#include <webots/Robot.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/Motor.hpp>
#include <webots/Device.hpp>
#include <math.h>

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
    const int tempo_int = static_cast<int>(tempo * 10);

    // valores do senssor ultrassônico
    double usv[5];
    for (int i = 0; i < 5; i++) {
      usv[i] = _us[i]->getValue();
    }
    // encontra a menor distância
    double m_usv = 1500.0;
    std::string m_usn = "";
    for (int i = 0; i < 5; i++) {
      if (usv[i] < m_usv && usv[i] > 1) {
        m_usv = usv[i];
        m_usn = usn[i];
        break;
      }
      // std::cout << usn[i] << " - " << usv[i] << "\n";
    }

    // ----------------------- ataque ----------------------- //
    bool is_tarefa_de_ataque = false;
    // define a terefa dependendo do valor da menor distância
    if (m_usv < 1450 && m_usv > 1) {
      is_tarefa_de_ataque = true;
      if (m_usn == "us3") {
        tarefa = "girar_us3_centro";
        tempo_inicio_tarefa = tempo;
      } else if (m_usn == "us1") {
        tarefa = "girar_us1_centro";
        tempo_inicio_tarefa = tempo;
      } else if (m_usn == "us4") {
        tarefa = "girar_us4_centro";
        tempo_inicio_tarefa = tempo;
      } else {
        tarefa = "girar_us0_centro";
        tempo_inicio_tarefa = tempo;
      }
    }

    // printa a tarefa a cada meio segundo
    if (tempo_int % 2 == 0) {
      std::cout << "tarefa:" << tarefa << " // tempo (s) : " << tempo - tempo_inicio_tarefa << " // us: " << m_usn << " - " << m_usv << "\n";
    }

    // radar >> roda buscando um valor dentro da arena de um usv
    if (tarefa == "radar") {
      roda_direita ->setVelocity(7);
      roda_esquerda->setVelocity(-7);
    // seguir >> acelera na direção do alvo
    } else if (tarefa == "seguir") {
      roda_direita ->setVelocity(MAX_SPEED*0.9);
      roda_esquerda->setVelocity(MAX_SPEED*0.9);  
      if (m_usn == "us2") {
        tempo_tolerancia = tempo;
      } else if ((tempo - tempo_tolerancia) > 1.5 /* delay aproximado para receber um valor de us */) {
        tarefa = "radar";
        tempo_inicio_tarefa = tempo;
      }
    // tarefas de ataque
    } else if (is_tarefa_de_ataque) {
      if (m_usn == "us2") {
        tarefa = "seguir";
        tempo_inicio_tarefa = tempo;
        tempo_tolerancia = tempo;
      } else if ((tempo - tempo_inicio_tarefa) > 0.18) {
        tarefa = "radar";
        tempo_inicio_tarefa = tempo;
      } else {
        if (tarefa == "girar_us3_centro" || tarefa == "girar_us4_centro") {
          roda_direita ->setVelocity(-MAX_SPEED*0.9);
          roda_esquerda->setVelocity(MAX_SPEED*0.9);
        } else {
          roda_direita ->setVelocity(MAX_SPEED*0.9);
          roda_esquerda->setVelocity(-MAX_SPEED*0.9);
        }
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
  }
  
//----------------------NÃO ALTERAR----------------------//
  delete robot;
//-------------------------------------------------------//

  return 0;
}
