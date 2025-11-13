#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"
#include "../qwiic_button.h"

namespace esphome {
namespace qwiic_button {

class QwiicButtonLEDSwitch : public switch_::Switch, public Component {
 public:
  void set_parent(QwiicButton *parent) { this->parent_ = parent; }
  void set_brightness(uint8_t brightness) { this->brightness_ = brightness; }

 protected:
  void write_state(bool state) override {
    if (this->parent_ != nullptr) {
      if (state) {
        this->parent_->set_led_brightness(this->brightness_);
      } else {
        this->parent_->set_led_off();
      }
    }
  }

  QwiicButton *parent_{nullptr};
  uint8_t brightness_{255};
};

}  // namespace qwiic_button
}  // namespace esphome
