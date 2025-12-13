#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

#include <string>
#include <vector>
#include <functional>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

class InputDevice {
public:
    // 事件回调函数类型
    using EventCallback = std::function<void(const input_event&)>;
    
    // 设备信息结构体
    struct DeviceInfo {
        std::string path;
        std::string name;
        std::string physical_path;
        struct input_id id;
        std::vector<unsigned int> supported_events;
    };

    InputDevice();
    virtual ~InputDevice();
    // 禁止拷贝
    InputDevice(const InputDevice&) = delete;
    InputDevice& operator=(const InputDevice&) = delete;

    // 静态方法：枚举所有输入设备
    static std::vector<DeviceInfo> enumerateDevices();
    
    // 设备操作
    bool openDevice(const std::string& device_path);
    bool openDeviceByEventType(unsigned int event_type);
    void closeDevice();
    bool isOpen() const { return fd_ != -1; }
    
    // 事件监听
    void startListening();
    void stopListening();
    bool isListening() const { return listening_; }
    
    // 回调设置
    void setEventCallback(EventCallback callback) { event_callback_ = callback; }
    void setRawEventCallback(EventCallback callback) { raw_event_callback_ = callback; }
    
    // 设备信息获取
    DeviceInfo getDeviceInfo() const;
    std::string getDeviceName() const;
    std::vector<unsigned int> getSupportedEvents() const;
    
    // 特定事件处理（虚函数，可重写）
    virtual void onKeyEvent(const input_event& ev);
    virtual void onAbsEvent(const input_event& ev);
    virtual void onRelEvent(const input_event& ev);
    virtual void onSynEvent(const input_event& ev);

private:
    void listeningThread();
    bool readEvent(input_event& ev);
    void processEvent(const input_event& ev);

    int fd_ = -1;
    bool listening_ = false;
    std::string device_path_;
    EventCallback event_callback_;
    EventCallback raw_event_callback_;
};

#endif // INPUT_DEVICE_H