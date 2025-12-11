#include "TouchScreen.h"
#include <iostream>
#include "Logger.h"

TouchScreen::TouchScreen() {}

void TouchScreen::onAbsEvent(const input_event& ev) {
    switch (ev.code) {
        case ABS_X:
            current_point_.x = ev.value;
            break;
        case ABS_Y:
            current_point_.y = ev.value;
            break;
        case ABS_PRESSURE:
            current_point_.pressure = ev.value;
            break;
        case ABS_MT_SLOT:
            // 多点触控槽位
            break;
        case ABS_MT_TRACKING_ID:
            current_point_.tracking_id = ev.value;
            current_point_.valid = (ev.value != -1);
            break;
        case ABS_MT_POSITION_X:
            current_point_.x = ev.value;
            break;
        case ABS_MT_POSITION_Y:
            current_point_.y = ev.value;
            break;
        default:
            break;
    }
}

void TouchScreen::onSynEvent(const input_event& ev) {
    if (ev.code == SYN_REPORT) {
        processTouchPoint();
    }
}

void TouchScreen::processTouchPoint() {
    if (current_point_.valid) {
        if (touch_callback_) {
            touch_callback_(current_point_);
        }
        
        std::cout << "触摸点: X=" << current_point_.x 
                  << ", Y=" << current_point_.y 
                  << ", 压力=" << current_point_.pressure 
                  << ", ID=" << current_point_.tracking_id << std::endl;
    }
}