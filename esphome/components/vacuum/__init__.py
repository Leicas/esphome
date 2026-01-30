from esphome import automation
from esphome.automation import maybe_simple_id
import esphome.codegen as cg
from esphome.components import web_server
import esphome.config_validation as cv
from esphome.const import (
    CONF_DEVICE_CLASS,
    CONF_ENTITY_CATEGORY,
    CONF_ICON,
    CONF_ID,
    CONF_ON_STATE,
    CONF_TRIGGER_ID,
    CONF_WEB_SERVER,
    DEVICE_CLASS_EMPTY,
)
from esphome.core import CORE, CoroPriority, coroutine_with_priority
from esphome.core.entity_helpers import entity_duplicate_validator, setup_entity
from esphome.cpp_generator import MockObjClass

CODEOWNERS = ["@esphome/core"]
IS_PLATFORM_COMPONENT = True

DEVICE_CLASSES = [DEVICE_CLASS_EMPTY]

vacuum_ns = cg.esphome_ns.namespace("vacuum")

Vacuum = vacuum_ns.class_("Vacuum", cg.EntityBase)
VacuumPtr = Vacuum.operator("ptr")

VacuumState = vacuum_ns.enum("VacuumState")

# Vacuum states
VACUUM_STATE_UNKNOWN = vacuum_ns.VACUUM_STATE_UNKNOWN
VACUUM_STATE_IDLE = vacuum_ns.VACUUM_STATE_IDLE
VACUUM_STATE_CLEANING = vacuum_ns.VACUUM_STATE_CLEANING
VACUUM_STATE_DOCKING = vacuum_ns.VACUUM_STATE_DOCKING
VACUUM_STATE_ERROR = vacuum_ns.VACUUM_STATE_ERROR
VACUUM_STATE_PAUSED = vacuum_ns.VACUUM_STATE_PAUSED
VACUUM_STATE_RETURNING = vacuum_ns.VACUUM_STATE_RETURNING

VACUUM_STATES = {
    "UNKNOWN": VACUUM_STATE_UNKNOWN,
    "IDLE": VACUUM_STATE_IDLE,
    "CLEANING": VACUUM_STATE_CLEANING,
    "DOCKING": VACUUM_STATE_DOCKING,
    "ERROR": VACUUM_STATE_ERROR,
    "PAUSED": VACUUM_STATE_PAUSED,
    "RETURNING": VACUUM_STATE_RETURNING,
}

validate_vacuum_state = cv.enum(VACUUM_STATES, upper=True)

# Actions
StartAction = vacuum_ns.class_("StartAction", automation.Action)
StopAction = vacuum_ns.class_("StopAction", automation.Action)
PauseAction = vacuum_ns.class_("PauseAction", automation.Action)
ReturnToBaseAction = vacuum_ns.class_("ReturnToBaseAction", automation.Action)

# Triggers
VacuumStateChangeTrigger = vacuum_ns.class_(
    "VacuumStateChangeTrigger", automation.Trigger.template(VacuumState)
)

validate_device_class = cv.one_of(*DEVICE_CLASSES, lower=True)

_VACUUM_SCHEMA = (
    cv.ENTITY_BASE_SCHEMA.extend(web_server.WEBSERVER_SORTING_SCHEMA)
    .extend(
        {
            cv.Optional(CONF_DEVICE_CLASS): validate_device_class,
            cv.Optional(CONF_ON_STATE): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        VacuumStateChangeTrigger
                    ),
                }
            ),
        }
    )
)

_VACUUM_SCHEMA.add_extra(entity_duplicate_validator("vacuum"))


def vacuum_schema(
    class_: MockObjClass,
    *,
    device_class: str = cv.UNDEFINED,
    entity_category: str = cv.UNDEFINED,
    icon: str = cv.UNDEFINED,
) -> cv.Schema:
    schema = {
        cv.GenerateID(): cv.declare_id(class_),
    }

    for key, default, validator in [
        (CONF_DEVICE_CLASS, device_class, validate_device_class),
        (CONF_ENTITY_CATEGORY, entity_category, cv.entity_category),
        (CONF_ICON, icon, cv.icon),
    ]:
        if default is not cv.UNDEFINED:
            schema[cv.Optional(key, default=default)] = validator

    return _VACUUM_SCHEMA.extend(schema)


async def setup_vacuum_core_(var, config):
    await setup_entity(var, config, "vacuum")
    for conf in config.get(CONF_ON_STATE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [(VacuumState, "state")], conf)

    if (device_class := config.get(CONF_DEVICE_CLASS)) is not None:
        cg.add(var.set_device_class(device_class))

    if web_server_config := config.get(CONF_WEB_SERVER):
        await web_server.add_entity_config(var, web_server_config)


@coroutine_with_priority(CoroPriority.CORE)
async def register_vacuum(var, config):
    """Register a vacuum entity."""
    if not CORE.has_id(config[CONF_ID]):
        var = cg.Pvariable(config[CONF_ID], var)
    cg.add(cg.App.register_component(var))
    CORE.register_platform_component("vacuum", var)
    await setup_vacuum_core_(var, config)


VACUUM_ACTION_SCHEMA = maybe_simple_id({cv.GenerateID(CONF_ID): cv.use_id(Vacuum)})


@automation.register_action("vacuum.start", StartAction, VACUUM_ACTION_SCHEMA)
@automation.register_action("vacuum.stop", StopAction, VACUUM_ACTION_SCHEMA)
@automation.register_action("vacuum.pause", PauseAction, VACUUM_ACTION_SCHEMA)
@automation.register_action(
    "vacuum.return_to_base", ReturnToBaseAction, VACUUM_ACTION_SCHEMA
)
async def vacuum_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


@coroutine_with_priority(CoroPriority.CORE)
async def to_code(config):
    cg.add_define("USE_VACUUM")
    cg.add_global(vacuum_ns.using)
