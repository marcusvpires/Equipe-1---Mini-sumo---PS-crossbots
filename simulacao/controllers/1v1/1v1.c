#include <stdio.h>
#include <webots/distance_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>

#define SPEED 6
#define TIME_STEP 64

enum DIRECAO { LEFT, RIGHT };
enum STATE { ANDAR, ESCAPA_LINHA, PROCURA, ATACAR, PARAR, DELAY };
int state = ANDAR;
int last_state;
double left_speed = 0, right_speed = 0;
float tempo, tempo_inicio_tarefa;
int direcao;

void verifica_linha(double right_ir_v, double left_ir_v) {
  if (left_ir_v > 1000 || right_ir_v > 1000) {
    tempo_inicio_tarefa = tempo;
    state = ESCAPA_LINHA;

    if (left_ir_v > 1020) {
      printf("Borda esquerda detectada\n");
      direcao = RIGHT;
    } else if (right_ir_v > 1020) {
      printf("Borda direita detectada\n");
      tempo_inicio_tarefa = tempo;
      state = ESCAPA_LINHA;
      direcao = LEFT;
    }
  }
}

int main() {
  wb_robot_init();
  printf("Robo inicializado\n");

  // Vetores para os sensores Lidar
  char lidar_tag[7][8] = {"lidar 1", "lidar 2", "lidar 3", "lidar 4",
                          "lidar 5", "lidar 6", "lidar 7"};
  WbDeviceTag lidar[7];
  for (int i = 0; i < 7; i++) {
    lidar[i] = wb_robot_get_device(lidar_tag[i]);
    wb_distance_sensor_enable(lidar[i], TIME_STEP);
  }

  // sensores de linha
  WbDeviceTag right_ir = wb_robot_get_device("right_ground_infrared");
  WbDeviceTag left_ir = wb_robot_get_device("left_ground_infrared");
  wb_distance_sensor_enable(right_ir, TIME_STEP);
  wb_distance_sensor_enable(left_ir, TIME_STEP);

  // motores
  WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
  WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);

  double lidar_v[7];
  double right_ir_v = 0, left_ir_v = 0;

  while (wb_robot_step(TIME_STEP) != -1) {
    tempo = wb_robot_get_time();

    // valores dos lidares
    printf("lidares: ");
    for (int i = 0; i < 7; i++) {
      lidar_v[i] = wb_distance_sensor_get_value(lidar[i]);
      printf(" %d: %f", i + 1, lidar_v[i]);
    }
    printf("\n");

    // sensor de linha ( 952 para preto e 1024 para branco )
    left_ir_v = wb_distance_sensor_get_value(left_ir);
    right_ir_v = wb_distance_sensor_get_value(right_ir);

    printf("right_ir: %f; left_ir: %f; state: %d; tempo: %f\n", right_ir_v,
           left_ir_v, state, tempo);
    
    switch (state) {
      case ANDAR:
        left_speed = 10;
        right_speed = 10;
        break;
      case ESCAPA_LINHA:

        left_speed = 10;
        right_speed = 10;
        break;
      case ATACAR:
        left_speed = 10;
        right_speed = 10;
        break;
      case PARAR:
        left_speed = 10;
        right_speed = 10;
        break;
      case DELAY:
        left_speed = 10;
        right_speed = 10;
        break;
    }
    wb_motor_set_velocity(right_motor, right_speed);
    wb_motor_set_velocity(left_motor, left_speed);
  }
  return 0;
}
