#pragma once

#include <string>
#include <cstring>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cstdint>

extern std::string ip;
extern std::string netmask;
extern std::string gateway;

std::string getLocalIP();
std::string findIfaceAndNetmask(std::string& ip);
std::string readDefaultGateway(const std::string& iface);
