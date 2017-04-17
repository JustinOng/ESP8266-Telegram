#include <ESP8266-Telegram.h>

TelegramBot::TelegramBot(const char* token, Client &client) {
  TelegramBot(String(token), client);
}

TelegramBot::TelegramBot(const String &token, Client &client) {
  _token = token;
  _client = &client;
}

void TelegramBot::set_debug_serial(Stream* stream) {
  _debug_serial = stream;
}

uint8_t TelegramBot::begin(void) {
  _client->setTimeout(500);
  DEBUGLN("Connecting to TELEGRAM_API_HOST");

  if (!_client->connect(TELEGRAM_API_HOST, 443)) {
    DEBUGLN("Failed to connect to TELEGRAM_API_HOST");
    return 0;
  }

  return 1;
}

uint32_t TelegramBot::send_message(const String& chat_id, const String& text) {
  DynamicJsonBuffer json_buffer(1024);
  JsonObject& data = json_buffer.createObject();
  data["chat_id"] = chat_id;
  data["text"] = text;

  String json_string;

  data.printTo(json_string);

  String response;
  uint16_t response_length = _send("/sendMessage", json_string, response);

  if (response_length == 0) return 0;

  DEBUGLN(response);

  JsonObject& json = json_buffer.parseObject(response);

  bool ok = json["ok"];

  if (!ok) {
    DEBUG("TG Error: ");
    String description = json["result"]["description"];
    DEBUGLN(description);
    return 0;
  }

  uint32_t message_id = json["result"]["message_id"];

  return message_id;
}

uint8_t TelegramBot::edit_message(const String& chat_id, uint32_t message_id, const String& text) {
  DynamicJsonBuffer json_buffer(1024);
  JsonObject& data = json_buffer.createObject();
  data["chat_id"] = chat_id;
  data["message_id"] = message_id;
  data["text"] = text;

  String json_string;

  data.printTo(json_string);

  String response;
  uint16_t response_length = _send("/editMessageText", json_string, response);

  if (response_length == 0) return 0;
  DEBUGLN(response);

  JsonObject& json = json_buffer.parseObject(response);

  bool ok = json["ok"];

  if (!ok) {
    DEBUG("TG Error: ");
    String description = json["result"]["description"];
    DEBUGLN(description);
    return 0;
  }

  return 1;
}

uint16_t TelegramBot::_send(const String &endpoint, const String &payload, String &response) {
  if (!_client->connect(TELEGRAM_API_HOST, 443)) {
    DEBUGLN("Failed to connect to TELEGRAM_API_HOST");
    return 0;
  }

  //DEBUG("Payload: ");
  //DEBUGLN(payload);

  DEBUG("Sending request to ");
  DEBUGLN(endpoint);

  uint32_t start_time = millis();
  // writing multiple times with header Connection: close does not give a response back?
  // https://github.com/esp8266/Arduino/issues/43#issuecomment-217454125
  String tx_data = "POST /bot"+_token+endpoint+" HTTP/1.1\r\n" +
  	"Host: api.telegram.org\r\n" +
    "Content-Type: application/json\r\n" +
    "Connection: close\r\n" +
    "Content-Length: " + String(payload.length()) + "\r\n\r\n" +
    payload + "\r\n";

  _client->print(tx_data);

  DEBUG("Took ");
  DEBUG(millis() - start_time);
  DEBUGLN("ms");

  DEBUGLN("Waiting for response...");

  while(_client->available() == 0) {
    delay(100);
  };

  _client->find("Content-Length: ");

  uint16_t content_length = _client->parseInt();

  // read until find {
  while(_client->available()) {
    if (_client->peek() == '{') {
      break;
    }
    else {
      _client->read();
    }
  }

  uint32_t timeout = millis();
  while (_client->available() < content_length) {
    if (millis() - timeout > 1000) break;
  }

  if (_client->available() < content_length) {
    DEBUG("Timed out, expected ");
    DEBUG(content_length);
    DEBUG(" bytes but only got ");
    DEBUGLN(_client->available());
    return 0;
  }

  char json_string[content_length];

  _client->readBytes(json_string, content_length);

  //DEBUGLN(json_string);

  DEBUGLN("Parsing response...");

  response = json_string;
  return content_length;
}
