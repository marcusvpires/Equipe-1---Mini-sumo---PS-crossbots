//----------------------NÃO ALTERAR----------------------//
import com.cyberbotics.webots.controller.Robot;
import com.cyberbotics.webots.controller.DistanceSensor;
import com.cyberbotics.webots.controller.Motor;

public class KheperaIII_JAVA 
{

  public static void main(String[] args) 
  {

    final int TIME_STEP = 32;
    final int MAX_SPEED = 12;
    
    final Robot robot = new Robot();
    
    //Definindo os motores/*
    final Motor rodaEsquerda = robot.getMotor("left wheel motor"); //Motor esquerdo
    final Motor rodaDireita = robot.getMotor("right wheel motor"); //Motor direito
    rodaEsquerda.setPosition(Double.POSITIVE_INFINITY);
    rodaDireita.setPosition(Double.POSITIVE_INFINITY);
    rodaEsquerda.setVelocity(0.0);
    rodaDireita.setVelocity(0.0);
    
    //Definindo os sensores infravermlhos
    final DistanceSensor infraL = robot.getDistanceSensor("ds10"); //Sensor infravermelho inferior esquerdo
    final DistanceSensor infraR = robot.getDistanceSensor("ds9"); //Sensor infravermelhor inferior direito
    infraL.enable(TIME_STEP);
    infraR.enable(TIME_STEP);
    
    //Definindo os sensores ultrassônicos
    final DistanceSensor us01 = robot.getDistanceSensor("us1"); //Sensor ultrassônico lateral esquerdo
    final DistanceSensor us02 = robot.getDistanceSensor("us2"); //Sensor ultrassônico frontal
    final DistanceSensor us03 = robot.getDistanceSensor("us3"); //Sensor ultrassônico lateral direito
    us01.enable(TIME_STEP);
    us02.enable(TIME_STEP);
    us03.enable(TIME_STEP);
//-------------------------------------------------------//    
 
    while (robot.step(TIME_STEP) != -1) //Insira dentro desse laço while o código que rodará continuamente (estilo loop do arduino)
    {
    }
    
  }
}
