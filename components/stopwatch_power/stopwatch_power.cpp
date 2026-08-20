#include "stopwatch_power.h"

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome::stopwatch_power {

static const char *const TAG = "stopwatch_power";

static constexpr uint8_t REG_UID_L = 0x00;
static constexpr uint8_t REG_VERSION = 0x02;
static constexpr uint8_t REG_GPIO_MODE_L = 0x03;
static constexpr uint8_t REG_GPIO_OUTPUT_L = 0x05;
static constexpr uint8_t REG_GPIO_DRIVE_L = 0x13;

bool StopWatchPowerComponent::read_u16_(uint8_t reg, uint16_t *value) {
  uint8_t data[2]{};
  if (!this->read_bytes(reg, data, 2)) {
    return false;
  }
  *value = uint16_t(data[0]) | (uint16_t(data[1]) << 8);
  return true;
}

bool StopWatchPowerComponent::write_u16_(uint8_t reg, uint16_t value) {
  uint8_t data[2] = {uint8_t(value & 0xFF), uint8_t(value >> 8)};
  return this->write_bytes(reg, data, 2);
}

bool StopWatchPowerComponent::set_output_pin_(uint8_t pin, bool value) {
  const uint16_t bit = uint16_t(1) << pin;

  uint16_t mode = 0;
  if (!this->read_u16_(REG_GPIO_MODE_L, &mode)) {
    ESP_LOGE(TAG, "Failed to read M5IOE1 GPIO mode registers");
    return false;
  }
  mode |= bit;
  if (!this->write_u16_(REG_GPIO_MODE_L, mode)) {
    ESP_LOGE(TAG, "Failed to write M5IOE1 GPIO mode registers");
    return false;
  }

  uint16_t drive = 0;
  if (!this->read_u16_(REG_GPIO_DRIVE_L, &drive)) {
    ESP_LOGE(TAG, "Failed to read M5IOE1 GPIO drive registers");
    return false;
  }
  drive &= ~bit;
  if (!this->write_u16_(REG_GPIO_DRIVE_L, drive)) {
    ESP_LOGE(TAG, "Failed to write M5IOE1 GPIO drive registers");
    return false;
  }

  uint16_t output = 0;
  if (!this->read_u16_(REG_GPIO_OUTPUT_L, &output)) {
    ESP_LOGE(TAG, "Failed to read M5IOE1 GPIO output registers");
    return false;
  }
  if (value) {
    output |= bit;
  } else {
    output &= ~bit;
  }
  if (!this->write_u16_(REG_GPIO_OUTPUT_L, output)) {
    ESP_LOGE(TAG, "Failed to write M5IOE1 GPIO output registers");
    return false;
  }

  return true;
}

void StopWatchPowerComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up StopWatch display power...");

  uint8_t uid[2]{};
  uint8_t version = 0;
  if (!this->read_bytes(REG_UID_L, uid, 2) || !this->read_byte(REG_VERSION, &version)) {
    ESP_LOGE(TAG, "M5IOE1 did not respond at 0x%02X", this->address_);
    this->mark_failed();
    return;
  }

  ESP_LOGI(TAG, "M5IOE1 UID: 0x%02X%02X, firmware: 0x%02X", uid[1], uid[0], version);

  if (!this->set_output_pin_(this->l3b_pin_, true)) {
    this->mark_failed();
    return;
  }
  ESP_LOGI(TAG, "Enabled AMOLED/L3B power on M5IOE1 pin %u", this->l3b_pin_);

  if (this->reset_pulse_) {
    this->set_output_pin_(this->oled_reset_pin_, false);
    delay(20);
    this->set_output_pin_(this->oled_reset_pin_, true);
    delay(120);
    ESP_LOGI(TAG, "Pulsed AMOLED reset on M5IOE1 pin %u", this->oled_reset_pin_);
  } else {
    this->set_output_pin_(this->oled_reset_pin_, true);
  }
}

void StopWatchPowerComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "StopWatch display power:");
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG, "  L3B power pin: %u", this->l3b_pin_);
  ESP_LOGCONFIG(TAG, "  OLED reset pin: %u", this->oled_reset_pin_);
  ESP_LOGCONFIG(TAG, "  Reset pulse: %s", TRUEFALSE(this->reset_pulse_));
}

}  // namespace esphome::stopwatch_power
