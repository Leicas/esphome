#pragma once

#include "esphome/components/vacuum/vacuum.h"
#include "esphome/core/automation.h"

namespace esphome::vacuum {

template<typename... Ts> class StartAction : public Action<Ts...> {
 public:
  explicit StartAction(Vacuum *vacuum) : vacuum_(vacuum) {}
  void play(const Ts &...x) override { this->vacuum_->start(); }

 protected:
  Vacuum *vacuum_;
};

template<typename... Ts> class StopAction : public Action<Ts...> {
 public:
  explicit StopAction(Vacuum *vacuum) : vacuum_(vacuum) {}
  void play(const Ts &...x) override { this->vacuum_->stop(); }

 protected:
  Vacuum *vacuum_;
};

template<typename... Ts> class PauseAction : public Action<Ts...> {
 public:
  explicit PauseAction(Vacuum *vacuum) : vacuum_(vacuum) {}
  void play(const Ts &...x) override { this->vacuum_->pause(); }

 protected:
  Vacuum *vacuum_;
};

template<typename... Ts> class ReturnToBaseAction : public Action<Ts...> {
 public:
  explicit ReturnToBaseAction(Vacuum *vacuum) : vacuum_(vacuum) {}
  void play(const Ts &...x) override { this->vacuum_->return_to_base(); }

 protected:
  Vacuum *vacuum_;
};

class VacuumStateChangeTrigger : public Trigger<VacuumState> {
 public:
  explicit VacuumStateChangeTrigger(Vacuum *vacuum) {
    vacuum->add_on_state_callback([this](VacuumState state) { this->trigger(state); });
  }
};

}  // namespace esphome::vacuum
