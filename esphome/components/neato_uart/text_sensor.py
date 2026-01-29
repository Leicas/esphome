"""Support for Neato UART text sensors."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    CONF_ID,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import NeatoUARTComponent, CONF_NEATO_UART_ID, neato_uart_ns

DEPENDENCIES = ["neato_uart"]

CONF_ROBOT_ERROR = "robot_error"
CONF_ROBOT_ALERT = "robot_alert"
CONF_UI_STATE = "ui_state"
CONF_ROBOT_STATE = "robot_state"
CONF_MAINBOARD_SERIAL = "mainboard_serial"
CONF_MAINBOARD_VERSION = "mainboard_version"
CONF_ROBOT_SERIAL = "robot_serial"
CONF_ROBOT_MODEL = "robot_model"
CONF_LANGUAGE = "language"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_NEATO_UART_ID): cv.use_id(NeatoUARTComponent),
        cv.Optional(CONF_ROBOT_ERROR): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_ROBOT_ALERT): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_UI_STATE): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_ROBOT_STATE): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_MAINBOARD_SERIAL): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_MAINBOARD_VERSION): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_ROBOT_SERIAL): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_ROBOT_MODEL): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_LANGUAGE): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
    }
)


async def to_code(config):
    """Code generation for Neato UART text sensors."""
    parent = await cg.get_variable(config[CONF_NEATO_UART_ID])

    if robot_error_config := config.get(CONF_ROBOT_ERROR):
        sens = await text_sensor.new_text_sensor(robot_error_config)
        cg.add(parent.set_robot_error_text_sensor(sens))

    if robot_alert_config := config.get(CONF_ROBOT_ALERT):
        sens = await text_sensor.new_text_sensor(robot_alert_config)
        cg.add(parent.set_robot_alert_text_sensor(sens))

    if ui_state_config := config.get(CONF_UI_STATE):
        sens = await text_sensor.new_text_sensor(ui_state_config)
        cg.add(parent.set_ui_state_text_sensor(sens))

    if robot_state_config := config.get(CONF_ROBOT_STATE):
        sens = await text_sensor.new_text_sensor(robot_state_config)
        cg.add(parent.set_robot_state_text_sensor(sens))

    if mainboard_serial_config := config.get(CONF_MAINBOARD_SERIAL):
        sens = await text_sensor.new_text_sensor(mainboard_serial_config)
        cg.add(parent.set_mainboard_serial_text_sensor(sens))

    if mainboard_version_config := config.get(CONF_MAINBOARD_VERSION):
        sens = await text_sensor.new_text_sensor(mainboard_version_config)
        cg.add(parent.set_mainboard_version_text_sensor(sens))

    if robot_serial_config := config.get(CONF_ROBOT_SERIAL):
        sens = await text_sensor.new_text_sensor(robot_serial_config)
        cg.add(parent.set_robot_serial_text_sensor(sens))

    if robot_model_config := config.get(CONF_ROBOT_MODEL):
        sens = await text_sensor.new_text_sensor(robot_model_config)
        cg.add(parent.set_robot_model_text_sensor(sens))

    if language_config := config.get(CONF_LANGUAGE):
        sens = await text_sensor.new_text_sensor(language_config)
        cg.add(parent.set_language_text_sensor(sens))
