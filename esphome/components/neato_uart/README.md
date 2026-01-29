# Neato UART Component

This ESPHome component provides clean integration with Neato vacuum cleaners over UART, replacing complex lambda-based parsing with a proper C++ component.

## Features

- **Automatic UART Message Parsing**: Handles all Neato protocol commands (GetErr, GetState, GetCharger, GetVersion, GetWarranty, GetUserSettings, TestMode)
- **Clean YAML Configuration**: No more 300+ line lambdas in your YAML files
- **Comprehensive Sensor Support**: Battery level, temperature, voltage, charging status, and more
- **Status Monitoring**: Robot error, alert, UI state, and robot state tracking
- **Cleaning Session Tracking**: Automatically detects when cleaning starts/stops and tracks duration
- **Control Actions**: Send custom commands and play sounds
- **ESP32-C6 Compatible**: Fully tested on ESP32-C6 with ESP-IDF framework
- **Home Assistant Integration**: Supports time synchronization for accurate cleaning session timestamps

## Hardware Requirements

- ESP32 microcontroller (ESP32-C6 recommended, but works with all ESP32 variants)
- Connection to Neato vacuum UART interface (typically TX, RX, and GND)
- Proper voltage level conversion if needed (Neato uses 3.3V logic)

## Installation

This component is built into the ESPHome distribution. Simply add it to your YAML configuration.

## Configuration

### Basic Setup

```yaml
time:
  - platform: homeassistant
    id: homeassistant_time

uart:
  id: uart_bus
  baud_rate: 115200
  tx_pin: GPIO16
  rx_pin: GPIO17
  rx_buffer_size: 4096

neato_uart:
  id: neato_device
  uart_id: uart_bus
  time_id: homeassistant_time  # Required for cleaning session tracking
```

### Cleaning Session Tracking

To track when the robot starts/stops cleaning, configure the cleaning-related text sensors:

```yaml
text_sensor:
  - platform: neato_uart
    neato_uart_id: neato_device
    ui_state:
      name: "UI State"
    last_cleaning_time:
      name: "Last Cleaning Time"  # When cleaning started
    last_cleaning_type:
      name: "Last Cleaning Type"  # "HOUSE" or "SPOT"

sensor:
  - platform: neato_uart
    neato_uart_id: neato_device
    last_cleaning_duration:
      name: "Last Cleaning Duration"  # In minutes
```

The component automatically detects when the UI state changes to a cleaning state (house or spot cleaning) and tracks the session. When cleaning completes, it calculates the duration and publishes timestamps.

### Sensors

All sensors are optional. Only define the ones you need:

```yaml
sensor:
  - platform: neato_uart
    neato_uart_id: neato_device
    fuel_percent:
      name: "Battery Level"
    battery_temp:
      name: "Battery Temperature"
    battery_voltage:
      name: "Battery Voltage"
    external_voltage:
      name: "Charger Voltage"
    battery_cycles:
      name: "Battery Cycles"
```

**Available sensors:**
- `fuel_percent` - Battery charge percentage (0-100%)
- `battery_temp` - Battery temperature (°C)
- `battery_voltage` - Battery voltage (V)
- `external_voltage` - External/charger voltage (V)
- `charger_mah` - Charger capacity (mAh)
- `discharge_mah` - Discharge capacity (mAh)
- `battery_cycles` - Total battery charge cycles
- `filter_change_time` - Filter replacement countdown (seconds)
- `brush_change_time` - Brush replacement countdown (seconds)
- `dirt_bin_alert` - Dirt bin alert interval (minutes)
- `current_dirt_bin_runtime` - Current dirt bin runtime (seconds)
- `number_dust_bin_full` - Count of full dust bin cleanings
- `last_cleaning_duration` - Duration of last cleaning session in minutes (requires `time_id` configuration)

### Binary Sensors

```yaml
binary_sensor:
  - platform: neato_uart
    neato_uart_id: neato_device
    usb_connected:
      name: "USB Connected"
    charging_active:
      name: "Charging Active"
    battery_over_temp:
      name: "Battery Over Temperature"
```

**Available binary sensors:**
- `usb_connected` - USB connection status
- `battery_over_temp` - Battery overtemperature warning
- `charging_active` - Currently charging
- `charging_enabled` - Charging is enabled
- `confident_on_fuel` - Confident fuel gauge reading
- `on_reserved_fuel` - Running on reserve battery
- `empty_fuel` - Battery empty
- `battery_failure` - Battery failure detected
- `ext_power_present` - External power connected
- `thermistor_present` - Battery thermistor present

### Text Sensors

```yaml
text_sensor:
  - platform: neato_uart
    neato_uart_id: neato_device
    robot_error:
      name: "Robot Error"
    robot_alert:
      name: "Robot Alert"
    ui_state:
      name: "UI State"
    robot_state:
      name: "Robot State"
```

**Available text sensors:**
- `robot_error` - Current error message
- `robot_alert` - Current alert message
- `ui_state` - UI state (e.g., "UIMGR_STATE_IDLE", "UIMGR_STATE_HOUSECLEANINGRUNNING")
- `robot_state` - Robot state
- `mainboard_serial` - Mainboard serial number
- `mainboard_version` - Mainboard firmware version
- `robot_serial` - Robot serial number
- `robot_model` - Robot model name
- `language` - Current language setting
- `last_cleaning_time` - Timestamp of when the last cleaning session started (requires `time_id` configuration)
- `last_cleaning_type` - Type of last cleaning ("HOUSE" or "SPOT")

## Actions

### Send Command

Send any command to the vacuum:

```yaml
button:
  - platform: template
    name: "House Clean"
    on_press:
      - neato_uart.send_command:
          id: neato_device
          command: "Clean"
```

**Common commands:**
- `Clean` - Start house cleaning
- `Clean Spot` - Start spot cleaning
- `Clean Spot Width 200 Height 200` - Spot clean with dimensions (cm)
- `Clean Stop` - Stop cleaning
- `GetErr` - Get error status
- `GetState` - Get current state
- `GetCharger` - Get battery/charger info
- `GetVersion` - Get firmware version
- `GetWarranty` - Get warranty info
- `GetUserSettings` - Get user settings
- `TestMode On/Off` - Enable/disable test mode
- `SetSystemMode Shutdown` - Shutdown robot
- `SetSystemMode PowerCycle` - Restart robot

### Play Sound

Play a sound on the vacuum:

```yaml
button:
  - platform: template
    name: "Locate Robot"
    on_press:
      - neato_uart.play_sound:
          id: neato_device
          sound_id: 20
```

## Full Example

See [example.yaml](example.yaml) for a complete working configuration.

## Troubleshooting

### No data received

1. **Check GPIO pins**: ESP32-C6 uses different default pins than ESP32. Use explicit GPIO numbers (e.g., GPIO16/GPIO17)
2. **Check baud rate**: Must be 115200
3. **Check wiring**: TX → RX, RX → TX, GND → GND
4. **Check voltage levels**: Ensure 3.3V logic levels

### Garbage data

1. **Pin configuration**: Use explicit GPIO numbers instead of symbolic names (TX/RX)
2. **RX buffer size**: Increase if needed (4096 is recommended)
3. **Wiring quality**: Check for loose connections

### Component not found

Make sure you're using a recent ESPHome version that includes this component.

## Protocol Documentation

The Neato vacuum uses a simple text-based protocol over UART at 115200 baud:
- Commands are sent as ASCII text ending with `\n`
- Responses are text lines separated by `\r\n`
- Messages end with delimiter byte `0x1A`

## Contributing

This component is part of the ESPHome project. Contributions and bug reports are welcome!

## License

This component is part of ESPHome and is released under the same license (MIT).

## Credits

- Original lambda-based implementation: philip2809
- Component refactoring: ESPHome community
