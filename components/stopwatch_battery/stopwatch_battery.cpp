#include "stopwatch_battery.h"

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome::stopwatch_battery {

static const char *const TAG = "stopwatch_battery";

static constexpr uint8_t REG_DEVICE_ID = 0x00;
static constexpr uint8_t REG_I2C_CFG = 0x09;
static constexpr uint8_t REG_VBAT_L = 0x22;

bool StopWatchBatteryComponent::read_u8_(uint8_t reg, uint8_t *value) {
  for (uint8_t attempt = 0; attempt < 3; attempt++) {
    if (this->read_byte(reg, value)) {
      delayMicroseconds(500);
      return true;
    }
    delay(20);
  }
  return false;
}

bool StopWatchBatteryComponent::write_u8_(uint8_t reg, uint8_t value) {
  for (uint8_t attempt = 0; attempt < 3; attempt++) {
    if (this->write_byte(reg, value)) {
      delayMicroseconds(500);
      return true;
    }
    delay(20);
  }
  return false;
}

bool StopWatchBatteryComponent::read_u16_(uint8_t reg, uint16_t *value) {
  uint8_t data[2]{};
  for (uint8_t attempt = 0; attempt < 3; attempt++) {
    if (this->read_bytes(reg, data, 2)) {
      *value = uint16_t(data[0]) | (uint16_t(data[1]) << 8);
      delayMicroseconds(500);
      return true;
    }
    delay(20);
  }
  return false;
}

float StopWatchBatteryComponent::voltage_to_percent_(float voltage) {
  float percent = (voltage - 3.30f) * 100.0f / (4.20f - 3.30f);
  if (percent < 0.0f) {
    return 0.0f;
  }
  if (percent > 100.0f) {
    return 100.0f;
  }
  return percent;
}

void StopWatchBatteryComponent::setup() {
  uint8_t device_id = 0;
  if (!this->read_u8_(REG_DEVICE_ID, &device_id)) {
    ESP_LOGE(TAG, "M5PM1 did not respond at 0x%02X", this->address_);
    this->mark_failed();
    return;
  }

  // Keep PM1 awake on the shared I2C bus and at 100 kHz.
  if (!this->write_u8_(REG_I2C_CFG, 0x00)) {
    ESP_LOGW(TAG, "Failed to disable M5PM1 I2C sleep");
  }

  ESP_LOGI(TAG, "M5PM1 device id: 0x%02X", device_id);
}

void StopWatchBatteryComponent::update() {
  if (this->is_failed()) {
    return;
  }

  uint16_t mv = 0;
  if (!this->read_u16_(REG_VBAT_L, &mv)) {
    ESP_LOGW(TAG, "Failed to read battery voltage");
    this->status_set_warning();
    return;
  }
  this->status_clear_warning();

  const float voltage = mv / 1000.0f;
  if (this->voltage_sensor_ != nullptr) {
    this->voltage_sensor_->publish_state(voltage);
  }
  if (this->level_sensor_ != nullptr) {
    this->level_sensor_->publish_state(this->voltage_to_percent_(voltage));
  }
}

void StopWatchBatteryComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "StopWatch Battery:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Voltage", this->voltage_sensor_);
  LOG_SENSOR("  ", "Level", this->level_sensor_);
  if (this->is_failed()) {
    ESP_LOGE(TAG, ESP_LOG_MSG_COMM_FAIL);
  }
}

}  // namespace esphome::stopwatch_battery
