#include "stopwatch_audio.h"

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome::stopwatch_audio {

static const char *const TAG = "stopwatch_audio";

bool StopWatchAudioComponent::write_u8_(uint8_t reg, uint8_t value) {
  for (uint8_t attempt = 0; attempt < 3; attempt++) {
    if (this->write_byte(reg, value)) {
      delayMicroseconds(500);
      return true;
    }
    delay(20);
  }
  ESP_LOGE(TAG, "Failed to write ES8311 register 0x%02X", reg);
  return false;
}

bool StopWatchAudioComponent::apply_output_setup_() {
  bool ok = true;
  ok &= this->write_u8_(0x00, 0x80);  // Reset / CSM power on.
  ok &= this->write_u8_(0x01, 0xB5);  // Clock manager: MCLK=BCLK.
  ok &= this->write_u8_(0x02, 0x18);  // Clock manager: MULT_PRE=3.
  ok &= this->write_u8_(0x0D, 0x01);  // Power up analog circuitry.
  ok &= this->write_u8_(0x12, 0x00);  // Power up DAC.
  ok &= this->write_u8_(0x13, 0x10);  // Enable output to headphone driver.
  ok &= this->write_u8_(0x32, 0xEF);  // DAC volume, +24 dB.
  ok &= this->write_u8_(0x37, 0x08);  // Bypass DAC equalizer.
  return ok;
}

void StopWatchAudioComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up StopWatch ES8311 output path...");

  if (!this->apply_output_setup_()) {
    this->mark_failed();
    return;
  }

  ESP_LOGI(TAG, "ES8311 output path configured");
}

void StopWatchAudioComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "StopWatch Audio:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, ESP_LOG_MSG_COMM_FAIL);
  }
}

}  // namespace esphome::stopwatch_audio
