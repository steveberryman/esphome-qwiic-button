import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID
from .. import qwiic_button_ns, QwiicButton, CONF_QWIIC_BUTTON_ID

DEPENDENCIES = ["qwiic_button"]

CONF_BRIGHTNESS = "brightness"

QwiicButtonLEDSwitch = qwiic_button_ns.class_(
    "QwiicButtonLEDSwitch", switch.Switch, cg.Component
)

CONFIG_SCHEMA = switch.switch_schema(QwiicButtonLEDSwitch).extend(
    {
        cv.GenerateID(CONF_QWIIC_BUTTON_ID): cv.use_id(QwiicButton),
        cv.Optional(CONF_BRIGHTNESS, default=255): cv.int_range(min=0, max=255),
    }
)


async def to_code(config):
    var = await switch.new_switch(config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_QWIIC_BUTTON_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_brightness(config[CONF_BRIGHTNESS]))
