# Mini sumo - C++

## Dados coletados

### senssor ultrassônico:

|  chave                            | valor         |
|-----------------------------------|---------------|
| margem de erro                    | A140 [ +-70 ] |
| maior distância dentro da arena   | 870 // 1200   |
| maior velociddade de rotação      | 7.0 && -7.0   |
| distância no limbo                | 1500          |

> por algum motivo, quando o valor é medido em quanto ele tá girando, esse valor aumenta, então eu aumentei o valor para 1450. 

###  senssor infra:

|  local                           | maior valor   |
|----------------------------------|---------------|
| preto                            | 3577.16       |
| branco                           | 4123.74       |

### teste de aceleração:
não foi verificado nenhuma mudança mais o sistema crachou bastante nos testes, principalmente em a partir de 100

> quando foi colocado dois carros, um ao lado do outro, sendo um com o método de aceleração calibrado em 100 e outro sem nada, o carro com o método de aceleração adquiriu uma vantagem considerável

Valores coletados:
| loop | tempo  |
|------|--------|
| 0020 | 3.104s | 
| 0040 | 3.104s | 
| 0100 | 3.104s | 
| 1000 | 3.104s | 

> método para o claculo de aceleração:
```
  if (maior_infrav == 0 && concluido) { // finaliza o tempo quando sai da arena
    std::cout << "tempo: " << time << "\n";
    concluido = 0;
  }
  float aceleracao = 1000;
  for (float i = 0; i <= aceleracao; i++)
  {
    float velocidade = MAX_SPEED * i / aceleracao;
    roda_direita->setVelocity(velocidade);
    roda_esquerda->setVelocity(velocidade);
    if (concluido) {
      std::cout << i << " - Velocidade: " << velocidade << "\n";
    }
  }
```

# output
### tarefas

printa a tarefa a cada meio segundo

```
  // biblioteca #include <math.h>
  // printa a tarefa a cada meio segundo
  if (fmod(tempo, 0.5) == 0) {
    std::cout << "tarefa:" << tarefa << " // tempo (s) : " << tempo - tempo_inicio_tarefa << "\n";
  }
```

# radar

### define os senssores
```
  DistanceSensor *_us[5];
  char usn[5][8] = { "us0", "us1", "us2", "us3", "us4" };
  for (int i = 0; i < 5; i++) {
    _us[i] = robot->getDistanceSensor(usn[i]); 
    _us[i]->enable(TIME_STEP);
  }
``` 

### define veriáveis

```
  std::string tarefa = "radar";
  float tempo_inicio_tarefa = 0;
  float tempo_tolerancia = 0;
```

## dentro do loop while

### registra a menor distância
```
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
```

### ação quando a tarefa for radar

em quanto a tarefa for radar o robo roda com uma velocidade de no máximo 7

> se a velocidade for maior, o sensor pode não captar o robô devido ao tempo de ida e volta do som

```
  if (tarefa == "radar") {
    roda_direita ->setVelocity(7);
    roda_esquerda->setVelocity(-7);
  }
```

## radar

define uma tarefa para cada valor registrado no radar
```
  bool is_tarefa_de_ataque = false;
  // define a terefa dependendo do valor da menor distância
  if (m_usv < 1200 && m_usv > 1) {
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
```

## execução das tarefas do ataque

```
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
```
