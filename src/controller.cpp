#include "controller.h"
#include "cheats.h"
#include "tp.h"

#define BUTTON_STATES 12
#define REPEAT_TIME 6
#define REPEAT_DELAY 10

static uint16_t sButtons_down_last_frame = 0;
static uint16_t sButtons_down = 0;
static uint16_t sButtons_pressed = 0;

struct ButtonState {
    uint16_t button;
    uint32_t pressed_frame;
    bool is_down;
};

static ButtonState buttonStates[BUTTON_STATES] = {
    {Controller::Pad::DPAD_LEFT, 0xFFFFFFFF, false},
    {Controller::Pad::DPAD_RIGHT, 0xFFFFFFFF, false},
    {Controller::Pad::DPAD_DOWN, 0xFFFFFFFF, false},
    {Controller::Pad::DPAD_UP, 0xFFFFFFFF, false},
    {Controller::Pad::Z, 0xFFFFFFFF, false},
    {Controller::Pad::R, 0xFFFFFFFF, false},
    {Controller::Pad::L, 0xFFFFFFFF, false},
    {Controller::Pad::A, 0xFFFFFFFF, false},
    {Controller::Pad::B, 0xFFFFFFFF, false},
    {Controller::Pad::X, 0xFFFFFFFF, false},
    {Controller::Pad::Y, 0xFFFFFFFF, false},
    {Controller::Pad::START, 0xFFFFFFFF, false}};

extern "C" uint32_t read_controller() {
    sButtons_down_last_frame = sButtons_down;
    sButtons_down = tp_mPadStatus.sval;
    sButtons_pressed = sButtons_down & (0xFFF ^ sButtons_down_last_frame);

    uint8_t idx = 0;
    for (; idx < BUTTON_STATES; idx++) {
        buttonStates[idx].is_down = (buttonStates[idx].button & sButtons_down) != 0;
        if ((buttonStates[idx].button & sButtons_pressed) != 0) {
            buttonStates[idx].pressed_frame = TP::get_frame_count() + 1;
        }
    }
    Cheats::apply_cheats();
    return 0x80000000;
}

namespace Controller {
    uint16_t buttons_down() {
        return tp_mPadButton.buttons;
    }

    uint16_t buttons_pressed() {
        return tp_mPadButton.buttons_down;
    }

    void set_buttons_down(uint16_t buttons) {
        tp_mPadButton.buttons = buttons;
    }

    void set_buttons_pressed(uint16_t buttons) {
        tp_mPadButton.buttons_down = buttons;
    }

    bool is_down(uint16_t buttons) {
        return (tp_mPadButton.buttons & buttons) == buttons;
    }

    bool is_pressed(uint16_t buttons) {
        return (tp_mPadButton.buttons_down & buttons) == buttons;
    }

    bool Button::is_pressed() {
        uint32_t delta = TP::get_frame_count() - buttonStates[idx].pressed_frame;
        bool just_clicked = delta == 0;
        bool held_down_long_enough = delta > REPEAT_DELAY;
        bool is_repeat_frame = held_down_long_enough && (delta % REPEAT_TIME == 0);

        return is_down() && (just_clicked || is_repeat_frame);
    }

    bool Button::is_down() {
        return buttonStates[idx].is_down;
    }
}  // namespace Controller