%%----------------------NÃO ALTERAR----------------------%%

TIME_STEP = 32;
MAX_SPEED = 12;

%Definindo os motores
roda_esquerda = wb_robot_get_device('left wheel motor'); %Motor esquerdo
roda_direita = wb_robot_get_device('right wheel motor'); %Motor direito
wb_motor_set_position(roda_esquerda, Inf);
wb_motor_set_position(roda_direita, Inf);
wb_motor_set_velocity(roda_esquerda, 0.0);
wb_motor_set_velocity(roda_direita, 0.0);

%Definindo os sensores infravermelhos
infraL = wb_robot_get_device('ds10'); %Sensor infravermelho inferior esquerdo
infraR = wb_robot_get_device('ds9'); %Sensor infravermelho inferior direito
wb_distance_sensor_enable(infraL, TIME_STEP);
wb_distance_sensor_enable(infraR, TIME_STEP);

%Definindo os sensores ultrassônicos
us01 = wb_robot_get_device('us1'); %Sensor ultrassônico lateral esquerdo
us02 = wb_robot_get_device('us2'); %Sensor ultrassônico frontal
us03 = wb_robot_get_device('us3'); %Sensor ultrassônico lateral direito
wb_distance_sensor_enable(us01, TIME_STEP);
wb_distance_sensor_enable(us02, TIME_STEP);
wb_distance_sensor_enable(us03, TIME_STEP);

%%-------------------------------------------------------%%
while wb_robot_step(TIME_STEP) ~= -1 %Insira dentro desse laço while o código que rodará continuamente (estilo loop do arduino)
  
end