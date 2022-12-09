#include <math.h>
#include <stdio.h>
#include <webots/accelerometer.h>
#include <webots/distance_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>

#define TIME_STEP 1
float max_speed = 20;
enum STATE { SEARCH, LINE_R, LINE_L, ATTACK, FORCE };
int last_state, state = SEARCH, m_lidar;
double speed_2 = 0, speed_1 = 0, motor_1 = 0, motor_2 = 0;
double lidar_v[7], m_lidar_v, right_ir_v = 0, left_ir_v = 0;
double *accelerometer_v;
float tm, tm_start, tm_relative;

int main(int argc, const char *argv[]) {
  wb_robot_init();

  printf("\niniciando controller desenvolvimento\n");
  // argumentos definidos nas configurações do robô
  for (int i = 0; i < argc; i++) printf("\nargumento[%i]=%s\n", i, argv[i]);

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

  // acelerometro
  WbDeviceTag accelerometer = wb_robot_get_device("accelerometer");
  wb_accelerometer_enable(accelerometer, TIME_STEP);

  void check_line(double right_ir_v, double left_ir_v) {
    if ((state == SEARCH || state == ATTACK) &&
        (left_ir_v > 1000 || right_ir_v > 1000)) {
      tm_start = tm;
      if (left_ir_v > 1000)
        state = LINE_R;
      else
        state = LINE_L;
    }
  }

  int setSpeed(int speed, int motor) {
    if (speed > max_speed) speed = max_speed;
    if (speed > motor + 5) {
      return (motor + 5);
    } else if (speed < motor - 5) {
      return (motor - 5);
    } else {
      return speed;
    }
  }

  void setState(int new_state) {
    tm_start = tm;
    state = new_state;
  }

  while (wb_robot_step(TIME_STEP) != -1) {
    tm = wb_robot_get_time();

    // avalores do acelerometro [vetor (x, y, z)]
    accelerometer_v = wb_accelerometer_get_values(accelerometer);

    // valores dos lidares
    m_lidar_v = 1000;
    m_lidar = -1;
    for (int i = 0; i < 7; i++) {
      lidar_v[i] = wb_distance_sensor_get_value(lidar[i]);
      if (lidar_v[i] < m_lidar_v) {
        m_lidar_v = lidar_v[i];
        m_lidar = i;
      }
    }

    // sensor de linha ( 952 para preto e 1024 para branco )
    left_ir_v = wb_distance_sensor_get_value(left_ir);
    right_ir_v = wb_distance_sensor_get_value(right_ir);

    check_line(right_ir_v, left_ir_v);
    if (m_lidar_v < 10 && m_lidar >= -2 && m_lidar <= 4) setState(FORCE);
    if (m_lidar_v < 1000) setState(ATTACK);

    tm_relative = tm - tm_start;

    switch (state) {
      case SEARCH:
        speed_1 = max_speed / 2;
        speed_2 = max_speed / 2;
        break;
      case LINE_R:
        speed_1 = -max_speed;
        speed_2 = max_speed;
        if (tm_relative > 0.2) setState(SEARCH);
        break;
      case LINE_L:
        speed_1 = max_speed;
        speed_2 = -max_speed;
        if (tm_relative > 0.2) setState(SEARCH);
        break;
      case ATTACK:
        speed_1 = max_speed * (1 - (m_lidar - 3) / 1.5);
        speed_2 = max_speed * (1 + (m_lidar - 3) / 1.5);
        break;
      case FORCE:
        speed_1 = max_speed;
        speed_2 = max_speed;
        break;
    }

    motor_1 = setSpeed(speed_1, motor_1);
    motor_2 = setSpeed(speed_2, motor_2);

    wb_motor_set_velocity(right_motor, motor_1);
    wb_motor_set_velocity(left_motor, motor_2);
  }
  wb_robot_cleanup();
  return 0;
}