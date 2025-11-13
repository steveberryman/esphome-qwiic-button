#include "qwiic_button.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace qwiic_button {

static const char *const TAG = "qwiic_button";

void QwiicButton::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Qwiic Button...");
  
  // Verify device ID
  uint8_t device_id;
  if (!this->read_byte(REG_ID, &device_id)) {
    ESP_LOGE(TAG, "Failed to read device ID");
    this->mark_failed();
    return;
  }
  
  if (device_id != QWIIC_BUTTON_DEV_ID) {
    ESP_LOGE(TAG, "Invalid device ID: 0x%02X (expected 0x%02X)", device_id, QWIIC_BUTTON_DEV_ID);
    this->mark_failed();
    return;
  }
  
  // Read firmware version
  this->read_byte(REG_FIRMWARE_MAJOR, &this->firmware_major_);
  this->read_byte(REG_FIRMWARE_MINOR, &this->firmware_minor_);
  ESP_LOGD(TAG, "Firmware version: %d.%d", this->firmware_major_, this->firmware_minor_);
  
  // Set debounce time
  if (this->debounce_time_ > 0) {
    uint8_t debounce_lsb = this->debounce_time_ & 0xFF;
    uint8_t debounce_msb = (this->debounce_time_ >> 8) & 0xFF;
    this->write_byte(REG_BUTTON_DEBOUNCE_TIME, debounce_lsb);
    this->write_byte(REG_BUTTON_DEBOUNCE_TIME + 1, debounce_msb);
    ESP_LOGD(TAG, "Set debounce time to %d ms", this->debounce_time_);
  }
  
  // Configure interrupts
  uint8_t interrupt_config = 0;
  if (this->enable_pressed_interrupt_) {
    interrupt_config |= INT_PRESSED_ENABLE;
  }
  if (this->enable_clicked_interrupt_) {
    interrupt_config |= INT_CLICKED_ENABLE;
  }
  if (interrupt_config != 0) {
    this->write_byte(REG_INTERRUPT_CONFIG, interrupt_config);
    ESP_LOGD(TAG, "Interrupt config: 0x%02X", interrupt_config);
  }
  
  // Initial state read
  this->update_button_state_();
}

void QwiicButton::loop() {
  this->update_button_state_();
}

void QwiicButton::dump_config() {
  ESP_LOGCONFIG(TAG, "Qwiic Button:");
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG, "  Firmware Version: %d.%d", this->firmware_major_, this->firmware_minor_);
  ESP_LOGCONFIG(TAG, "  Debounce Time: %d ms", this->debounce_time_);
  ESP_LOGCONFIG(TAG, "  Pressed Interrupt: %s", YESNO(this->enable_pressed_interrupt_));
  ESP_LOGCONFIG(TAG, "  Clicked Interrupt: %s", YESNO(this->enable_clicked_interrupt_));
  
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with Qwiic Button failed!");
  }
  
  LOG_BINARY_SENSOR("  ", "Pressed", this->pressed_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Clicked", this->clicked_binary_sensor_);
}

void QwiicButton::update_button_state_() {
  uint8_t status;
  if (!this->read_byte(REG_BUTTON_STATUS, &status)) {
    ESP_LOGW(TAG, "Failed to read button status");
    return;
  }
  
  bool is_pressed = status & STATUS_IS_PRESSED;
  
  // Update pressed state - simple state tracking
  if (this->pressed_binary_sensor_ != nullptr && is_pressed != this->last_pressed_state_) {
    this->pressed_binary_sensor_->publish_state(is_pressed);
    this->last_pressed_state_ = is_pressed;
    ESP_LOGD(TAG, "Button %s", is_pressed ? "pressed" : "released");
  }
  
  // Update clicked state - check queue directly instead of flag
  if (this->clicked_binary_sensor_ != nullptr) {
    uint8_t clicked_queue_status;
    if (this->read_byte(REG_CLICKED_QUEUE_STATUS, &clicked_queue_status)) {
      // If there's an item in the clicked queue, we have a new click
      if (!(clicked_queue_status & QUEUE_IS_EMPTY)) {
        // Pop the queue item
        this->pop_clicked_queue();
        
        // Fire the clicked event
        this->clicked_binary_sensor_->publish_state(true);
        ESP_LOGD(TAG, "Button clicked");
        
        // Schedule a reset of the clicked state
        this->set_timeout(50, [this]() {
          this->clicked_binary_sensor_->publish_state(false);
        });
      }
    }
  }
}

void QwiicButton::set_led_brightness(uint8_t brightness) {
  if (!this->write_byte(REG_LED_BRIGHTNESS, brightness)) {
    ESP_LOGW(TAG, "Failed to set LED brightness");
    return;
  }
  ESP_LOGD(TAG, "Set LED brightness to %d", brightness);
}

void QwiicButton::set_led_pulse(uint16_t cycle_time, uint16_t off_time, uint8_t granularity) {
  this->write_byte(REG_LED_PULSE_GRANULARITY, granularity);
  
  uint8_t cycle_lsb = cycle_time & 0xFF;
  uint8_t cycle_msb = (cycle_time >> 8) & 0xFF;
  this->write_byte(REG_LED_PULSE_CYCLE_TIME, cycle_lsb);
  this->write_byte(REG_LED_PULSE_CYCLE_TIME + 1, cycle_msb);
  
  uint8_t off_lsb = off_time & 0xFF;
  uint8_t off_msb = (off_time >> 8) & 0xFF;
  this->write_byte(REG_LED_PULSE_OFF_TIME, off_lsb);
  this->write_byte(REG_LED_PULSE_OFF_TIME + 1, off_msb);
  
  ESP_LOGD(TAG, "Set LED pulse: cycle=%d, off=%d, granularity=%d", cycle_time, off_time, granularity);
}

void QwiicButton::set_led_off() {
  this->set_led_brightness(0);
}

void QwiicButton::set_led_on() {
  this->set_led_brightness(255);
}

bool QwiicButton::is_pressed() {
  uint8_t status;
  if (!this->read_byte(REG_BUTTON_STATUS, &status)) {
    return false;
  }
  return status & STATUS_IS_PRESSED;
}

bool QwiicButton::has_been_clicked() {
  uint8_t status;
  if (!this->read_byte(REG_BUTTON_STATUS, &status)) {
    return false;
  }
  return status & STATUS_HAS_BEEN_CLICKED;
}

bool QwiicButton::is_event_available() {
  uint8_t status;
  if (!this->read_byte(REG_BUTTON_STATUS, &status)) {
    return false;
  }
  return status & STATUS_EVENT_AVAILABLE;
}

uint32_t QwiicButton::get_pressed_queue_front() {
  uint8_t data[4];
  if (!this->read_bytes(REG_PRESSED_QUEUE_FRONT, data, 4)) {
    return 0;
  }
  return (uint32_t)data[0] | ((uint32_t)data[1] << 8) | 
         ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

uint32_t QwiicButton::get_clicked_queue_front() {
  uint8_t data[4];
  if (!this->read_bytes(REG_CLICKED_QUEUE_FRONT, data, 4)) {
    return 0;
  }
  return (uint32_t)data[0] | ((uint32_t)data[1] << 8) | 
         ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

void QwiicButton::pop_pressed_queue() {
  uint8_t status;
  if (this->read_byte(REG_PRESSED_QUEUE_STATUS, &status)) {
    status |= QUEUE_POP_REQUEST;
    this->write_byte(REG_PRESSED_QUEUE_STATUS, status);
  }
}

void QwiicButton::pop_clicked_queue() {
  uint8_t status;
  if (this->read_byte(REG_CLICKED_QUEUE_STATUS, &status)) {
    status |= QUEUE_POP_REQUEST;
    this->write_byte(REG_CLICKED_QUEUE_STATUS, status);
  }
}

}  // namespace qwiic_button
}  // namespace esphome
