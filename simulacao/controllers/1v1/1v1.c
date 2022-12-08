#include <math.h>
#include <stdio.h>
#include <webots/distance_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>

#define TIME_STEP 1
enum STATE { SEARCH, LINE_R, LINE_L, ATTACK };
int last_state, state = SEARCH, m_lidar;
double speed_2 = 0, speed_1 = 0, motor_1 = 0, motor_2 = 0;
double lidar_v[7], m_lidar_v, right_ir_v = 0, left_ir_v = 0;
float tm, tm_start, tm_relative;

int main() {
  wb_robot_init();

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
    if (speed > motor + 5) {
      return (motor + 5);
    } else if (speed < motor - 5) {
      return (motor - 5);
    } else {
      return speed;
    }
  }

  int setState(int new_state) {
    tm_start = tm;
    state = new_state;
  }

  while (wb_robot_step(TIME_STEP) != -1) {
    tm = wb_robot_get_time();

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
    if (m_lidar_v < 1000) setState(ATTACK);

    tm_relative = tm - tm_start;
    printf("\nstate: %d; tm_relative: %f; lidar: %d\n", state, tm_relative,
           m_lidar + 1);

    switch (state) {
      case SEARCH:
        speed_1 = 15;
        speed_2 = 15;
        break;
      case LINE_R:
        speed_1 = -20;
        speed_2 = 20;
        if (tm_relative > 0.2) setState(SEARCH);
        break;
      case LINE_L:
        speed_1 = 20;
        speed_2 = -20;
        if (tm_relative > 0.2) setState(SEARCH);
        break;
      case ATTACK:
        speed_1 = 20 * (1 - ((m_lidar - 3) / 1.5));
        speed_2 = 20 * (1 + ((m_lidar - 3) / 1.5));
        break;
    }

    motor_1 = setSpeed(speed_1, motor_1);
    motor_2 = setSpeed(speed_2, motor_2);
    printf("1: %lf >> %lf;  2: %lf >> %lf\n", motor_1, speed_1, motor_2,
           speed_2);

    wb_motor_set_velocity(right_motor, motor_1);
    wb_motor_set_velocity(left_motor, motor_2);
  }
  return 0;
}
