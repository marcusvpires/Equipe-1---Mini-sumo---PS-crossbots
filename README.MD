# padawans crossbots processo seletivo

O projeto tem como intuito projetar um mini-sumo teórico, fazendo o projeto eletrônico, mecânico e simulando a programação no Webots. O “Gado” foi projetado para ser um robô com um peso estimado de 230,9g e dimensões de 100x100x55 mm

## PCB e esquemático:

A PCB foi desenvolvida de modo a incluir 2 headers de 3 vias para os sensores de linha, 7 headers de 5 vias para os sensores de distância, a ESP32, a ponte H e 4 bornes para a conexão da saída Step Down (alimentação 3.3V), da tensão da bateria (para o VM da ponte H) e dois bornes para conectar as saídas da ponte H aos motores.  Assim, a PCB tem por função conectar todos esses componentes entre si, principalmente os sensores e a ponte H ao microcontrolador. 

A PCB possui um plano de terra, a vantagem da criação de um plano de terra é diminuir a necessidade de criação de trilhas para tantos terras, menos cobre a ser corroído no processo de confecção da placa, garante que todos os terras estejam mais facilmente conectados e diminui quedas de tensão significativas, pois melhora a condução da corrente pela PCB.

![](videos\solid.jpg)

# KiCad projeto eletrônico 

![](videos\esquema.png)

![](videos\PCB.png)

## Programação:

A programação foi feita em linguagem C e simulada no Webots. Ela foi feita utilizando de base uma máquina de estados na qual o robô, caso identifique um robô adversário, rotacione de modo a apontar para para ele e seguir em sua direção para iniciar o ataque. A velocidade das rodas do robô dependerá da proximidade e direção do adversário, além da velocidade anterior, para que o robô acelere aos poucos, suavizando os movimentos e controlando a tração.

Caso o robô não esteja atacando e identifique uma linha, ele rotaciona para direção oposta à direção do sensor que detectou a linha, para desviar das extremidades do campo. E caso nem um adversário seja detectado ou nenhuma linha seja detectada, ele percorre o mapa. A mudança de estados vai depender do estado anterior, da identificação ou não do adversário e da identificação ou não de uma linha.

[video de demonstração](https://www.youtube.com/watch?v=tuTpYWDZn94&feature=youtu.be)

![](videos\fluxogramapng.png)

## componentes

### Motor N20
-   12 x 10 x 36 mm  
-   15g
-   Alimentação: 12V
-   500RPM
-   Corrente Stall: 1A
-   Quantidade: 2

> observação: caixa de redução muito frágil para o mini-sumo

### Ponte H TB6612FNG Duplo
-   21 x 19 x 3.6 mm
-   2g
-   Alimentação: 4.5 V a 13.5 V
-   Tensão Lógica: 2.7 V a 5.5 V
-   Corrente pico: 3.2A
-   Corrente contínua: 1.2A

### Bateria LiPo 3S
-   60 x 30 x 8 mm
-   45 g
-   11.1V 450mAh

### Regulador de tensão - Step Down - LM2596
-   50 x 23mm
-   12g
-   Tensão de entrada: 3.2 - 40V
-   Tensão de saída: 1.5 - 35V
-   Corrente de saída: 
	-   2A nominal
	-   3A máxima
-   Eficiência: 92%

### Microcontrolador ESP32
-   27.5 x 51 x 7 mm
-   20g
-   Alimentação: 3.3V
-   Corrente máxima: 20-40 mA
-   Número de GPIOs: 25 (para a V1 30 pinos) 
-   Diversos periféricos
-   Compatível com a Arduino IDE
-   Diversas bibliotecas

### Sensor de distância VL53L0X
-   20.5 x 18 x 2.7 mm
-   1.5g
-   3 a 5V (Regulador integrado)
-   Laser
-   I2C
-   ToF
-   Quantidade: 7

### Sensor de linha TCRT5000
-   10.2 x 5.8 x 7 mm
-   1.7g
-   3.3V a 5V 
-   Infravermelho + Fototransistor
-   LM393
-   Quantidade: 2

### Rodas S15 StickyMAX
-   32 x 21 x 3 mm
-   23g
-   Alta aderência
-   Material do Cubo: Alumínio 6351-T6
-   Material do Pneu: Borracha de Silicone Ultra-Resistente
-   Quantidade: 2

## Escolha dos componentes:

O motor N20 foi escolhido por que além de ser um motor comum e acessível, tinha o modelo disponível no Solid para a realização da modelagem 3D. Os sensores LIDAR VL53L0X são acessíveis no mercado, comumente utilizados em projetos de mini-sumo e menos afetados por ruído comparado aos  sensores de ultrassom. O uso de 7 sensores de distância foi para possibilitar uma captação melhor do oponente. A ESP32, além de ser mais acessível e comum tem mais GPIO e periféricos que o arduíno nano/uno, tem muita biblioteca compatível e tem um tamanho razoável para o projeto. O módulo step down LM2596 é um componente comum e acessível no mercado, por isso foi selecionado como regulador de tensão. A ponte H TB6612FNG  por disponibilizar em apenas um módulo o controle para dois motores, o que gera uma economia de espaço e de número de componentes a serem adquiridos.

## Mecânica:

A carcaça do robô foi projetada com as dimensões de 100x100x55 mm e material Abs( plástico parecido com o usado em impressão 3d). Foi 1 furo para cada roda( com as dimensões da mesma), feito 7 furos de sensores a laser, dos quais 5 na frente e 1 em cada lateral, e 2 furos para os sensores de linha na parte da frente. Na carcaça foi implementado 4 suportes logo acima do motor e 2 suportes no chão do robô, com o objetivo de colocar o step down em cima do motor e a pcb em pé. Ao fim foi projetado uma tampa de acrílico que cobre toda parte do robô ( com exceção da rampa), totalizando 55mm de altura do robo.