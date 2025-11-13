import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, binary_sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_EMPTY,
)

DEPENDENCIES = ["i2c"]
MULTI_CONF = True

CONF_QWIIC_BUTTON_ID = "qwiic_button_id"
CONF_PRESSED = "pressed"
CONF_CLICKED = "clicked"
CONF_DEBOUNCE_TIME = "debounce_time"
CONF_ENABLE_PRESSED_INTERRUPT = "enable_pressed_interrupt"
CONF_ENABLE_CLICKED_INTERRUPT = "enable_clicked_interrupt"
CONF_USE_INTERRUPT = "use_interrupt"

qwiic_button_ns = cg.esphome_ns.namespace("qwiic_button")
QwiicButton = qwiic_button_ns.class_("QwiicButton", cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(QwiicButton),
            cv.Optional(CONF_PRESSED): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_EMPTY
            ),
            cv.Optional(CONF_CLICKED): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_EMPTY
            ),
            cv.Optional(CONF_DEBOUNCE_TIME, default=10): cv.int_range(
                min=0, max=65535
            ),
            cv.Optional(CONF_ENABLE_PRESSED_INTERRUPT, default=False): cv.boolean,
            cv.Optional(CONF_ENABLE_CLICKED_INTERRUPT, default=False): cv.boolean,
            cv.Optional(CONF_USE_INTERRUPT, default=False): cv.boolean,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x6F))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_PRESSED in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_PRESSED])
        cg.add(var.set_pressed_binary_sensor(sens))

    if CONF_CLICKED in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_CLICKED])
        cg.add(var.set_clicked_binary_sensor(sens))

    cg.add(var.set_debounce_time(config[CONF_DEBOUNCE_TIME]))
    cg.add(var.set_enable_pressed_interrupt(config[CONF_ENABLE_PRESSED_INTERRUPT]))
    cg.add(var.set_enable_clicked_interrupt(config[CONF_ENABLE_CLICKED_INTERRUPT]))
    cg.add(var.set_use_interrupt(config[CONF_USE_INTERRUPT]))
