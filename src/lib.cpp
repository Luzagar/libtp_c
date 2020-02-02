#include "controller.h"
#include "inventory.h"
#include "system.h"
#include "tp.h"

static bool visible = false;

void render() {
    if (Controller::B.is_pressed()) {
        visible = false;
        tp_osReport("Goodbye!");
        return;
    }
    tp_osReport("Hello!");
}

extern "C" void game_loop() {
    char buffer[128];
    sprintf(buffer, "%d", sizeof(TP::GameInfo));
    tp_osReport(buffer);
    bool rt_down = Controller::R.is_down();
    bool lt_down = Controller::L.is_down();
    bool d_down = Controller::DPAD_DOWN.is_down();

    if (visible) {
        render();
    } else if (rt_down && d_down && lt_down && !visible) {
        visible = true;
    } else {
        //something
    }
}