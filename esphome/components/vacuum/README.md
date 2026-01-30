# Vacuum Component

This is a base vacuum component for ESPHome that provides a standard interface for vacuum cleaner devices.

## Features

- Basic vacuum control (start, stop, pause, return to base)
- State tracking (unknown, idle, cleaning, docking, error, paused, returning)
- Battery level monitoring
- Home Assistant integration
- Web server support

## Platform Components

This component requires platform implementations. Currently supported:
- `neato_uart` - Neato vacuum cleaners via UART

## Configuration

### Basic Setup

```yaml
vacuum:
  - platform: neato_uart
    neato_uart_id: neato_device
    name: "Neato Vacuum"
```

## States

Vacuums can be in one of the following states:
- `UNKNOWN` - Initial or unknown state
- `IDLE` - Not cleaning, not docked
- `CLEANING` - Currently cleaning
- `DOCKING` - Returning to dock
- `ERROR` - An error occurred
- `PAUSED` - Cleaning is paused
- `RETURNING` - Returning to dock

## Actions

### Start Cleaning

```yaml
button:
  - platform: template
    name: "Start"
    on_press:
      - vacuum.start:
          id: my_vacuum
```

### Stop Cleaning

```yaml
button:
  - platform: template
    name: "Stop"
    on_press:
      - vacuum.stop:
          id: my_vacuum
```

### Pause

```yaml
button:
  - platform: template
    name: "Pause"
    on_press:
      - vacuum.pause:
          id: my_vacuum
```

### Return to Base

```yaml
button:
  - platform: template
    name: "Return to Base"
    on_press:
      - vacuum.return_to_base:
          id: my_vacuum
```

## Properties

- `state` - Current vacuum state (read-only)
- `battery_level` - Battery level as percentage 0-100 (read-only)

## See Also

- [Neato UART Component](../neato_uart/README.md)
