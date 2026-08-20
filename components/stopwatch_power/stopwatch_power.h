#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

namespace esphome::stopwatch_power {

class StopWatchPowerComponent : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::IO; }

  void set_l3b_pin(uint8_t pin) { this->l3b_pin_ = pin; }
  void set_oled_reset_pin(uint8_t pin) { this->oled_reset_pin_ = pin; }
  void set_reset_pulse(bool reset_pulse) { this->reset_pulse_ = reset_pulse; }
  void set_speaker_amp(bool speaker_amp) { this->speaker_amp_ = speaker_amp; }
  void set_vibration(float level);

 protected:
  bool read_u16_(uint8_t reg, uint16_t *value);
  bool write_u16_(uint8_t reg, uint16_t value);
  bool read_u8_(uint8_t reg, uint8_t *value);
  bool write_u8_(uint8_t reg, uint8_t value);
  bool bit_write_(uint8_t reg, uint8_t bit, bool value);
  bool set_output_pin_(uint8_t pin, bool value);
  bool write_pwm1_duty_(uint16_t duty12, bool enable);

  uint8_t l3b_pin_{7};
  uint8_t oled_reset_pin_{4};
  bool reset_pulse_{true};
  bool speaker_amp_{false};
};

}  // namespace esphome::stopwatch_power
