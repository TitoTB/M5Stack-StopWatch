import esphome.codegen as cg
from esphome.components import i2c
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["i2c"]
CODEOWNERS = ["@codex"]

stopwatch_power_ns = cg.esphome_ns.namespace("stopwatch_power")
StopWatchPowerComponent = stopwatch_power_ns.class_(
    "StopWatchPowerComponent", cg.Component, i2c.I2CDevice
)

CONF_L3B_PIN = "l3b_pin"
CONF_OLED_RESET_PIN = "oled_reset_pin"
CONF_RESET_PULSE = "reset_pulse"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(StopWatchPowerComponent),
            cv.Optional(CONF_L3B_PIN, default=7): cv.int_range(min=0, max=13),
            cv.Optional(CONF_OLED_RESET_PIN, default=4): cv.int_range(min=0, max=13),
            cv.Optional(CONF_RESET_PULSE, default=True): cv.boolean,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x4F))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_l3b_pin(config[CONF_L3B_PIN]))
    cg.add(var.set_oled_reset_pin(config[CONF_OLED_RESET_PIN]))
    cg.add(var.set_reset_pulse(config[CONF_RESET_PULSE]))
