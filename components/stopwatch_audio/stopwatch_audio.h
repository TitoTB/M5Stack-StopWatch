#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

namespace esphome::stopwatch_audio {

class StopWatchAudioComponent : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::LATE; }
  void configure_speaker();
  void configure_microphone();

 protected:
  bool write_u8_(uint8_t reg, uint8_t value);
  bool apply_speaker_setup_();
  bool apply_microphone_setup_();
};

}  // namespace esphome::stopwatch_audio
