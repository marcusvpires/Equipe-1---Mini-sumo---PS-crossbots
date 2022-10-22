##----------------------NÃO ALTERAR----------------------##
from controller import Robot, Motor, DistanceSensor

TIME_STEP = 32
MAX_SPEED = 12

robot = Robot()

#Definindo os motores
roda_esquerda = robot.getDevice("left wheel motor") #Motor esquerdo
roda_direita = robot.getDevice("right wheel motor") #Motor direito
roda_esquerda.setPosition(float('inf'))
roda_direita.setPosition(float('inf'))
roda_esquerda.setVelocity(0.0)
roda_direita.setVelocity(0.0)

#Definindo os sensores infravermelhos inferiores
infraL = robot.getDevice("ds10") #Sensor infravermelho inferior esquerdo
infraR = robot.getDevice("ds9") #Sensor infravermelho inferior direito
infraL.enable(TIME_STEP)
infraR.enable(TIME_STEP)

#Definindos os sensores ultrassônicos
us01 = robot.getDevice("us1") #Sensor ultrassônico lateral esquerdo
us02 = robot.getDevice("us2") #Sensor ultrassônico fronta
us03 = robot.getDevice("us3") #Sensor ultrassônico lateral direito
us01.enable(TIME_STEP)
us02.enable(TIME_STEP)
us03.enable(TIME_STEP)

##-------------------------------------------------------##
while robot.step(TIME_STEP) != -1: #Insira dentro desse laço while o código que rodará continuamente (estilo loop do arduino)
    
    pass


