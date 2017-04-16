#ifndef ESP8266TELEGRAMH
#define ESP8266TELEGRAMH

#define CONNECTION_TIMEOUT 5000

#define DEBUG(msg) if (_debug_serial != NULL) _debug_serial->print(msg);
#define DEBUGLN(msg) if (_debug_serial != NULL) _debug_serial->println(msg);

#include <Client.h>
#include <Arduino.h>
#include <ArduinoJson.h>

class TelegramBot {
  public:
    TelegramBot(const char* token, Client &client);
    TelegramBot(const String &token, Client &client);
    uint8_t begin(void);
    uint32_t send_message(const String& chat_id, const String& text);

    void set_debug_serial(Stream* stream);
  private:
    String _token;
    Client* _client;
    DynamicJsonBuffer _json_buffer;
    Stream* _debug_serial;

    const char* TELEGRAM_API_HOST = "api.telegram.org";

    JsonObject* _send(const String &endpoint, JsonObject& data);
};

#endif
