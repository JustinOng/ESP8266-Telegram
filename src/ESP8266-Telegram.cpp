#include <ESP8266-Telegram.h>

TelegramBot::TelegramBot(const char* token, Client &client) {
  TelegramBot(String(token), client);
}

TelegramBot::TelegramBot(const String &token, Client &client) {
  _token = token;
  _client = &client;

  _json_buffer = DynamicJsonBuffer(1024);
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
  JsonObject& data = _json_buffer.createObject();
  data["chat_id"] = chat_id;
  data["text"] = text;

  JsonObject* response = _send("/sendMessage", data);

  if (response == NULL) return 0;

  JsonObject& json = *response;

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

JsonObject* TelegramBot::_send(const String &endpoint, JsonObject& data) {
  String payload;
  data.printTo(payload);

  if (!_client->connect(TELEGRAM_API_HOST, 443)) {
    DEBUGLN("Failed to connect to TELEGRAM_API_HOST");
    return NULL;
  }

  DEBUG("Payload: ");
  DEBUGLN(payload);

  DEBUGLN("Sending request...");
  _client->println("POST /bot"+_token+endpoint+" HTTP/1.1");
	_client->println("Host: api.telegram.org");
  _client->println("Content-Type: application/json");
  //_client->println("Connection: close");
  _client->print("Content-Length: "); _client->println(payload.length());

  // blank line to indicate end of headers
  _client->println();

  _client->println(payload);

  _client->println();

  DEBUGLN("Waiting for response...");

  /*uint32_t timeout = millis();

  while(_client->connected() && _client->available() == 0) {
    if ((millis() - timeout) > CONNECTION_TIMEOUT) {
      DEBUGLN("Client timeout!");
      _client->stop();
      return NULL;
    }
    DEBUGLN(millis() - timeout);
  }*/

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
    return NULL;
  }

  char json_string[content_length];

  _client->readBytes(json_string, content_length);

  _client->stop();

  DEBUGLN(json_string);

  DEBUGLN("Parsing response...");

  JsonObject& response = _json_buffer.parseObject(json_string);

  if (!response.success()) {
    DEBUGLN("Failed to parse response!");
  }

  return &response;
}
