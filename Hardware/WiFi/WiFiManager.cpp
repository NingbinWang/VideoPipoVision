#include "WiFiManager.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

// 工具函数
namespace {
    std::string executeCommand(const std::string& cmd) {
        char buffer[128];
        std::string result = "";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) return "";
        
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
        return result;
    }

    bool isInterfaceUp(const std::string& interface) {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) return false;

        struct ifreq ifr;
        strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ-1);
        
        bool up = false;
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) >= 0) {
            up = (ifr.ifr_flags & IFF_UP) != 0;
        }
        
        close(sock);
        return up;
    }

    bool setInterfaceUp(const std::string& interface, bool up) {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) return false;

        struct ifreq ifr;
        strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ-1);
        
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
            close(sock);
            return false;
        }

        if (up) {
            ifr.ifr_flags |= IFF_UP;
        } else {
            ifr.ifr_flags &= ~IFF_UP;
        }

        bool success = ioctl(sock, SIOCSIFFLAGS, &ifr) >= 0;
        close(sock);
        return success;
    }
}

class DualModeWifiManager::Impl {
public:
    Impl() = default;
    ~Impl() { cleanup(); }

    bool initialize(const std::string& interface) {
        interface_ = interface;
        
        // 检查接口是否存在
        if (!isInterfaceUp(interface_)) {
            if (!setInterfaceUp(interface_, true)) {
                logError("无法启用网络接口: " + interface_);
                return false;
            }
        }

        // 停止可能存在的服务
        stopHostapd();
        stopWpaSupplicant();

        logInfo("WiFi管理器初始化成功");
        return true;
    }

    void cleanup() {
        apStop();
        staDisconnect();
        stopHostapd();
        stopWpaSupplicant();
    }

    bool setMode(WifiMode mode) {
        current_mode_ = mode;
        
        switch (mode) {
            case WifiMode::STATION:
                stopHostapd();
                startWpaSupplicant();
                break;
            case WifiMode::ACCESS_POINT:
                stopWpaSupplicant();
                break;
            case WifiMode::DUAL_MODE:
                // 双模式需要虚拟接口支持
                createVirtualInterface();
                break;
        }
        
        return true;
    }

    bool staConnect(const STAConfig& config) {
        if (current_mode_ == WifiMode::ACCESS_POINT) {
            logError("当前为AP模式，无法连接STA");
            return false;
        }

        sta_config_ = config;
        
        // 生成 wpa_supplicant 配置
        if (!generateWpaConfig()) {
            return false;
        }

        // 启动 wpa_supplicant
        if (!startWpaSupplicant()) {
            return false;
        }

        // 设置静态IP或DHCP
        if (!config.ip_address.empty()) {
            setStaticIP(interface_, config.ip_address, config.netmask, config.gateway);
        } else {
            setDHCP(interface_);
        }

        updateState(WifiState::CONNECTING, "正在连接网络: " + config.ssid);
        return true;
    }

    bool staDisconnect() {
        stopWpaSupplicant();
        updateState(WifiState::DISCONNECTED, "已断开连接");
        return true;
    }

    bool apStart(const APConfig& config) {
        ap_config_ = config;
        
        // 生成 hostapd 配置
        if (!generateHostapdConfig()) {
            return false;
        }

        // 设置AP模式IP
        setStaticIP(interface_, config.ip_address, config.netmask, "");

        // 启动 hostapd
        if (!startHostapd()) {
            return false;
        }

        // 启动 DHCP 服务器
        startDHCPServer();

        updateState(WifiState::AP_RUNNING, "AP模式已启动: " + config.ssid);
        return true;
    }

    bool apStop() {
        stopDHCPServer();
        stopHostapd();
        updateState(WifiState::DISCONNECTED, "AP模式已停止");
        return true;
    }

    bool enableDualMode(const STAConfig& sta_config, const APConfig& ap_config) {
        // 创建虚拟接口用于AP模式
        std::string ap_interface = interface_ + "_ap";
        if (!createVirtualInterface(ap_interface)) {
            logError("无法创建虚拟接口");
            return false;
        }

        // STA模式使用物理接口
        sta_config_ = sta_config;
        if (!staConnect(sta_config)) {
            return false;
        }

        // AP模式使用虚拟接口
        ap_config_ = ap_config;
        std::string original_interface = interface_;
        interface_ = ap_interface;
        
        bool success = apStart(ap_config);
        interface_ = original_interface;

        if (success) {
            current_mode_ = WifiMode::DUAL_MODE;
            updateState(WifiState::CONNECTED, "双模式已启用");
        }

        return success;
    }

private:
    // 配置生成
    bool generateWpaConfig() {
        std::ofstream file("/tmp/wpa_supplicant.conf");
        if (!file.is_open()) return false;

        file << "ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev\n";
        file << "update_config=1\n";
        file << "country=CN\n\n";
        
        file << "network={\n";
        file << "    ssid=\"" << sta_config_.ssid << "\"\n";
        if (!sta_config_.password.empty()) {
            file << "    psk=\"" << sta_config_.password << "\"\n";
        } else {
            file << "    key_mgmt=NONE\n";
        }
        file << "}\n";
        
        file.close();
        return true;
    }

    bool generateHostapdConfig() {
        std::ofstream file("/tmp/hostapd.conf");
        if (!file.is_open()) return false;

        file << "interface=" << interface_ << "\n";
        file << "driver=nl80211\n";
        file << "ssid=" << ap_config_.ssid << "\n";
        file << "hw_mode=g\n";
        file << "channel=" << ap_config_.channel << "\n";
        file << "wmm_enabled=1\n";
        file << "macaddr_acl=0\n";
        file << "ignore_broadcast_ssid=" << (ap_config_.hidden ? "1" : "0") << "\n";
        file << "max_num_sta=" << ap_config_.max_clients << "\n";
        
        if (!ap_config_.password.empty()) {
            file << "auth_algs=1\n";
            file << "wpa=2\n";
            file << "wpa_key_mgmt=WPA-PSK\n";
            file << "wpa_passphrase=" << ap_config_.password << "\n";
            file << "rsn_pairwise=CCMP\n";
        } else {
            file << "auth_algs=1\n";
        }
        
        file.close();
        return true;
    }

    // 服务管理
    bool startWpaSupplicant() {
        std::string cmd = "wpa_supplicant -B -i " + interface_ + 
                         " -c /tmp/wpa_supplicant.conf";
        return system(cmd.c_str()) == 0;
    }

    bool stopWpaSupplicant() {
        return system("pkill wpa_supplicant") == 0;
    }

    bool startHostapd() {
        std::string cmd = "hostapd -B /tmp/hostapd.conf";
        return system(cmd.c_str()) == 0;
    }

    bool stopHostapd() {
        return system("pkill hostapd") == 0;
    }

    bool startDHCPServer() {
        // 生成 dhcpd 配置
        std::ofstream file("/tmp/dhcpd.conf");
        file << "subnet " << ap_config_.ip_address << " netmask " << ap_config_.netmask << " {\n";
        file << "    range " << getNextIP(ap_config_.ip_address) << " " << getBroadcastIP(ap_config_.ip_address, ap_config_.netmask) << ";\n";
        file << "    option routers " << ap_config_.ip_address << ";\n";
        file << "    option domain-name-servers 8.8.8.8, 8.8.4.4;\n";
        file << "}\n";
        file.close();

        std::string cmd = "udhcpd -f /tmp/dhcpd.conf &";
        return system(cmd.c_str()) == 0;
    }

    bool stopDHCPServer() {
        return system("pkill udhcpd") == 0;
    }

    // 网络配置
    bool setStaticIP(const std::string& interface, const std::string& ip, 
                    const std::string& netmask, const std::string& gateway) {
        std::string cmd = "ifconfig " + interface + " " + ip + " netmask " + netmask;
        if (system(cmd.c_str()) != 0) return false;

        if (!gateway.empty()) {
            cmd = "route add default gw " + gateway + " " + interface;
            return system(cmd.c_str()) == 0;
        }
        return true;
    }

    bool setDHCP(const std::string& interface) {
        std::string cmd = "dhclient -r " + interface + " && dhclient " + interface;
        return system(cmd.c_str()) == 0;
    }

    bool createVirtualInterface(const std::string& vif_name = "") {
        std::string target_vif = vif_name.empty() ? interface_ + "_ap" : vif_name;
        std::string cmd = "iw dev " + interface_ + " interface add " + target_vif + " type __ap";
        return system(cmd.c_str()) == 0;
    }

    // 工具函数
    std::string getNextIP(const std::string& ip) {
        struct in_addr addr;
        inet_pton(AF_INET, ip.c_str(), &addr);
        addr.s_addr = htonl(ntohl(addr.s_addr) + 10); // IP + 10
        char next_ip[16];
        inet_ntop(AF_INET, &addr, next_ip, sizeof(next_ip));
        return next_ip;
    }

    std::string getBroadcastIP(const std::string& ip, const std::string& netmask) {
        struct in_addr ip_addr, mask_addr, broadcast;
        inet_pton(AF_INET, ip.c_str(), &ip_addr);
        inet_pton(AF_INET, netmask.c_str(), &mask_addr);
        broadcast.s_addr = ip_addr.s_addr | ~mask_addr.s_addr;
        
        char broadcast_ip[16];
        inet_ntop(AF_INET, &broadcast, broadcast_ip, sizeof(broadcast_ip));
        return broadcast_ip;
    }

    void updateState(WifiState state, const std::string& message) {
        current_state_ = state;
        if (state_callback_) {
            state_callback_(state, message);
        }
        logInfo("状态更新: " + message);
    }

    void logInfo(const std::string& message) {
        std::cout << "[INFO] " << message << std::endl;
    }

    void logError(const std::string& message) {
        std::cerr << "[ERROR] " << message << std::endl;
    }

    // 成员变量
    std::string interface_;
    WifiMode current_mode_ = WifiMode::STATION;
    WifiState current_state_ = WifiState::DISCONNECTED;
    STAConfig sta_config_;
    APConfig ap_config_;
    StateCallback state_callback_;
    ClientCallback client_callback_;
};

// DualModeWifiManager 实现
DualModeWifiManager::DualModeWifiManager() : pimpl_(std::make_unique<Impl>()) {}
DualModeWifiManager::~DualModeWifiManager() = default;

bool DualModeWifiManager::initialize(const std::string& interface) {
    return pimpl_->initialize(interface);
}

void DualModeWifiManager::cleanup() {
    pimpl_->cleanup();
}

bool DualModeWifiManager::setMode(WifiMode mode) {
    return pimpl_->setMode(mode);
}

bool DualModeWifiManager::staConnect(const STAConfig& config) {
    return pimpl_->staConnect(config);
}

bool DualModeWifiManager::staDisconnect() {
    return pimpl_->staDisconnect();
}

bool DualModeWifiManager::apStart(const APConfig& config) {
    return pimpl_->apStart(config);
}

bool DualModeWifiManager::apStop() {
    return pimpl_->apStop();
}

bool DualModeWifiManager::enableDualMode(const STAConfig& sta_config, const APConfig& ap_config) {
    return pimpl_->enableDualMode(sta_config, ap_config);
}

//WifiState DualModeWifiManager::getCurrentState() const {
//    return pimpl_->current_state_;
//}