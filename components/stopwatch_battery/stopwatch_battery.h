#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome::stopwatch_battery {

class StopWatchBatteryComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_voltage_sensor(sensor::Sensor *sensor) { this->voltage_sensor_ = sensor; }
  void set_level_sensor(sensor::Sensor *sensor) { this->level_sensor_ = sensor; }

 protected:
  bool read_u8_(uint8_t reg, uint8_t *value);
  bool write_u8_(uint8_t reg, uint8_t value);
  bool read_u16_(uint8_t reg, uint16_t *value);
  float voltage_to_percent_(float voltage);

  sensor::Sensor *voltage_sensor_{nullptr};
  sensor::Sensor *level_sensor_{nullptr};
};

}  // namespace esphome::stopwatch_battery
