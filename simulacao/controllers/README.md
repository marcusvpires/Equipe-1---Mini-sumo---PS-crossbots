# Mini sumo - C++

## Dados coletados

### senssor ultrassônico:

- Margem de erro = A140 [ +-70 ]
- Ponto mais longe possível = 1035 [ +- 165 ] ( 870 / 1200 )

> Radar

Velocidade recomendada de giro = 7.0 // -7.0

```
  DistanceSensor *_us[5];
  char usn[5][8] = { "us0", "us1", "us2", "us3", "us4" };
  for (int i = 0; i < 5; i++) {
    _us[i] = robot->getDistanceSensor(usn[i]); 
    _us[i]->enable(TIME_STEP);
  }

  ---------------------fora de main----------------------

  int radar(DistanceSensor *_us[5], ) {
    // valores do senssor ultrassônico
    double usv[5];
    for (int i = 0; i < 5; i++) {
      usv[i] = _us[i]->getValue();
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
    if (m_usv < 1200 && m_usv > 1)
      std::cout << m_usn << " >> " << m_usv << "\n";
  }

  ---------------------dentro do while----------------------

  radar(_us, usn);

  roda_direita->setVelocity(7);
  roda_esquerda->setVelocity(-7);

```

###  senssor infra:

- máximo preto = 3577.16
- máximo branco = 4123.74

### teste de aceleração:
não foi verificado nenhuma mudança mais o sistema crachou bastante nos testes, principalmente em a partir de 100

> quando foi colocado dois carros, um ao lado do outro, sendo um com o método de aceleração calibrado em 100 e outro sem nada, o carro com o método de aceleração adquiriu uma vantagem considerável

Valores coletados:
| loop | tempo  |
|------|--------|
| 0020 | 3.104s | 
| 0030 | 3.104s | 
| 0040 | 3.104s | 
| 0050 | 3.104s | 
| 0060 | 3.104s | 
| 0070 | 3.104s | 
| 0100 | 3.104s | 
| 0200 | 3.104s | 
| 1000 | 3.104s | 

> método de aceleração:
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