# ESPHome Qwiic Button Component

ESPHome integration for SparkFun Qwiic Button with LED control.

## Installation
```yaml
external_components:
  - source:
      type: git
      url: https://github.com/YOUR-USERNAME/esphome-qwiic-button
      ref: v1.0.0
    components: [qwiic_button]

i2c:
  sda: GPIO21
  scl: GPIO22

qwiic_button:
  - id: my_button
    pressed:
      name: "Button"

switch:
  - platform: qwiic_button
    qwiic_button_id: my_button
    name: "Button LED"
```

## Features

✅ Button press/release detection  
✅ Click event detection  
✅ LED brightness control (0-255)  
✅ LED pulse effects  
✅ Multiple button support  
✅ Full Home Assistant integration  

## Examples

See [examples/](examples/) for complete configurations.

## Documentation

Full documentation in [components/qwiic_button/README.md](components/qwiic_button/README.md)

## Hardware

Compatible with:
- SparkFun Qwiic Button - Red LED
- SparkFun Qwiic Button - Green LED
- SparkFun Qwiic Button Breakout

## License

MIT License - See LICENSE file
Create LICENSE file
Create my-component-repo/LICENSE:
MIT License

Copyright (c) 2024 [Your Name]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
