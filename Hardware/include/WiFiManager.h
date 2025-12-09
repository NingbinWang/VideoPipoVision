#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

// 工作模式
enum class WifiMode {
    STATION,        // 客户端模式
    ACCESS_POINT,   // 热点模式
    DUAL_MODE       // 同时支持 STA 和 AP
};

// WiFi 状态
enum class WifiState {
    DISCONNECTED,
    SCANNING,
    CONNECTING,
    CONNECTED,
    AP_STARTING,
    AP_RUNNING,
    ERROR
};

// AP 配置
struct APConfig {
    std::string ssid;
    std::string password;
    std::string ip_address = "192.168.1.21";
    std::string netmask = "255.255.255.0";
    int channel = 6;
    bool hidden = false;
    int max_clients = 10;
};

// STA 配置
struct STAConfig {
    std::string ssid;
    std::string password;
    std::string ip_address;  // 为空则使用 DHCP
    std::string netmask;
    std::string gateway;
};

// 网络信息
struct NetworkInfo {
    std::string ssid;
    std::string bssid;
    int signal_level;
    int frequency;
    bool encrypted;
};

// 客户端信息 (AP 模式)
struct ClientInfo {
    std::string mac_address;
    std::string ip_address;
    std::string hostname;
};

// 回调函数
using StateCallback = std::function<void(WifiState, const std::string&)>;
using ScanCallback = std::function<void(const std::vector<NetworkInfo>&)>;
using ClientCallback = std::function<void(const ClientInfo&, bool connected)>;

class DualModeWifiManager {
public:
    DualModeWifiManager();
    virtual ~DualModeWifiManager();

    // 禁止拷贝
    DualModeWifiManager(const DualModeWifiManager&) = delete;
    DualModeWifiManager& operator=(const DualModeWifiManager&) = delete;

    // 初始化
    bool initialize(const std::string& interface = "wlan0");
    void cleanup();

    // 模式设置
    bool setMode(WifiMode mode);
    WifiMode getCurrentMode() const;

    // STA 模式操作
    bool staConnect(const STAConfig& config);
    bool staDisconnect();
    bool staScan(ScanCallback callback = nullptr);
    std::vector<NetworkInfo> staGetScanResults();
    STAConfig staGetCurrentConfig() const;

    // AP 模式操作
    bool apStart(const APConfig& config);
    bool apStop();
    bool apIsRunning() const;
    std::vector<ClientInfo> apGetClients() const;
    APConfig apGetCurrentConfig() const;

    // 双模式操作
    bool enableDualMode(const STAConfig& sta_config, const APConfig& ap_config);

    // 状态信息
    WifiState getCurrentState() const;
    std::string getIPAddress() const;
    std::string getMACAddress() const;

    // 回调设置
    void setStateCallback(StateCallback callback);
    void setClientCallback(ClientCallback callback);

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

#endif // WIFI_MANAGER_H