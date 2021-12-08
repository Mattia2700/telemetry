#include "proto_deserializer.h"

void Imu::CheckFieldsSize(uint64_t maxSize)
{
	size_t size = 0;
	Erase(timestamp, maxSize);
	Erase(x, maxSize);
	Erase(y, maxSize);
	Erase(z, maxSize);
	Erase(scale, maxSize);
}

void Encoder::CheckFieldsSize(uint64_t maxSize)
{
	size_t size = 0;
	Erase(timestamp, maxSize);
	Erase(rads, maxSize);
	Erase(km, maxSize);
	Erase(rotations, maxSize);
}

void Steer::CheckFieldsSize(uint64_t maxSize)
{
	size_t size = 0;
	Erase(timestamp, maxSize);
	Erase(angle, maxSize);
}

void Pedals::CheckFieldsSize(uint64_t maxSize)
{
	size_t size = 0;
	Erase(timestamp, maxSize);
	Erase(throttle1, maxSize);
	Erase(throttle2, maxSize);
	Erase(brake_front, maxSize);
	Erase(brake_rear, maxSize);
}

void Inverter::CheckFieldsSize(uint64_t maxSize)
{
	size_t size = 0;
	Erase(timestamp, maxSize);
	Erase(temperature, maxSize);
	Erase(motor_temp, maxSize);
	Erase(torque, maxSize);
	Erase(speed, maxSize);
}

void Bms::CheckFieldsSize(uint64_t maxSize)
{
	size_t size = 0;
	Erase(timestamp, maxSize);
	Erase(temperature, maxSize);
	Erase(max_temperature, maxSize);
	Erase(min_temperature, maxSize);
	Erase(current, maxSize);
	Erase(voltage, maxSize);
	Erase(max_voltage, maxSize);
	Erase(min_voltage, maxSize);
	Erase(power, maxSize);
}

void Ecu::CheckFieldsSize(uint64_t maxSize)
{
	size_t size = 0;
	Erase(timestamp, maxSize);
	Erase(power_request_left, maxSize);
	Erase(power_request_right, maxSize);
}

void State::CheckFieldsSize(uint64_t maxSize)
{
	size_t size = 0;
	Erase(timestamp, maxSize);
	Erase(value, maxSize);
}

void Gps::CheckFieldsSize(uint64_t maxSize)
{
	size_t size = 0;
	Erase(timestamp, maxSize);
	Erase(msg_type, maxSize);
	Erase(time, maxSize);
	Erase(latitude, maxSize);
	Erase(longitude, maxSize);
	Erase(altitude, maxSize);
	Erase(fix, maxSize);
	Erase(satellites, maxSize);
	Erase(fix_state, maxSize);
	Erase(age_of_correction, maxSize);
	Erase(course_over_ground_degrees, maxSize);
	Erase(course_over_ground_degrees_magnetic, maxSize);
	Erase(speed_kmh, maxSize);
	Erase(mode, maxSize);
}

///////////////////////////////
//////////  CHIMERA  //////////
///////////////////////////////
void ChimeraDeserializer::Deserialize(devices::Chimera* dev)
{
	int size;
	size = dev->accel_size();
	for(int i = 0; i < size; i ++)
	{
		auto  accel_proto = dev->accel(i);
		this->accel.timestamp.push_back(accel_proto.timestamp());
		this->accel.x.push_back(accel_proto.x());
		this->accel.y.push_back(accel_proto.y());
		this->accel.z.push_back(accel_proto.z());
		this->accel.scale.push_back(accel_proto.scale());
	}
	this->accel.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->gyro_size();
	for(int i = 0; i < size; i ++)
	{
		auto  gyro_proto = dev->gyro(i);
		this->gyro.timestamp.push_back(gyro_proto.timestamp());
		this->gyro.x.push_back(gyro_proto.x());
		this->gyro.y.push_back(gyro_proto.y());
		this->gyro.z.push_back(gyro_proto.z());
		this->gyro.scale.push_back(gyro_proto.scale());
	}
	this->gyro.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->encoder_left_size();
	for(int i = 0; i < size; i ++)
	{
		auto  encoder_left_proto = dev->encoder_left(i);
		this->encoder_left.timestamp.push_back(encoder_left_proto.timestamp());
		this->encoder_left.rads.push_back(encoder_left_proto.rads());
		this->encoder_left.km.push_back(encoder_left_proto.km());
		this->encoder_left.rotations.push_back(encoder_left_proto.rotations());
	}
	this->encoder_left.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->encoder_right_size();
	for(int i = 0; i < size; i ++)
	{
		auto  encoder_right_proto = dev->encoder_right(i);
		this->encoder_right.timestamp.push_back(encoder_right_proto.timestamp());
		this->encoder_right.rads.push_back(encoder_right_proto.rads());
		this->encoder_right.km.push_back(encoder_right_proto.km());
		this->encoder_right.rotations.push_back(encoder_right_proto.rotations());
	}
	this->encoder_right.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->bms_lv_size();
	for(int i = 0; i < size; i ++)
	{
		auto  bms_lv_proto = dev->bms_lv(i);
		this->bms_lv.timestamp.push_back(bms_lv_proto.timestamp());
		this->bms_lv.temperature.push_back(bms_lv_proto.temperature());
		this->bms_lv.max_temperature.push_back(bms_lv_proto.max_temperature());
		this->bms_lv.min_temperature.push_back(bms_lv_proto.min_temperature());
		this->bms_lv.current.push_back(bms_lv_proto.current());
		this->bms_lv.voltage.push_back(bms_lv_proto.voltage());
		this->bms_lv.max_voltage.push_back(bms_lv_proto.max_voltage());
		this->bms_lv.min_voltage.push_back(bms_lv_proto.min_voltage());
		this->bms_lv.power.push_back(bms_lv_proto.power());
	}
	this->bms_lv.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->bms_hv_size();
	for(int i = 0; i < size; i ++)
	{
		auto  bms_hv_proto = dev->bms_hv(i);
		this->bms_hv.timestamp.push_back(bms_hv_proto.timestamp());
		this->bms_hv.temperature.push_back(bms_hv_proto.temperature());
		this->bms_hv.max_temperature.push_back(bms_hv_proto.max_temperature());
		this->bms_hv.min_temperature.push_back(bms_hv_proto.min_temperature());
		this->bms_hv.current.push_back(bms_hv_proto.current());
		this->bms_hv.voltage.push_back(bms_hv_proto.voltage());
		this->bms_hv.max_voltage.push_back(bms_hv_proto.max_voltage());
		this->bms_hv.min_voltage.push_back(bms_hv_proto.min_voltage());
		this->bms_hv.power.push_back(bms_hv_proto.power());
	}
	this->bms_hv.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->inverter_left_size();
	for(int i = 0; i < size; i ++)
	{
		auto  inverter_left_proto = dev->inverter_left(i);
		this->inverter_left.timestamp.push_back(inverter_left_proto.timestamp());
		this->inverter_left.temperature.push_back(inverter_left_proto.temperature());
		this->inverter_left.motor_temp.push_back(inverter_left_proto.motor_temp());
		this->inverter_left.torque.push_back(inverter_left_proto.torque());
		this->inverter_left.speed.push_back(inverter_left_proto.speed());
	}
	this->inverter_left.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->inverter_right_size();
	for(int i = 0; i < size; i ++)
	{
		auto  inverter_right_proto = dev->inverter_right(i);
		this->inverter_right.timestamp.push_back(inverter_right_proto.timestamp());
		this->inverter_right.temperature.push_back(inverter_right_proto.temperature());
		this->inverter_right.motor_temp.push_back(inverter_right_proto.motor_temp());
		this->inverter_right.torque.push_back(inverter_right_proto.torque());
		this->inverter_right.speed.push_back(inverter_right_proto.speed());
	}
	this->inverter_right.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->pedal_size();
	for(int i = 0; i < size; i ++)
	{
		auto  pedal_proto = dev->pedal(i);
		this->pedal.timestamp.push_back(pedal_proto.timestamp());
		this->pedal.throttle1.push_back(pedal_proto.throttle1());
		this->pedal.throttle2.push_back(pedal_proto.throttle2());
		this->pedal.brake_front.push_back(pedal_proto.brake_front());
		this->pedal.brake_rear.push_back(pedal_proto.brake_rear());
	}
	this->pedal.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->steer_size();
	for(int i = 0; i < size; i ++)
	{
		auto  steer_proto = dev->steer(i);
		this->steer.timestamp.push_back(steer_proto.timestamp());
		this->steer.angle.push_back(steer_proto.angle());
	}
	this->steer.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->ecu_state_size();
	for(int i = 0; i < size; i ++)
	{
		auto  ecu_state_proto = dev->ecu_state(i);
		this->ecu_state.timestamp.push_back(ecu_state_proto.timestamp());
		this->ecu_state.value.push_back(ecu_state_proto.value());
	}
	this->ecu_state.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->bms_hv_state_size();
	for(int i = 0; i < size; i ++)
	{
		auto  bms_hv_state_proto = dev->bms_hv_state(i);
		this->bms_hv_state.timestamp.push_back(bms_hv_state_proto.timestamp());
		this->bms_hv_state.value.push_back(bms_hv_state_proto.value());
	}
	this->bms_hv_state.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->steering_wheel_state_size();
	for(int i = 0; i < size; i ++)
	{
		auto  steering_wheel_state_proto = dev->steering_wheel_state(i);
		this->steering_wheel_state.timestamp.push_back(steering_wheel_state_proto.timestamp());
		this->steering_wheel_state.value.push_back(steering_wheel_state_proto.value());
	}
	this->steering_wheel_state.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->ecu_size();
	for(int i = 0; i < size; i ++)
	{
		auto  ecu_proto = dev->ecu(i);
		this->ecu.timestamp.push_back(ecu_proto.timestamp());
		this->ecu.power_request_left.push_back(ecu_proto.power_request_left());
		this->ecu.power_request_right.push_back(ecu_proto.power_request_right());
	}
	this->ecu.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->gps1_size();
	for(int i = 0; i < size; i ++)
	{
		auto  gps1_proto = dev->gps1(i);
		this->gps1.timestamp.push_back(gps1_proto.timestamp());
		this->gps1.msg_type.push_back(gps1_proto.msg_type());
		this->gps1.time.push_back(gps1_proto.time());
		this->gps1.latitude.push_back(gps1_proto.latitude());
		this->gps1.longitude.push_back(gps1_proto.longitude());
		this->gps1.altitude.push_back(gps1_proto.altitude());
		this->gps1.fix.push_back(gps1_proto.fix());
		this->gps1.satellites.push_back(gps1_proto.satellites());
		this->gps1.fix_state.push_back(gps1_proto.fix_state());
		this->gps1.age_of_correction.push_back(gps1_proto.age_of_correction());
		this->gps1.course_over_ground_degrees.push_back(gps1_proto.course_over_ground_degrees());
		this->gps1.course_over_ground_degrees_magnetic.push_back(gps1_proto.course_over_ground_degrees_magnetic());
		this->gps1.speed_kmh.push_back(gps1_proto.speed_kmh());
		this->gps1.mode.push_back(gps1_proto.mode());
	}
	this->gps1.CheckFieldsSize(m_MaxDeviceSize);

	size = dev->gps2_size();
	for(int i = 0; i < size; i ++)
	{
		auto  gps2_proto = dev->gps2(i);
		this->gps2.timestamp.push_back(gps2_proto.timestamp());
		this->gps2.msg_type.push_back(gps2_proto.msg_type());
		this->gps2.time.push_back(gps2_proto.time());
		this->gps2.latitude.push_back(gps2_proto.latitude());
		this->gps2.longitude.push_back(gps2_proto.longitude());
		this->gps2.altitude.push_back(gps2_proto.altitude());
		this->gps2.fix.push_back(gps2_proto.fix());
		this->gps2.satellites.push_back(gps2_proto.satellites());
		this->gps2.fix_state.push_back(gps2_proto.fix_state());
		this->gps2.age_of_correction.push_back(gps2_proto.age_of_correction());
		this->gps2.course_over_ground_degrees.push_back(gps2_proto.course_over_ground_degrees());
		this->gps2.course_over_ground_degrees_magnetic.push_back(gps2_proto.course_over_ground_degrees_magnetic());
		this->gps2.speed_kmh.push_back(gps2_proto.speed_kmh());
		this->gps2.mode.push_back(gps2_proto.mode());
	}
	this->gps2.CheckFieldsSize(m_MaxDeviceSize);

}

