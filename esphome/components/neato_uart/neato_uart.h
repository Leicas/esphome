#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/vacuum/vacuum.h"
#include <vector>
#include <string>

namespace esphome {
namespace neato_uart {

class NeatoUARTComponent : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Send command to vacuum
  void send_command(const std::string &command);
  void play_sound(uint8_t sound_id);

  // Getters for initial ready state
  bool is_initial_ready() const { return this->initial_ready_; }

  // Sensor setters - GetCharger
  void set_fuel_percent_sensor(sensor::Sensor *sensor) { this->fuel_percent_sensor_ = sensor; }
  void set_battery_temp_sensor(sensor::Sensor *sensor) { this->battery_temp_sensor_ = sensor; }
  void set_battery_voltage_sensor(sensor::Sensor *sensor) { this->battery_voltage_sensor_ = sensor; }
  void set_external_voltage_sensor(sensor::Sensor *sensor) { this->external_voltage_sensor_ = sensor; }
  void set_charger_mah_sensor(sensor::Sensor *sensor) { this->charger_mah_sensor_ = sensor; }
  void set_discharge_mah_sensor(sensor::Sensor *sensor) { this->discharge_mah_sensor_ = sensor; }

  // Sensor setters - GetWarranty
  void set_battery_cycles_sensor(sensor::Sensor *sensor) { this->battery_cycles_sensor_ = sensor; }

  // Sensor setters - GetUserSettings
  void set_filter_change_time_sensor(sensor::Sensor *sensor) { this->filter_change_time_sensor_ = sensor; }
  void set_brush_change_time_sensor(sensor::Sensor *sensor) { this->brush_change_time_sensor_ = sensor; }
  void set_dirt_bin_alert_sensor(sensor::Sensor *sensor) { this->dirt_bin_alert_sensor_ = sensor; }
  void set_current_dirt_bin_runtime_sensor(sensor::Sensor *sensor) { this->current_dirt_bin_runtime_sensor_ = sensor; }
  void set_number_dust_bin_full_sensor(sensor::Sensor *sensor) { this->number_dust_bin_full_sensor_ = sensor; }

  // Binary Sensor setters - GetErr
  void set_usb_connected_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->usb_connected_binary_sensor_ = sensor;
  }

  // Binary Sensor setters - GetCharger
  void set_battery_over_temp_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->battery_over_temp_binary_sensor_ = sensor;
  }
  void set_charging_active_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->charging_active_binary_sensor_ = sensor;
  }
  void set_charging_enabled_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->charging_enabled_binary_sensor_ = sensor;
  }
  void set_confident_on_fuel_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->confident_on_fuel_binary_sensor_ = sensor;
  }
  void set_on_reserved_fuel_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->on_reserved_fuel_binary_sensor_ = sensor;
  }
  void set_empty_fuel_binary_sensor(binary_sensor::BinarySensor *sensor) { this->empty_fuel_binary_sensor_ = sensor; }
  void set_battery_failure_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->battery_failure_binary_sensor_ = sensor;
  }
  void set_ext_power_present_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->ext_power_present_binary_sensor_ = sensor;
  }
  void set_thermistor_present_binary_sensor(binary_sensor::BinarySensor *sensor) {
    this->thermistor_present_binary_sensor_ = sensor;
  }

  // Text Sensor setters
  void set_robot_error_text_sensor(text_sensor::TextSensor *sensor) { this->robot_error_text_sensor_ = sensor; }
  void set_robot_alert_text_sensor(text_sensor::TextSensor *sensor) { this->robot_alert_text_sensor_ = sensor; }
  void set_ui_state_text_sensor(text_sensor::TextSensor *sensor) { this->ui_state_text_sensor_ = sensor; }
  void set_robot_state_text_sensor(text_sensor::TextSensor *sensor) { this->robot_state_text_sensor_ = sensor; }
  void set_mainboard_serial_text_sensor(text_sensor::TextSensor *sensor) {
    this->mainboard_serial_text_sensor_ = sensor;
  }
  void set_mainboard_version_text_sensor(text_sensor::TextSensor *sensor) {
    this->mainboard_version_text_sensor_ = sensor;
  }
  void set_robot_serial_text_sensor(text_sensor::TextSensor *sensor) { this->robot_serial_text_sensor_ = sensor; }
  void set_robot_model_text_sensor(text_sensor::TextSensor *sensor) { this->robot_model_text_sensor_ = sensor; }
  void set_language_text_sensor(text_sensor::TextSensor *sensor) { this->language_text_sensor_ = sensor; }
  void set_last_cleaning_time_text_sensor(text_sensor::TextSensor *sensor) {
    this->last_cleaning_time_text_sensor_ = sensor;
  }
  void set_last_cleaning_type_text_sensor(text_sensor::TextSensor *sensor) {
    this->last_cleaning_type_text_sensor_ = sensor;
  }
  void set_last_cleaning_duration_sensor(sensor::Sensor *sensor) { this->last_cleaning_duration_sensor_ = sensor; }
  void set_time_id(time::RealTimeClock *time_id) { this->time_id_ = time_id; }

 protected:
  // UART reading and parsing
  void read_uart_data_();
  void process_buffer_();
  void parse_message_(const std::string &message);

  // Individual command parsers
  void parse_get_err_(const std::vector<std::string> &lines);
  void parse_get_charger_(const std::vector<std::string> &lines);
  void parse_get_warranty_(const std::vector<std::string> &lines);
  void parse_get_version_(const std::vector<std::string> &lines);
  void parse_test_mode_(const std::vector<std::string> &lines);
  void parse_get_state_(const std::vector<std::string> &lines);
  void handle_cleaning_state_change_(const std::string &new_state);
  void parse_get_user_settings_(const std::vector<std::string> &lines);

  // Helper functions
  std::vector<std::string> split_lines_(const std::string &str);
  void trim_(std::string &s);
  uint32_t hex_to_uint32_(const std::string &hex);

  // UART buffer
  std::vector<uint8_t> rx_buffer_;

  // State tracking
  bool initial_ready_{false};
  std::string last_ui_state_;

  // Sensors - GetCharger
  sensor::Sensor *fuel_percent_sensor_{nullptr};
  sensor::Sensor *battery_temp_sensor_{nullptr};
  sensor::Sensor *battery_voltage_sensor_{nullptr};
  sensor::Sensor *external_voltage_sensor_{nullptr};
  sensor::Sensor *charger_mah_sensor_{nullptr};
  sensor::Sensor *discharge_mah_sensor_{nullptr};

  // Sensors - GetWarranty
  sensor::Sensor *battery_cycles_sensor_{nullptr};

  // Sensors - GetUserSettings
  sensor::Sensor *filter_change_time_sensor_{nullptr};
  sensor::Sensor *brush_change_time_sensor_{nullptr};
  sensor::Sensor *dirt_bin_alert_sensor_{nullptr};
  sensor::Sensor *current_dirt_bin_runtime_sensor_{nullptr};
  sensor::Sensor *number_dust_bin_full_sensor_{nullptr};

  // Binary Sensors - GetErr
  binary_sensor::BinarySensor *usb_connected_binary_sensor_{nullptr};

  // Binary Sensors - GetCharger
  binary_sensor::BinarySensor *battery_over_temp_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *charging_active_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *charging_enabled_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *confident_on_fuel_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *on_reserved_fuel_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *empty_fuel_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *battery_failure_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *ext_power_present_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *thermistor_present_binary_sensor_{nullptr};

  // Text Sensors
  text_sensor::TextSensor *robot_error_text_sensor_{nullptr};
  text_sensor::TextSensor *robot_alert_text_sensor_{nullptr};
  text_sensor::TextSensor *ui_state_text_sensor_{nullptr};
  text_sensor::TextSensor *robot_state_text_sensor_{nullptr};
  text_sensor::TextSensor *mainboard_serial_text_sensor_{nullptr};
  text_sensor::TextSensor *mainboard_version_text_sensor_{nullptr};
  text_sensor::TextSensor *robot_serial_text_sensor_{nullptr};
  text_sensor::TextSensor *robot_model_text_sensor_{nullptr};
  text_sensor::TextSensor *language_text_sensor_{nullptr};
  text_sensor::TextSensor *last_cleaning_time_text_sensor_{nullptr};
  text_sensor::TextSensor *last_cleaning_type_text_sensor_{nullptr};
  sensor::Sensor *last_cleaning_duration_sensor_{nullptr};

  // Time integration
  time::RealTimeClock *time_id_{nullptr};

  // Cleaning session tracking
  uint64_t cleaning_start_time_{0};
  std::string last_cleaning_type_stored_;
};

// Action templates
template<typename... Ts> class SendCommandAction : public Action<Ts...> {
 public:
  SendCommandAction(NeatoUARTComponent *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(std::string, command)

  void play(const Ts &...x) override { this->parent_->send_command(this->command_.value(x...)); }

 protected:
  NeatoUARTComponent *parent_;
};

template<typename... Ts> class PlaySoundAction : public Action<Ts...> {
 public:
  PlaySoundAction(NeatoUARTComponent *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(uint8_t, sound_id)

  void play(const Ts &...x) override { this->parent_->play_sound(this->sound_id_.value(x...)); }

 protected:
  NeatoUARTComponent *parent_;
};

class NeatoVacuum : public vacuum::Vacuum, public Component {
 public:
  NeatoVacuum(NeatoUARTComponent *parent) : parent_(parent) {}

  void setup() override {}
  void dump_config() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void control(const vacuum::VacuumCall &call) override {
    if (!call.get_state().has_value()) {
      return;
    }
    switch (*call.get_state()) {
      case vacuum::VACUUM_STATE_CLEANING:
        this->parent_->send_command("Clean");
        break;
      case vacuum::VACUUM_STATE_IDLE:
        this->parent_->send_command("Clean Stop");
        break;
      case vacuum::VACUUM_STATE_PAUSED:
        this->parent_->send_command("Clean Pause");
        break;
      case vacuum::VACUUM_STATE_RETURNING:
      case vacuum::VACUUM_STATE_DOCKING:
        this->parent_->send_command("Clean GoBase");
        break;
      default:
        break;
    }
  }

 protected:
  NeatoUARTComponent *parent_;
};

}  // namespace neato_uart
}  // namespace esphome
