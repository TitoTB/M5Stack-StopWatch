import esphome.codegen as cg
from esphome.components import i2c, sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_VOLTAGE,
    ICON_BATTERY,
    STATE_CLASS_MEASUREMENT,
    UNIT_PERCENT,
    UNIT_VOLT,
)

DEPENDENCIES = ["i2c"]
CODEOWNERS = ["@codex"]

stopwatch_battery_ns = cg.esphome_ns.namespace("stopwatch_battery")
StopWatchBatteryComponent = stopwatch_battery_ns.class_(
    "StopWatchBatteryComponent", cg.PollingComponent, i2c.I2CDevice
)

CONF_VOLTAGE = "voltage"
CONF_LEVEL = "level"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(StopWatchBatteryComponent),
            cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_VOLTAGE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_LEVEL): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_BATTERY,
                state_class=STATE_CLASS_MEASUREMENT,
                icon=ICON_BATTERY,
            ),
        }
    )
    .extend(cv.polling_component_schema("30s"))
    .extend(i2c.i2c_device_schema(0x6E))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if voltage_config := config.get(CONF_VOLTAGE):
        sens = await sensor.new_sensor(voltage_config)
        cg.add(var.set_voltage_sensor(sens))

    if level_config := config.get(CONF_LEVEL):
        sens = await sensor.new_sensor(level_config)
        cg.add(var.set_level_sensor(sens))
