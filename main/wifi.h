#ifndef WIFI_
#define WIFI_

#define TAG "wifi"
// TODO: DELETE
// #define SSID "WIELKI MASZT 5G"
// #define PASS "StudentPiwo123"

#define SSID "NETIASPOT-BcgF4Zc-2.4G"
#define PASS "bVF7hRUnhD42x8hEZY"

extern char IsWifiConnected;
extern uint8_t mac[6];
extern char deviceId[13];

void initialize_wifi();

#endif