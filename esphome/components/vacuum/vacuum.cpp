#include "vacuum.h"
#include "esphome/core/log.h"

namespace esphome {
namespace vacuum {

static const char *const TAG = "vacuum";

const LogString *vacuum_state_to_str(VacuumState state) {
  switch (state) {
    case VACUUM_STATE_UNKNOWN:
      return LOG_STR("UNKNOWN");
    case VACUUM_STATE_IDLE:
      return LOG_STR("IDLE");
    case VACUUM_STATE_CLEANING:
      return LOG_STR("CLEANING");
    case VACUUM_STATE_DOCKING:
      return LOG_STR("DOCKING");
    case VACUUM_STATE_ERROR:
      return LOG_STR("ERROR");
    case VACUUM_STATE_PAUSED:
      return LOG_STR("PAUSED");
    case VACUUM_STATE_RETURNING:
      return LOG_STR("RETURNING");
    default:
      return LOG_STR("UNKNOWN");
  }
}

Vacuum::Vacuum() = default;

VacuumCall Vacuum::make_call() { return VacuumCall(this); }

void Vacuum::set_state_(VacuumState state) {
  auto call = this->make_call();
  call.set_state(state);
  this->control(call);
}

void Vacuum::start() { this->set_state_(VACUUM_STATE_CLEANING); }
void Vacuum::stop() { this->set_state_(VACUUM_STATE_IDLE); }
void Vacuum::pause() { this->set_state_(VACUUM_STATE_PAUSED); }
void Vacuum::return_to_base() { this->set_state_(VACUUM_STATE_RETURNING); }

void Vacuum::publish_state(bool save) {
  if (!this->publish_dedup_.next(this->state))
    return;

  ESP_LOGD(TAG, "'%s' >> %s (battery=%.0f%%)", this->get_name().c_str(), LOG_STR_ARG(vacuum_state_to_str(this->state)),
           this->battery_level);
  this->state_callback_.call(this->state);
}

void VacuumCall::perform() {
  ESP_LOGD(TAG, "'%s' - Setting", this->parent_->get_name().c_str());
  this->validate_();
  if (this->state_.has_value()) {
    ESP_LOGD(TAG, "  State: %s", LOG_STR_ARG(vacuum_state_to_str(*this->state_)));
  }
  this->parent_->control(*this);
}

void VacuumCall::validate_() {
  if (this->state_.has_value()) {
    auto state = *this->state_;
    if (!this->parent_->traits.supports_state(state)) {
      ESP_LOGW(TAG, "  State %s is not supported by this device!", LOG_STR_ARG(vacuum_state_to_str(state)));
      this->state_.reset();
    }
  }
}

VacuumCall &VacuumCall::set_state(VacuumState state) {
  this->state_ = state;
  return *this;
}

VacuumCall &VacuumCall::set_state(optional<VacuumState> state) {
  this->state_ = state;
  return *this;
}

VacuumCall &VacuumCall::set_state(const std::string &state) {
  if (str_equals_case_insensitive(state, "CLEANING")) {
    this->set_state(VACUUM_STATE_CLEANING);
  } else if (str_equals_case_insensitive(state, "IDLE")) {
    this->set_state(VACUUM_STATE_IDLE);
  } else if (str_equals_case_insensitive(state, "PAUSED")) {
    this->set_state(VACUUM_STATE_PAUSED);
  } else if (str_equals_case_insensitive(state, "RETURNING")) {
    this->set_state(VACUUM_STATE_RETURNING);
  } else if (str_equals_case_insensitive(state, "DOCKING")) {
    this->set_state(VACUUM_STATE_DOCKING);
  } else if (str_equals_case_insensitive(state, "ERROR")) {
    this->set_state(VACUUM_STATE_ERROR);
  } else if (str_equals_case_insensitive(state, "UNKNOWN")) {
    this->set_state(VACUUM_STATE_UNKNOWN);
  } else {
    ESP_LOGW(TAG, "'%s' - Unrecognized state %s", this->parent_->get_name().c_str(), state.c_str());
  }
  return *this;
}

const optional<VacuumState> &VacuumCall::get_state() const { return this->state_; }

}  // namespace vacuum
}  // namespace esphome
