#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace qwiic_button {

// Register addresses
static const uint8_t QWIIC_BUTTON_DEV_ID = 0x5D;
static const uint8_t REG_ID = 0x00;
static const uint8_t REG_FIRMWARE_MINOR = 0x01;
static const uint8_t REG_FIRMWARE_MAJOR = 0x02;
static const uint8_t REG_BUTTON_STATUS = 0x03;
static const uint8_t REG_INTERRUPT_CONFIG = 0x04;
static const uint8_t REG_BUTTON_DEBOUNCE_TIME = 0x05;
static const uint8_t REG_PRESSED_QUEUE_STATUS = 0x07;
static const uint8_t REG_PRESSED_QUEUE_FRONT = 0x08;
static const uint8_t REG_CLICKED_QUEUE_STATUS = 0x10;
static const uint8_t REG_CLICKED_QUEUE_FRONT = 0x11;
static const uint8_t REG_LED_BRIGHTNESS = 0x19;
static const uint8_t REG_LED_PULSE_GRANULARITY = 0x1A;
static const uint8_t REG_LED_PULSE_CYCLE_TIME = 0x1B;
static const uint8_t REG_LED_PULSE_OFF_TIME = 0x1D;
static const uint8_t REG_I2C_ADDRESS = 0x1F;

// Status bits
static const uint8_t STATUS_IS_PRESSED = 0x01;
static const uint8_t STATUS_HAS_BEEN_CLICKED = 0x02;
static const uint8_t STATUS_EVENT_AVAILABLE = 0x04;

// Interrupt config bits
static const uint8_t INT_PRESSED_ENABLE = 0x01;
static const uint8_t INT_CLICKED_ENABLE = 0x02;

// Queue status bits
static const uint8_t QUEUE_IS_EMPTY = 0x01;
static const uint8_t QUEUE_IS_FULL = 0x02;
static const uint8_t QUEUE_POP_REQUEST = 0x04;

class QwiicButton : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Binary sensors
  void set_pressed_binary_sensor(binary_sensor::BinarySensor *pressed_sensor) { 
    pressed_binary_sensor_ = pressed_sensor; 
  }
  void set_clicked_binary_sensor(binary_sensor::BinarySensor *clicked_sensor) { 
    clicked_binary_sensor_ = clicked_sensor; 
  }

  // Configuration
  void set_debounce_time(uint16_t debounce_time) { debounce_time_ = debounce_time; }
  void set_enable_pressed_interrupt(bool enable) { enable_pressed_interrupt_ = enable; }
  void set_enable_clicked_interrupt(bool enable) { enable_clicked_interrupt_ = enable; }
  void set_use_interrupt(bool use) { use_interrupt_ = use; }

  // LED control
  void set_led_brightness(uint8_t brightness);
  void set_led_pulse(uint16_t cycle_time, uint16_t off_time, uint8_t granularity);
  void set_led_off();
  void set_led_on();

  // Button state
  bool is_pressed();
  bool has_been_clicked();
  bool is_event_available();

  // Queue operations
  uint32_t get_pressed_queue_front();
  uint32_t get_clicked_queue_front();
  void pop_pressed_queue();
  void pop_clicked_queue();

 protected:
  binary_sensor::BinarySensor *pressed_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *clicked_binary_sensor_{nullptr};
  
  uint16_t debounce_time_{10};  // Default 10ms
  bool enable_pressed_interrupt_{false};
  bool enable_clicked_interrupt_{false};
  bool use_interrupt_{false};
  
  bool last_pressed_state_{false};
  
  uint8_t firmware_major_{0};
  uint8_t firmware_minor_{0};
  
  void update_button_state_();
};

}  // namespace qwiic_button
}  // namespace esphome
