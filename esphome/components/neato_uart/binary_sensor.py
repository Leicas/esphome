"""Support for Neato UART binary sensors."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_ID,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import NeatoUARTComponent, CONF_NEATO_UART_ID, neato_uart_ns

DEPENDENCIES = ["neato_uart"]

CONF_USB_CONNECTED = "usb_connected"
CONF_BATTERY_OVER_TEMP = "battery_over_temp"
CONF_CHARGING_ACTIVE = "charging_active"
CONF_CHARGING_ENABLED = "charging_enabled"
CONF_CONFIDENT_ON_FUEL = "confident_on_fuel"
CONF_ON_RESERVED_FUEL = "on_reserved_fuel"
CONF_EMPTY_FUEL = "empty_fuel"
CONF_BATTERY_FAILURE = "battery_failure"
CONF_EXT_POWER_PRESENT = "ext_power_present"
CONF_THERMISTOR_PRESENT = "thermistor_present"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_NEATO_UART_ID): cv.use_id(NeatoUARTComponent),
        cv.Optional(CONF_USB_CONNECTED): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_BATTERY_OVER_TEMP): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_CHARGING_ACTIVE): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_CHARGING_ENABLED): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_CONFIDENT_ON_FUEL): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_ON_RESERVED_FUEL): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_EMPTY_FUEL): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_BATTERY_FAILURE): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_EXT_POWER_PRESENT): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
        cv.Optional(CONF_THERMISTOR_PRESENT): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC
        ),
    }
)


async def to_code(config):
    """Code generation for Neato UART binary sensors."""
    parent = await cg.get_variable(config[CONF_NEATO_UART_ID])

    if usb_connected_config := config.get(CONF_USB_CONNECTED):
        sens = await binary_sensor.new_binary_sensor(usb_connected_config)
        cg.add(parent.set_usb_connected_binary_sensor(sens))

    if battery_over_temp_config := config.get(CONF_BATTERY_OVER_TEMP):
        sens = await binary_sensor.new_binary_sensor(battery_over_temp_config)
        cg.add(parent.set_battery_over_temp_binary_sensor(sens))

    if charging_active_config := config.get(CONF_CHARGING_ACTIVE):
        sens = await binary_sensor.new_binary_sensor(charging_active_config)
        cg.add(parent.set_charging_active_binary_sensor(sens))

    if charging_enabled_config := config.get(CONF_CHARGING_ENABLED):
        sens = await binary_sensor.new_binary_sensor(charging_enabled_config)
        cg.add(parent.set_charging_enabled_binary_sensor(sens))

    if confident_on_fuel_config := config.get(CONF_CONFIDENT_ON_FUEL):
        sens = await binary_sensor.new_binary_sensor(confident_on_fuel_config)
        cg.add(parent.set_confident_on_fuel_binary_sensor(sens))

    if on_reserved_fuel_config := config.get(CONF_ON_RESERVED_FUEL):
        sens = await binary_sensor.new_binary_sensor(on_reserved_fuel_config)
        cg.add(parent.set_on_reserved_fuel_binary_sensor(sens))

    if empty_fuel_config := config.get(CONF_EMPTY_FUEL):
        sens = await binary_sensor.new_binary_sensor(empty_fuel_config)
        cg.add(parent.set_empty_fuel_binary_sensor(sens))

    if battery_failure_config := config.get(CONF_BATTERY_FAILURE):
        sens = await binary_sensor.new_binary_sensor(battery_failure_config)
        cg.add(parent.set_battery_failure_binary_sensor(sens))

    if ext_power_present_config := config.get(CONF_EXT_POWER_PRESENT):
        sens = await binary_sensor.new_binary_sensor(ext_power_present_config)
        cg.add(parent.set_ext_power_present_binary_sensor(sens))

    if thermistor_present_config := config.get(CONF_THERMISTOR_PRESENT):
        sens = await binary_sensor.new_binary_sensor(thermistor_present_config)
        cg.add(parent.set_thermistor_present_binary_sensor(sens))
