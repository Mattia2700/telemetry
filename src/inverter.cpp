#include "inverter.h"

std::string inverter_filename(int j){
  switch (j) {
    case 0:
      return "INVERTER_L_TEMP";
    case 1:
      return "MOTOR_L_TEMP";
    case 2:
      return "SPEED_L";
    case 3:
      return "INVERTER_R_TEMP";
    case 4:
      return "MOTOR_R_TEMP";
    case 5:
      return "SPEED_R";
  }
}

int inverter_fields(FILE *buffer, int j) {
  switch (j) {
  case 0: case 4:
    fprintf(buffer, "timestamp,inverter_temp");
    break;
  case 1: case 5:
    fprintf(buffer, "timestamp,motor_temp");
    break;
  case 3: case 6:
    fprintf(buffer, "timestamp,speed");
    break;
  }
  return 0;
}

float convert_motor_temp(inverter_data_t *data) {
  int16_t m_temp = ((data->data_1 << 8) | data->data_0);
  float motor_temp = (float)((m_temp - 9393.9) / 55.1f);
  return motor_temp;
}

float convert_inverter_temp(inverter_data_t *data) {
  uint16_t i_temp = ((data->data_1 << 8) | data->data_0);
  float igbt_temp = (float)((i_temp - 15797) / 112.1f);
  return igbt_temp;
}

int16_t convert_speed(inverter_data_t *data) {
  int16_t raw_val = ((data->data_1 << 8) | data->data_0);
  int16_t rpm = 7000 * raw_val / 32767;
  return rpm;
}

int inverter_to_string(void *message, int id, FILE *inverter_files[]) {
  inverter_data_t *data = (inverter_data_t *)message;
  switch (data->reg_id) {
  case 0x49: // motor temp
    fprintf(inverter_files[CHECK_MOTOR_TEMP_SIDE(id)], "%" PRIu64 ",%f", data->_timestamp,
                   convert_motor_temp(data));
    fprintf(inverter_files[CHECK_MOTOR_TEMP_SIDE(id)], "\n");
    fflush(inverter_files[CHECK_MOTOR_TEMP_SIDE(id)]);
    break;
  case 0x4A: // inverter temp
    fprintf(inverter_files[CHECK_INV_TEMP_SIDE(id)], "%" PRIu64 ",%f", data->_timestamp,
                   convert_inverter_temp(data));
    fprintf(inverter_files[CHECK_INV_TEMP_SIDE(id)], "\n");
    fflush(inverter_files[CHECK_INV_TEMP_SIDE(id)]);
  case 0xA8: // speed
    fprintf(inverter_files[CHECK_SPEED_SIDE(id)], "%" PRIu64 ",%" PRId16, data->_timestamp,
                   convert_speed(data));
    fprintf(inverter_files[CHECK_SPEED_SIDE(id)], "\n");
    fflush(inverter_files[CHECK_SPEED_SIDE(id)]);
  }
  return 0;
}