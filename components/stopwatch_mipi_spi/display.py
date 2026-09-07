from esphome.components.mipi import (
    MODE_RGB,
    PAGESEL,
    SLPOUT,
    SPIMODESEL,
    WCE,
    WRCTRLD,
    DriverChip,
)
from esphome.components.mipi_spi.display import *  # noqa: F401,F403
from esphome.components.mipi_spi.display import MODELS
from esphome.components.spi import TYPE_QUAD
from esphome.const import CONF_MIRROR_X, CONF_MIRROR_Y


AUTO_LOAD = ["mipi_spi"]
DEPENDENCIES = ["spi"]

MODELS["M5STACK-STOPWATCH"] = DriverChip(
    "M5STACK-STOPWATCH",
    brightness=0xD0,
    color_order=MODE_RGB,
    bus_mode=TYPE_QUAD,
    no_slpout=True,
    transforms={CONF_MIRROR_X, CONF_MIRROR_Y},
    native_width=480,
    native_height=480,
    width=466,
    height=466,
    offset_width=6,
    offset_height=0,
    pad_width=8,
    pad_height=14,
    initsequence=(
        (SLPOUT,),
        (PAGESEL, 0x00),
        (SPIMODESEL, 0x80),
        (WRCTRLD, 0x20),
        (WCE, 0x00),
    ),
)
