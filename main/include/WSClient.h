#ifndef WS_CLIENT_H
#define WS_CLIENT_H

#include <ArduinoWebsockets.h>
#include <WiFi.h>

class WSClient {
public:
    WSClient(const char* host, uint16_t port);
    void begin(const char* ssid, const char* password);
    void checkConnection();
    void poll();
    void send(String message);

private:
    const char* websockets_server_host;
    uint16_t websockets_server_port;
    websockets::WebsocketsClient client;
};

#endif
