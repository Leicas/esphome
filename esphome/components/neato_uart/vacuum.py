"""Support for Neato UART vacuum cleaner."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import vacuum
from esphome.const import CONF_ID
from . import NeatoUARTComponent, CONF_NEATO_UART_ID, neato_uart_ns

DEPENDENCIES = ["neato_uart", "vacuum"]

NeatoVacuum = neato_uart_ns.class_("NeatoVacuum", vacuum.Vacuum, cg.Component)

CONFIG_SCHEMA = vacuum.vacuum_schema(NeatoVacuum).extend(
    {
        cv.GenerateID(CONF_NEATO_UART_ID): cv.use_id(NeatoUARTComponent),
    }
)


async def to_code(config):
    """Code generation for Neato UART vacuum."""
    parent = await cg.get_variable(config[CONF_NEATO_UART_ID])
    var = cg.new_Pvariable(config[CONF_ID], parent)
    await cg.register_component(var, config)
    await vacuum.register_vacuum(var, config)


