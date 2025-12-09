#ifndef TOUCH_SCREEN_H
#define TOUCH_SCREEN_H

#include "InputDevice.h"
#include <map>

struct TouchPoint {
    int x = 0;
    int y = 0;
    int pressure = 0;
    int tracking_id = -1;
    bool valid = false;
};

class TouchScreen : public InputDevice {
public:
    using TouchCallback = std::function<void(const TouchPoint&)>;
    
    TouchScreen();
    
    void setTouchCallback(TouchCallback callback) { touch_callback_ = callback; }
    void setMultiTouchCallback(TouchCallback callback) { multitouch_callback_ = callback; }
    
    // 重写事件处理
    void onAbsEvent(const input_event& ev) override;
    void onSynEvent(const input_event& ev) override;

private:
    void processTouchPoint();
    
    TouchCallback touch_callback_;
    TouchCallback multitouch_callback_;
    TouchPoint current_point_;
    std::map<int, TouchPoint> multi_touch_points_;
};

#endif // TOUCH_SCREEN_H