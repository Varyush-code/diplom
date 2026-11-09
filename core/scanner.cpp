#include "scanner.h"

std::string getLocalIP() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) return "";

    struct sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &dest.sin_addr);

    if (connect(sock, (struct sockaddr*)&dest, sizeof(dest)) == -1) {
        close(sock);
        return "";
    }

    struct sockaddr_in local{};
    socklen_t len = sizeof(local);
    if (getsockname(sock, (struct sockaddr*)&local, &len) == -1) {
        close(sock);
        return "";
    }
    close(sock);

    char buf[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &local.sin_addr, buf, sizeof(buf))) {
        ip = std::string(buf);
        return ip;
    }

    return "";
}

std::string findIfaceAndNetmask(std::string& ip) {
    struct ifaddrs *interfaces = nullptr;
    if (getifaddrs(&interfaces) == -1) return "";

    bool found = false;
    for (struct ifaddrs *ifa = interfaces; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr || !ifa->ifa_netmask) continue;
        if (ifa->ifa_addr->sa_family != AF_INET) continue;

        char ipBuf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr,
                  ipBuf, sizeof(ipBuf));

        if (ip != ipBuf) continue;   // ищем именно наш IP

        char maskBuf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &((struct sockaddr_in*)ifa->ifa_netmask)->sin_addr,
                  maskBuf, sizeof(maskBuf));

        //ifaceOut = ifa->ifa_name;
        netmask  = maskBuf;
        found = true;
        break;
    }

    freeifaddrs(interfaces);
    return netmask;
}

std::string readDefaultGateway(const std::string& iface) {
    std::ifstream f("/proc/net/route");
    if (!f.is_open()) return "";

    std::string line;
    std::getline(f, line); // заголовок

    while (std::getline(f, line)) {
        std::istringstream iss(line);
        std::string ifname, destHex, gwHex, flagsHex;

        if (!(iss >> ifname >> destHex >> gwHex >> flagsHex))
            continue;

        // Destination == 00000000 => маршрут по умолчанию
        if (destHex != "00000000") continue;

        // Если задан конкретный интерфейс — фильтруем
        if (!iface.empty() && ifname != iface) continue;

        // gwHex — hex в сетевом порядке байт, s_addr уже сетевой порядок
        uint32_t gw = std::stoul(gwHex, nullptr, 16);
        struct in_addr addr;
        addr.s_addr = gw;            // не делаем htonl!

        char buf[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &addr, buf, sizeof(buf))) {
            gateway = buf;
            return gateway;
        }
    }
    return "";
}
