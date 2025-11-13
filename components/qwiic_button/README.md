# ESPHome Qwiic Button Component

A custom ESPHome component for the SparkFun Qwiic Button with full support for button events, LED control, and FIFO queues.

## Features

- ✅ Button press and click detection
- ✅ Binary sensors for pressed and clicked states
- ✅ Configurable debounce time
- ✅ LED brightness control (0-255)
- ✅ LED pulsing effects
- ✅ Interrupt support (hardware interrupts)
- ✅ FIFO queue support for event timestamps
- ✅ Multiple buttons on same I2C bus (configurable addresses)
- ✅ Compatible with all SparkFun Qwiic Button variants (Red, Green, Breakout)

## Hardware

This component works with:
- [SparkFun Qwiic Button - Red LED (BOB-15932)](https://www.sparkfun.com/products/15932)
- [SparkFun Qwiic Button - Green LED (BOB-16842)](https://www.sparkfun.com/products/16842)
- [SparkFun Qwiic Button Breakout (BOB-15584)](https://www.sparkfun.com/products/15584)

Default I2C address: **0x6F** (configurable via solder jumpers)

## Installation

### Method 1: Local Component (Recommended for Development)

1. Copy the `qwiic_button` folder to your ESPHome configuration directory under `components/`:

```
config/
├── components/
│   └── qwiic_button/
│       ├── __init__.py
│       ├── qwiic_button.h
│       ├── qwiic_button.cpp
│       └── switch/
│           ├── __init__.py
│           └── qwiic_button_led_switch.h
└── your_device.yaml
```

2. Reference it in your YAML:

```yaml
external_components:
  - source:
      type: local
      path: components
    components: [qwiic_button]
```

### Method 2: GitHub Repository (When Published)

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/your-username/esphome-qwiic-button
      ref: main
    components: [qwiic_button]
```

## Basic Configuration

```yaml
# Configure I2C bus
i2c:
  sda: GPIO21  # Adjust for your board
  scl: GPIO22  # Adjust for your board
  scan: true

# Add the Qwiic Button
qwiic_button:
  - id: my_button
    address: 0x6F  # Default address
    debounce_time: 10  # Optional, default 10ms
    
    # Button state sensors
    pressed:
      name: "Button Pressed"
      on_press:
        - logger.log: "Button pressed!"
      on_release:
        - logger.log: "Button released!"
    
    clicked:
      name: "Button Clicked"
      on_press:
        - logger.log: "Button clicked!"

# Optional: Control the LED
switch:
  - platform: qwiic_button
    qwiic_button_id: my_button
    name: "Button LED"
    brightness: 255  # 0-255
```

## Configuration Variables

### Main Component

- **id** (*Required*, ID): The ID for this component
- **address** (*Optional*, int): I2C address (default: 0x6F)
- **debounce_time** (*Optional*, int): Debounce time in milliseconds (0-65535, default: 10)
- **enable_pressed_interrupt** (*Optional*, boolean): Enable interrupt on press (default: false)
- **enable_clicked_interrupt** (*Optional*, boolean): Enable interrupt on click (default: false)
- **use_interrupt** (*Optional*, boolean): Use interrupt pin (requires hardware connection, default: false)
- **pressed** (*Optional*): Binary sensor configuration for button pressed state
- **clicked** (*Optional*): Binary sensor configuration for button clicked state

### LED Switch

- **platform** (*Required*): Must be `qwiic_button`
- **qwiic_button_id** (*Required*, ID): The ID of the parent Qwiic Button component
- **name** (*Required*, string): The name for this switch
- **brightness** (*Optional*, int): LED brightness when on (0-255, default: 255)

## Advanced Usage

### Multiple Buttons

You can use multiple Qwiic Buttons on the same I2C bus by changing their addresses via solder jumpers:

```yaml
qwiic_button:
  - id: button_1
    address: 0x6F
    pressed:
      name: "Button 1"
  
  - id: button_2
    address: 0x6E  # Changed via solder jumpers
    pressed:
      name: "Button 2"
  
  - id: button_3
    address: 0x6D
    pressed:
      name: "Button 3"
```

### LED Control with Lambda

You can control the LED directly from lambdas:

```yaml
on_press:
  - lambda: |-
      // Turn LED on at full brightness
      id(my_button).set_led_on();
      
on_release:
  - lambda: |-
      // Turn LED off
      id(my_button).set_led_off();
      
      // Or set specific brightness (0-255)
      id(my_button).set_led_brightness(128);
      
      // Or make it pulse
      // set_led_pulse(cycle_time_ms, off_time_ms, granularity)
      id(my_button).set_led_pulse(1000, 500, 1);
```

### Long Press Detection

```yaml
automation:
  - trigger:
      platform: binary_sensor
      id: my_button_pressed
      on_press:
        for:
          time: 2s  # Hold for 2 seconds
    then:
      - logger.log: "Long press detected!"
      - switch.toggle: some_output
```

### Reading FIFO Queues

The button maintains FIFO queues of press and click timestamps. Access them in lambdas:

```yaml
on_click:
  - lambda: |-
      uint32_t timestamp = id(my_button).get_clicked_queue_front();
      ESP_LOGI("button", "Click timestamp: %u ms", timestamp);
      id(my_button).pop_clicked_queue();
```

## Pin Connections

### Standard Qwiic Connection

| Qwiic Pin | Color | Signal | ESP Pin |
|-----------|-------|--------|---------|
| 1 | Black | GND | GND |
| 2 | Red | 3.3V | 3.3V |
| 3 | Blue | SDA | GPIO21 (ESP32) / GPIO4 (ESP8266) |
| 4 | Yellow | SCL | GPIO22 (ESP32) / GPIO5 (ESP8266) |

### Interrupt Pin (Optional)

If you want to use hardware interrupts, connect the INT pin on the Qwiic Button to a GPIO on your ESP:

```yaml
qwiic_button:
  - id: my_button
    # ... other config ...
    enable_pressed_interrupt: true
    enable_clicked_interrupt: true
    use_interrupt: true
```

Then wire the INT pin to a GPIO and configure it as an interrupt input in your code.

## Troubleshooting

### Button not detected

1. Verify I2C wiring (SDA/SCL not swapped)
2. Check that 3.3V power is connected
3. Enable I2C scanning to see if device appears:
   ```yaml
   i2c:
     scan: true
   ```
4. Check the I2C address is correct (default 0x6F)

### LED not working

1. Verify the LED button is soldered correctly (check polarity)
2. Try setting brightness directly:
   ```yaml
   on_press:
     - lambda: "id(my_button).set_led_brightness(255);"
   ```

### Button presses not registering

1. Increase debounce time: `debounce_time: 50`
2. Check physical button is properly soldered
3. Enable debug logging: `logger: level: DEBUG`

## Register Map Reference

For advanced users who want to access registers directly:

| Register | Address | Description |
|----------|---------|-------------|
| ID | 0x00 | Device ID (0x5D) |
| FIRMWARE_MINOR | 0x01 | Firmware version (minor) |
| FIRMWARE_MAJOR | 0x02 | Firmware version (major) |
| BUTTON_STATUS | 0x03 | Current button status |
| INTERRUPT_CONFIG | 0x04 | Interrupt configuration |
| BUTTON_DEBOUNCE_TIME | 0x05 | Debounce time (LSB + MSB) |
| LED_BRIGHTNESS | 0x19 | LED brightness (0-255) |
| LED_PULSE_GRANULARITY | 0x1A | Pulse timing granularity |
| LED_PULSE_CYCLE_TIME | 0x1B | Pulse cycle time (LSB + MSB) |
| LED_PULSE_OFF_TIME | 0x1D | Pulse off time (LSB + MSB) |

## Examples

See the included example files:
- `qwiic_button_example_basic.yaml` - Simple button with LED
- `qwiic_button_example_advanced.yaml` - Multiple buttons, automations, LED effects

## Contributing

Contributions are welcome! Please open an issue or pull request on GitHub.

## License

This component is open source. See LICENSE file for details.

## Credits

- Based on SparkFun's Qwiic Button hardware and Arduino library
- Component structure inspired by ESPHome's built-in I2C components
- Created for the ESPHome community

## Support

- [ESPHome Documentation](https://esphome.io/)
- [SparkFun Qwiic Button Hookup Guide](https://learn.sparkfun.com/tutorials/sparkfun-qwiic-button-hookup-guide)
- [SparkFun Qwiic Button GitHub](https://github.com/sparkfun/Qwiic_Button)
