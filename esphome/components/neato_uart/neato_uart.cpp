#include "neato_uart.h"
#include "esphome/core/log.h"
#include <algorithm>
#include <cctype>

namespace esphome {
namespace neato_uart {

static const char *const TAG = "neato_uart";

void NeatoUARTComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Neato UART...");
  this->rx_buffer_.reserve(512);
}

void NeatoUARTComponent::loop() {
  // Read available data from UART
  while (this->available()) {
    uint8_t byte;
    if (this->read_byte(&byte)) {
      this->rx_buffer_.push_back(byte);

      // Check for delimiter (0x1A marks end of message)
      if (byte == 0x1A) {
        this->process_buffer_();
        this->rx_buffer_.clear();
      }

      // Prevent buffer from growing too large
      if (this->rx_buffer_.size() > 4096) {
        ESP_LOGW(TAG, "Buffer overflow, clearing");
        this->rx_buffer_.clear();
      }
    }
  }
}

void NeatoUARTComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Neato UART:");
  this->check_uart_settings(115200);
}

void NeatoUARTComponent::send_command(const std::string &command) {
  std::string cmd = command;
  if (cmd.empty() || cmd.back() != '\n') {
    cmd += "\n";
  }
  this->write_str(cmd.c_str());
  ESP_LOGD(TAG, "Sent command: %s", cmd.c_str());
}

void NeatoUARTComponent::play_sound(uint8_t sound_id) {
  char cmd[32];
  snprintf(cmd, sizeof(cmd), "PlaySound SoundId %u\n", sound_id);
  this->write_str(cmd);
  ESP_LOGD(TAG, "Playing sound: %u", sound_id);
}

void NeatoUARTComponent::process_buffer_() {
  if (this->rx_buffer_.empty()) {
    return;
  }

  // Convert buffer to string
  std::string message(this->rx_buffer_.begin(), this->rx_buffer_.end());
  this->parse_message_(message);
}

void NeatoUARTComponent::parse_message_(const std::string &message) {
  std::vector<std::string> lines = this->split_lines_(message);

  if (lines.empty()) {
    return;
  }

  // First line is the command echo
  std::string command = lines[0];
  this->trim_(command);

  ESP_LOGD(TAG, "Received command: %s", command.c_str());

  // Route to appropriate parser
  if (command == "GetErr") {
    this->parse_get_err_(lines);
  } else if (command == "GetCharger") {
    this->parse_get_charger_(lines);
  } else if (command == "GetWarranty") {
    this->parse_get_warranty_(lines);
  } else if (command == "GetVersion") {
    this->parse_get_version_(lines);
  } else if (command == "TestMode") {
    this->parse_test_mode_(lines);
  } else if (command == "GetState") {
    this->parse_get_state_(lines);
  } else if (command == "GetUserSettings") {
    this->parse_get_user_settings_(lines);
  } else if (command.find("ARCHES Board") != std::string::npos) {
    // Robot starting up
    if (this->ui_state_text_sensor_ != nullptr) {
      this->ui_state_text_sensor_->publish_state("Starting...");
    }
  }
}

void NeatoUARTComponent::parse_get_err_(const std::vector<std::string> &lines) {
  // Format:
  // 0: GetErr
  // 1: Error
  // 2: <error_message>
  // 3: Alert
  // 4: <alert_message>
  // 5: USB state
  // 6: connected/NOT connected

  if (lines.size() < 7) {
    return;
  }

  if (lines[1] != "Error" || lines[3] != "Alert") {
    return;
  }

  if (this->robot_error_text_sensor_ != nullptr) {
    this->robot_error_text_sensor_->publish_state(lines[2]);
  }

  if (this->robot_alert_text_sensor_ != nullptr) {
    this->robot_alert_text_sensor_->publish_state(lines[4]);
  }

  if (this->usb_connected_binary_sensor_ != nullptr) {
    bool connected = lines[6].find("NOT connected") == std::string::npos;
    this->usb_connected_binary_sensor_->publish_state(connected);
  }

  // If we can get errors/alerts, the system is ready
  if (!this->initial_ready_) {
    this->initial_ready_ = true;
    ESP_LOGI(TAG, "Neato vacuum is ready for commands");
  }
}

void NeatoUARTComponent::parse_get_charger_(const std::vector<std::string> &lines) {
  // Format: Label,Value starting from line 2
  for (size_t i = 2; i < lines.size(); i++) {
    std::string line = lines[i];
    size_t comma = line.find(',');
    if (comma == std::string::npos) {
      continue;
    }

    std::string key = line.substr(0, comma);
    std::string value = line.substr(comma + 1);
    this->trim_(key);
    this->trim_(value);

    if (key == "FuelPercent" && this->fuel_percent_sensor_ != nullptr) {
      this->fuel_percent_sensor_->publish_state(atof(value.c_str()));
    } else if (key == "BatteryOverTemp" && this->battery_over_temp_binary_sensor_ != nullptr) {
      this->battery_over_temp_binary_sensor_->publish_state(value == "1");
    } else if (key == "ChargingActive" && this->charging_active_binary_sensor_ != nullptr) {
      this->charging_active_binary_sensor_->publish_state(value == "1");
    } else if (key == "ChargingEnabled" && this->charging_enabled_binary_sensor_ != nullptr) {
      this->charging_enabled_binary_sensor_->publish_state(value == "1");
    } else if (key == "ConfidentOnFuel" && this->confident_on_fuel_binary_sensor_ != nullptr) {
      this->confident_on_fuel_binary_sensor_->publish_state(value == "1");
    } else if (key == "OnReservedFuel" && this->on_reserved_fuel_binary_sensor_ != nullptr) {
      this->on_reserved_fuel_binary_sensor_->publish_state(value == "1");
    } else if (key == "EmptyFuel" && this->empty_fuel_binary_sensor_ != nullptr) {
      this->empty_fuel_binary_sensor_->publish_state(value == "1");
    } else if (key == "BatteryFailure" && this->battery_failure_binary_sensor_ != nullptr) {
      this->battery_failure_binary_sensor_->publish_state(value == "1");
    } else if (key == "ExtPwrPresent" && this->ext_power_present_binary_sensor_ != nullptr) {
      this->ext_power_present_binary_sensor_->publish_state(value == "1");
    } else if (key == "ThermistorPresent" && this->thermistor_present_binary_sensor_ != nullptr) {
      this->thermistor_present_binary_sensor_->publish_state(value == "1");
    } else if (key == "BattTempCAvg" && this->battery_temp_sensor_ != nullptr) {
      this->battery_temp_sensor_->publish_state(atof(value.c_str()));
    } else if (key == "VBattV" && this->battery_voltage_sensor_ != nullptr) {
      this->battery_voltage_sensor_->publish_state(atof(value.c_str()));
    } else if (key == "VExtV" && this->external_voltage_sensor_ != nullptr) {
      this->external_voltage_sensor_->publish_state(atof(value.c_str()));
    } else if (key == "Charger_mAH" && this->charger_mah_sensor_ != nullptr) {
      this->charger_mah_sensor_->publish_state(atof(value.c_str()));
    } else if (key == "Discharge_mAH" && this->discharge_mah_sensor_ != nullptr) {
      this->discharge_mah_sensor_->publish_state(atof(value.c_str()));
    }
  }
}

void NeatoUARTComponent::parse_get_warranty_(const std::vector<std::string> &lines) {
  // Format: Item,Value starting from line 2
  for (size_t i = 2; i < lines.size(); i++) {
    std::string line = lines[i];
    size_t comma = line.find(',');
    if (comma == std::string::npos) {
      continue;
    }

    std::string key = line.substr(0, comma);
    std::string value = line.substr(comma + 1);
    this->trim_(key);
    this->trim_(value);

    if (key == "CumulativeBatteryCycles" && this->battery_cycles_sensor_ != nullptr) {
      uint32_t cycles = this->hex_to_uint32_(value);
      this->battery_cycles_sensor_->publish_state(cycles);
    }
  }
}

void NeatoUARTComponent::parse_get_version_(const std::vector<std::string> &lines) {
  for (const auto &line : lines) {
    size_t comma = line.find(',');
    if (comma == std::string::npos) {
      continue;
    }

    std::string key = line.substr(0, comma);
    std::string value = line.substr(comma + 1);
    this->trim_(key);
    this->trim_(value);

    if (key == "MainBoard Serial Number" && this->mainboard_serial_text_sensor_ != nullptr) {
      this->mainboard_serial_text_sensor_->publish_state(value);
    } else if (key == "MainBoard Version" && this->mainboard_version_text_sensor_ != nullptr) {
      this->mainboard_version_text_sensor_->publish_state(value);
    } else if (key == "Serial Number" && this->robot_serial_text_sensor_ != nullptr) {
      this->robot_serial_text_sensor_->publish_state(value);
    } else if (key == "Model" && this->robot_model_text_sensor_ != nullptr) {
      this->robot_model_text_sensor_->publish_state(value);
    }
  }
}

void NeatoUARTComponent::parse_test_mode_(const std::vector<std::string> &lines) {
  if (lines.size() < 2) {
    return;
  }
  // TestMode response: "TestMode: On" or "TestMode: Off"
  ESP_LOGD(TAG, "TestMode status: %s", lines[1].c_str());
}

void NeatoUARTComponent::parse_get_state_(const std::vector<std::string> &lines) {
  if (lines.size() < 2) {
    return;
  }

  std::string states_line = lines[1];
  size_t sep = states_line.find('\n');
  if (sep == std::string::npos) {
    return;
  }

  std::string ui_line = states_line.substr(0, sep);
  std::string robot_line = states_line.substr(sep + 1);

  static const char *prefix_ui = "Current UI State is: ";
  static const char *prefix_robot = "Current Robot State is: ";

  // Extract UI state
  if (ui_line.find(prefix_ui) == 0) {
    std::string ui_state = ui_line.substr(strlen(prefix_ui));
    this->trim_(ui_state);

    if (this->ui_state_text_sensor_ != nullptr) {
      this->ui_state_text_sensor_->publish_state(ui_state);
    }

    // Detect cleaning session start/stop
    this->handle_cleaning_state_change_(ui_state);

    this->last_ui_state_ = ui_state;
  }

  // Extract Robot state
  if (robot_line.find(prefix_robot) == 0) {
    std::string robot_state = robot_line.substr(strlen(prefix_robot));
    this->trim_(robot_state);

    if (this->robot_state_text_sensor_ != nullptr) {
      this->robot_state_text_sensor_->publish_state(robot_state);
    }
  }
}

void NeatoUARTComponent::handle_cleaning_state_change_(const std::string &new_state) {
  // Check if cleaning just started
  bool old_spot = (this->last_ui_state_.find("SPOTCLEANING") != std::string::npos);
  bool old_house = (this->last_ui_state_.find("HOUSECLEANING") != std::string::npos);
  bool old_cleaning = old_spot || old_house;

  bool new_spot = (new_state.find("SPOTCLEANING") != std::string::npos);
  bool new_house = (new_state.find("HOUSECLEANING") != std::string::npos);
  bool new_cleaning = new_spot || new_house;

  // Cleaning started
  if (!old_cleaning && new_cleaning) {
    this->cleaning_start_time_ = 0;
    if (this->time_id_ != nullptr && this->time_id_->now().is_valid()) {
      this->cleaning_start_time_ = this->time_id_->now().timestamp;
    }
    
    // Record cleaning type
    this->last_cleaning_type_stored_ = new_spot ? "SPOT" : "HOUSE";
    
    if (this->last_cleaning_type_text_sensor_ != nullptr) {
      this->last_cleaning_type_text_sensor_->publish_state(this->last_cleaning_type_stored_);
    }
    
    ESP_LOGD(TAG, "Cleaning started: %s", this->last_cleaning_type_stored_.c_str());
  }
  // Cleaning stopped
  else if (old_cleaning && !new_cleaning) {
    if (this->cleaning_start_time_ > 0 && this->time_id_ != nullptr && this->time_id_->now().is_valid()) {
      uint64_t now_ts = this->time_id_->now().timestamp;
      uint32_t duration_min = (now_ts - this->cleaning_start_time_) / 60;
      
      if (this->last_cleaning_duration_sensor_ != nullptr) {
        this->last_cleaning_duration_sensor_->publish_state(duration_min);
      }
      
      // Format and publish last cleaning time
      if (this->last_cleaning_time_text_sensor_ != nullptr && this->time_id_ != nullptr) {
        ESPTime t = ESPTime::from_epoch_local(this->cleaning_start_time_);
        std::string ts = t.strftime("%Y-%m-%d %H:%M:%S");
        this->last_cleaning_time_text_sensor_->publish_state(ts);
      }
      
      ESP_LOGD(TAG, "Cleaning stopped after %u minutes", duration_min);
    }
    
    this->cleaning_start_time_ = 0;
  }
}

void NeatoUARTComponent::parse_get_user_settings_(const std::vector<std::string> &lines) {
  for (const auto &line_orig : lines) {
    std::string line = line_orig;
    this->trim_(line);

    // Skip binary/invalid lines
    if (!line.empty() && static_cast<unsigned char>(line[0]) >= 0x80) {
      continue;
    }

    // Key,Value lines
    size_t comma = line.find(',');
    if (comma != std::string::npos) {
      std::string key = line.substr(0, comma);
      std::string value = line.substr(comma + 1);
      this->trim_(key);
      this->trim_(value);

      if (key == "Language" && this->language_text_sensor_ != nullptr) {
        this->language_text_sensor_->publish_state(value);
      } else if (key == "Filter Change Time (seconds)" && this->filter_change_time_sensor_ != nullptr) {
        this->filter_change_time_sensor_->publish_state(atof(value.c_str()));
      } else if (key == "Brush Change Time (seconds)" && this->brush_change_time_sensor_ != nullptr) {
        this->brush_change_time_sensor_->publish_state(atof(value.c_str()));
      } else if (key == "Dirt Bin Alert Reminder Interval (minutes)" && this->dirt_bin_alert_sensor_ != nullptr) {
        this->dirt_bin_alert_sensor_->publish_state(atof(value.c_str()));
      }
    }
    // Special lines without commas
    else if (line.find("Current Dirt Bin Runtime is:") == 0) {
      std::string val = line.substr(strlen("Current Dirt Bin Runtime is:"));
      this->trim_(val);
      if (this->current_dirt_bin_runtime_sensor_ != nullptr) {
        this->current_dirt_bin_runtime_sensor_->publish_state(atof(val.c_str()));
      }
    } else if (line.find("Number of Cleanings where Dust Bin was Full is:") == 0) {
      std::string val = line.substr(strlen("Number of Cleanings where Dust Bin was Full is:"));
      this->trim_(val);
      if (this->number_dust_bin_full_sensor_ != nullptr) {
        this->number_dust_bin_full_sensor_->publish_state(atof(val.c_str()));
      }
    }
  }
}

// Helper functions
std::vector<std::string> NeatoUARTComponent::split_lines_(const std::string &str) {
  std::vector<std::string> lines;
  size_t start = 0;
  size_t end = 0;

  while ((end = str.find("\r\n", start)) != std::string::npos) {
    std::string line = str.substr(start, end - start);
    this->trim_(line);
    if (!line.empty()) {
      lines.push_back(line);
    }
    start = end + 2;
  }

  if (start < str.size()) {
    std::string last = str.substr(start);
    this->trim_(last);
    if (!last.empty()) {
      lines.push_back(last);
    }
  }

  return lines;
}

void NeatoUARTComponent::trim_(std::string &s) {
  // Trim leading whitespace
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
  // Trim trailing whitespace
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

uint32_t NeatoUARTComponent::hex_to_uint32_(const std::string &hex) {
  return static_cast<uint32_t>(strtoul(hex.c_str(), nullptr, 16));
}

}  // namespace neato_uart
}  // namespace esphome
