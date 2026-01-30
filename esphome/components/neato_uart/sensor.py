"""Support for Neato UART sensors."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_BATTERY_VOLTAGE,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    UNIT_VOLT,
    UNIT_SECOND,
    UNIT_MINUTE,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import NeatoUARTComponent, CONF_NEATO_UART_ID, neato_uart_ns

DEPENDENCIES = ["neato_uart"]

CONF_FUEL_PERCENT = "fuel_percent"
CONF_BATTERY_TEMP = "battery_temp"
CONF_EXTERNAL_VOLTAGE = "external_voltage"
CONF_CHARGER_MAH = "charger_mah"
CONF_DISCHARGE_MAH = "discharge_mah"
CONF_BATTERY_CYCLES = "battery_cycles"
CONF_FILTER_CHANGE_TIME = "filter_change_time"
CONF_BRUSH_CHANGE_TIME = "brush_change_time"
CONF_DIRT_BIN_ALERT = "dirt_bin_alert"
CONF_CURRENT_DIRT_BIN_RUNTIME = "current_dirt_bin_runtime"
CONF_NUMBER_DUST_BIN_FULL = "number_dust_bin_full"
CONF_LAST_CLEANING_DURATION = "last_cleaning_duration"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_NEATO_UART_ID): cv.use_id(NeatoUARTComponent),
        cv.Optional(CONF_FUEL_PERCENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_BATTERY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_TEMP): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_EXTERNAL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CHARGER_MAH): sensor.sensor_schema(
            unit_of_measurement="mAh",
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_DISCHARGE_MAH): sensor.sensor_schema(
            unit_of_measurement="mAh",
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_BATTERY_CYCLES): sensor.sensor_schema(
            unit_of_measurement="cycles",
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_FILTER_CHANGE_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECOND,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_BRUSH_CHANGE_TIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECOND,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_DIRT_BIN_ALERT): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTE,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CURRENT_DIRT_BIN_RUNTIME): sensor.sensor_schema(
            unit_of_measurement=UNIT_SECOND,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_NUMBER_DUST_BIN_FULL): sensor.sensor_schema(
            unit_of_measurement="count",
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_LAST_CLEANING_DURATION): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTE,
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    """Code generation for Neato UART sensors."""
    parent = await cg.get_variable(config[CONF_NEATO_UART_ID])

    if fuel_percent_config := config.get(CONF_FUEL_PERCENT):
        sens = await sensor.new_sensor(fuel_percent_config)
        cg.add(parent.set_fuel_percent_sensor(sens))

    if battery_temp_config := config.get(CONF_BATTERY_TEMP):
        sens = await sensor.new_sensor(battery_temp_config)
        cg.add(parent.set_battery_temp_sensor(sens))

    if battery_voltage_config := config.get(CONF_BATTERY_VOLTAGE):
        sens = await sensor.new_sensor(battery_voltage_config)
        cg.add(parent.set_battery_voltage_sensor(sens))

    if external_voltage_config := config.get(CONF_EXTERNAL_VOLTAGE):
        sens = await sensor.new_sensor(external_voltage_config)
        cg.add(parent.set_external_voltage_sensor(sens))

    if charger_mah_config := config.get(CONF_CHARGER_MAH):
        sens = await sensor.new_sensor(charger_mah_config)
        cg.add(parent.set_charger_mah_sensor(sens))

    if discharge_mah_config := config.get(CONF_DISCHARGE_MAH):
        sens = await sensor.new_sensor(discharge_mah_config)
        cg.add(parent.set_discharge_mah_sensor(sens))

    if battery_cycles_config := config.get(CONF_BATTERY_CYCLES):
        sens = await sensor.new_sensor(battery_cycles_config)
        cg.add(parent.set_battery_cycles_sensor(sens))

    if filter_change_time_config := config.get(CONF_FILTER_CHANGE_TIME):
        sens = await sensor.new_sensor(filter_change_time_config)
        cg.add(parent.set_filter_change_time_sensor(sens))

    if brush_change_time_config := config.get(CONF_BRUSH_CHANGE_TIME):
        sens = await sensor.new_sensor(brush_change_time_config)
        cg.add(parent.set_brush_change_time_sensor(sens))

    if dirt_bin_alert_config := config.get(CONF_DIRT_BIN_ALERT):
        sens = await sensor.new_sensor(dirt_bin_alert_config)
        cg.add(parent.set_dirt_bin_alert_sensor(sens))

    if current_dirt_bin_runtime_config := config.get(CONF_CURRENT_DIRT_BIN_RUNTIME):
        sens = await sensor.new_sensor(current_dirt_bin_runtime_config)
        cg.add(parent.set_current_dirt_bin_runtime_sensor(sens))

    if number_dust_bin_full_config := config.get(CONF_NUMBER_DUST_BIN_FULL):
        sens = await sensor.new_sensor(number_dust_bin_full_config)
        cg.add(parent.set_number_dust_bin_full_sensor(sens))

    if last_cleaning_duration_config := config.get(CONF_LAST_CLEANING_DURATION):
        sens = await sensor.new_sensor(last_cleaning_duration_config)
        cg.add(parent.set_last_cleaning_duration_sensor(sens))
