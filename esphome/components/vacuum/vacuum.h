#pragma once

#include "esphome/core/component.h"
#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include <initializer_list>

namespace esphome {
namespace vacuum {

/// Enum encoding the current state of a vacuum cleaner.
enum VacuumState : uint32_t {
  VACUUM_STATE_UNKNOWN = 0,
  VACUUM_STATE_IDLE = 1,
  VACUUM_STATE_CLEANING = 2,
  VACUUM_STATE_DOCKING = 3,
  VACUUM_STATE_ERROR = 4,
  VACUUM_STATE_PAUSED = 5,
  VACUUM_STATE_RETURNING = 6,
};

const LogString *vacuum_state_to_str(VacuumState state);

class Vacuum;

class VacuumTraits {
 public:
  VacuumTraits() = default;

  bool get_assumed_state() const { return this->assumed_state_; }
  void set_assumed_state(bool assumed_state) { this->assumed_state_ = assumed_state; }

  bool supports_state(VacuumState state) const { return supported_states_mask_ & (1U << state); }
  void set_supported_states(std::initializer_list<VacuumState> states) {
    supported_states_mask_ = 0;
    for (auto state : states) {
      supported_states_mask_ |= (1U << state);
    }
  }
  uint32_t get_supported_states_mask() const { return supported_states_mask_; }
  void set_supported_states_mask(uint32_t mask) { supported_states_mask_ = mask; }
  void add_supported_state(VacuumState state) { supported_states_mask_ |= (1U << state); }

 protected:
  bool assumed_state_{false};
  uint32_t supported_states_mask_{(1U << VACUUM_STATE_UNKNOWN) | (1U << VACUUM_STATE_IDLE) |
                                  (1U << VACUUM_STATE_CLEANING) | (1U << VACUUM_STATE_DOCKING) |
                                  (1U << VACUUM_STATE_ERROR) | (1U << VACUUM_STATE_PAUSED) |
                                  (1U << VACUUM_STATE_RETURNING)};
};

class VacuumCall {
 public:
  explicit VacuumCall(Vacuum *parent) : parent_(parent) {}

  VacuumCall &set_state(VacuumState state);
  VacuumCall &set_state(optional<VacuumState> state);
  VacuumCall &set_state(const std::string &state);

  void perform();

  const optional<VacuumState> &get_state() const;

 protected:
  void validate_();

  Vacuum *const parent_;
  optional<VacuumState> state_;
};

/** Base class for all vacuum cleaner devices.
 *
 * Vacuums have the following properties:
 *  - state - The current state of the vacuum (idle, cleaning, docking, error, paused, returning)
 *  - battery_level - The current battery level as a percentage (0-100)
 *
 * For users: All vacuum operations must be performed over action calls.
 * For integrations: Integrations must implement the control methods:
 *  - start() - Start cleaning
 *  - stop() - Stop cleaning
 *  - pause() - Pause cleaning
 *  - return_to_base() - Return to dock/base
 */
class Vacuum : public EntityBase, public EntityBase_DeviceClass {
 public:
  explicit Vacuum();

  /// The current state of the vacuum
  VacuumState state{VACUUM_STATE_UNKNOWN};
  /// The current battery level (0-100)
  float battery_level{-1.0f};

  VacuumTraits traits;

  VacuumCall make_call();

  void add_on_state_callback(std::function<void(VacuumState)> &&f) { this->state_callback_.add(std::move(f)); }

  /// Publish the current state of the vacuum
  void publish_state(bool save = false);

  /// Virtual methods to be implemented by vacuums
  void start();
  void stop();
  void pause();
  void return_to_base();

 protected:
  friend VacuumCall;

  void set_state_(VacuumState state);

  virtual void control(const VacuumCall &call) = 0;

  CallbackManager<void(VacuumState)> state_callback_;
  Deduplicator<VacuumState> publish_dedup_;
};

}  // namespace vacuum
}  // namespace esphome
