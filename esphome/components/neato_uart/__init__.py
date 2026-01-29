"""Support for Neato vacuum cleaners over UART."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart, time
from esphome.const import CONF_ID

CODEOWNERS = ["@leicas"]
DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "binary_sensor", "text_sensor"]

neato_uart_ns = cg.esphome_ns.namespace("neato_uart")
NeatoUARTComponent = neato_uart_ns.class_(
    "NeatoUARTComponent", cg.Component, uart.UARTDevice
)

# Actions
SendCommandAction = neato_uart_ns.class_("SendCommandAction", automation.Action)
PlaySoundAction = neato_uart_ns.class_("PlaySoundAction", automation.Action)

CONF_NEATO_UART_ID = "neato_uart_id"
CONF_COMMAND = "command"
CONF_SOUND_ID = "sound_id"
CONF_TIME_ID = "time_id"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(NeatoUARTComponent),
            cv.Optional(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    """Code generation for Neato UART component."""
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_TIME_ID in config:
        time_id = await cg.get_variable(config[CONF_TIME_ID])
        cg.add(var.set_time_id(time_id))


@automation.register_action(
    "neato_uart.send_command",
    SendCommandAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(NeatoUARTComponent),
            cv.Required(CONF_COMMAND): cv.templatable(cv.string),
        }
    ),
)
async def send_command_to_code(config, action_id, template_arg, args):
    """Code generation for send command action."""
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_COMMAND], args, cg.std_string)
    cg.add(var.set_command(template_))
    return var


@automation.register_action(
    "neato_uart.play_sound",
    PlaySoundAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(NeatoUARTComponent),
            cv.Required(CONF_SOUND_ID): cv.templatable(cv.int_),
        }
    ),
)
async def play_sound_to_code(config, action_id, template_arg, args):
    """Code generation for play sound action."""
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_SOUND_ID], args, cg.int_)
    cg.add(var.set_sound_id(template_))
    return var
