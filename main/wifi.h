#ifndef WIFI_
#define WIFI_

#define TAG "wifi"
// TODO: DELETE
// #define SSID "WIELKI MASZT 5G"
// #define PASS "StudentPiwo123"

#define SSID "LSO-01"
#define PASS "Lesio12345"

extern char IsWifiConnected;
extern uint8_t mac[6];
extern char deviceId[13];

void initialize_wifi();

#endif