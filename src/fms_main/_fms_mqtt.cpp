#include "fms_mqtt.h"
#include <stdarg.h>
/*
* fms mqtt client 
* maintained by : Trion (nck)
* copyright :@2025 iih
* guided by :Sir Thiha Kyaw (Senior Engineer)
*/

FmsMqtt::FmsMqtt() {
  _connected = false;
  _lastPingTime = 0;
  _lastIncomingTime = 0;
  _keepAliveInterval = MQTT_DEFAULT_KEEPALIVE;
  _socketTimeout = MQTT_SOCKET_TIMEOUT;
  _maxPacketSize = MQTT_MAX_PACKET_SIZE;
  _callback = NULL;
  _lastError = MQTT_ERROR_NONE;
  _nextPacketId = 1;
  _cleanSession = true;
  _protocolVersion = MQTT_VERSION_3_1_1;
  _debug = false;
}

bool FmsMqtt::connect(const char* host, uint16_t port, const char* clientId) {
  return connect(host, port, clientId, NULL, NULL);
}

bool FmsMqtt::connect(const char* host, uint16_t port, const char* clientId,
                      const char* username, const char* password) {
  return connect(host, port, clientId, username, password, NULL, 0, false, NULL);
}

bool FmsMqtt::connect(const char* host, uint16_t port, const char* clientId,
                      const char* username, const char* password,
                      const char* willTopic, uint8_t willQos, bool willRetain,
                      const char* willMessage) {
  _host = String(host);
  _port = port;
  _clientId = String(clientId);

  if (username) _username = String(username);
  if (password) _password = String(password);
  if (willTopic) _willTopic = String(willTopic);
  if (willMessage) _willMessage = String(willMessage);

  _willQos = willQos;
  _willRetain = willRetain;

  // Connect to MQTT broker
  _debugPrintln("Connecting to MQTT broker");
  if (!_client.connect(host, port)) {
    _lastError = MQTT_ERROR_NETWORK_CONNECTION;
    _debugPrintln("Network connection failed");
    return false;
  }

  // Set socket timeout
  _client.setTimeout(_socketTimeout * 1000);

  // Build and send CONNECT packet
  uint16_t len = _buildConnect();
  uint8_t* buffer = new uint8_t[len];

  // Construct fixed header
  buffer[0] = MQTT_CONNECT << 4;

  // Variable length encoding of remaining length
  uint16_t remainingLength = len - 2;  // Subtract fixed header (2 bytes)
  buffer[1] = remainingLength;

  // Protocol name length (MSB, LSB)
  buffer[2] = 0;
  buffer[3] = 4;

  // Protocol name "MQTT"
  buffer[4] = 'M';
  buffer[5] = 'Q';
  buffer[6] = 'T';
  buffer[7] = 'T';

  // Protocol level
  buffer[8] = _protocolVersion;

  // Connect flags
  uint8_t connectFlags = _cleanSession ? MQTT_CLEAN_SESSION : 0;
  if (!_username.isEmpty()) connectFlags |= MQTT_USERNAME_FLAG;
  if (!_password.isEmpty()) connectFlags |= MQTT_PASSWORD_FLAG;
  if (!_willTopic.isEmpty()) {
    connectFlags |= MQTT_WILL_FLAG;
    connectFlags |= (_willQos & 0x03) << 3;
    if (_willRetain) connectFlags |= MQTT_WILL_RETAIN;
  }
  buffer[9] = connectFlags;

  // Keep alive (MSB, LSB)
  buffer[10] = _keepAliveInterval >> 8;
  buffer[11] = _keepAliveInterval & 0xFF;

  // Client ID length (MSB, LSB)
  uint16_t clientIdLength = _clientId.length();
  buffer[12] = clientIdLength >> 8;
  buffer[13] = clientIdLength & 0xFF;

  // Client ID
  uint16_t index = 14;
  for (uint16_t i = 0; i < clientIdLength; i++) {
    buffer[index++] = _clientId.charAt(i);
  }

  // Will topic and message if present
  if (!_willTopic.isEmpty()) {
    // Will topic length
    uint16_t willTopicLength = _willTopic.length();
    buffer[index++] = willTopicLength >> 8;
    buffer[index++] = willTopicLength & 0xFF;

    // Will topic
    for (uint16_t i = 0; i < willTopicLength; i++) {
      buffer[index++] = _willTopic.charAt(i);
    }

    // Will message length
    uint16_t willMessageLength = _willMessage.length();
    buffer[index++] = willMessageLength >> 8;
    buffer[index++] = willMessageLength & 0xFF;

    // Will message
    for (uint16_t i = 0; i < willMessageLength; i++) {
      buffer[index++] = _willMessage.charAt(i);
    }
  }

  // Username if present
  if (!_username.isEmpty()) {
    // Username length
    uint16_t usernameLength = _username.length();
    buffer[index++] = usernameLength >> 8;
    buffer[index++] = usernameLength & 0xFF;

    // Username
    for (uint16_t i = 0; i < usernameLength; i++) {
      buffer[index++] = _username.charAt(i);
    }
  }

  // Password if present
  if (!_password.isEmpty()) {
    // Password length
    uint16_t passwordLength = _password.length();
    buffer[index++] = passwordLength >> 8;
    buffer[index++] = passwordLength & 0xFF;

    // Password
    for (uint16_t i = 0; i < passwordLength; i++) {
      buffer[index++] = _password.charAt(i);
    }
  }

  // Send CONNECT packet
  _debugPrintln("Sending CONNECT packet");
  if (!_sendPacket(buffer, len)) {
    delete[] buffer;
    _client.stop();
    _lastError = MQTT_ERROR_TIMEOUT;
    _debugPrintln("Failed to send CONNECT packet");
    return false;
  }

  delete[] buffer;

  // Wait for CONNACK
  _debugPrintln("Waiting for CONNACK");
  unsigned long timeout = millis() + (_socketTimeout * 1000);
  while (!_client.available() && millis() < timeout) {
    delay(10);
  }

  if (!_client.available()) {
    _client.stop();
    _lastError = MQTT_ERROR_TIMEOUT;
    _debugPrintln("CONNACK timeout");
    return false;
  }

  // Read CONNACK packet
  uint8_t packetType = _client.read();
  uint8_t remainingLengthByte = _client.read();

  if ((packetType >> 4) != MQTT_CONNACK || remainingLengthByte != 2) {
    _client.stop();
    _lastError = MQTT_ERROR_MALFORMED_PACKET;
    _debugPrintln("Invalid CONNACK packet");
    return false;
  }

  // Reserved byte and return code
  uint8_t reserved = _client.read();
  uint8_t returnCode = _client.read();

  _debugPrintf("CONNACK received: %d", returnCode);

  if (returnCode != 0) {
    _client.stop();
    _lastError = returnCode;
    _debugPrintf("Connection refused: %d", returnCode);
    return false;
  }

  _connected = true;
  _lastPingTime = millis();
  _lastIncomingTime = millis();
  _lastError = MQTT_ERROR_NONE;

  // If clean session, clear any existing subscriptions
  if (_cleanSession) {
    _subscriptions.clear();
    _qosMessages.clear();
  } else {
    // Resubscribe to topics if not clean session
    for (const auto& sub : _subscriptions) {
      subscribe(sub.topic.c_str(), sub.qos);
    }
  }

  _debugPrintln("Connected successfully");
  return true;
}

void FmsMqtt::disconnect() {
  if (_connected) {
    // Build and send DISCONNECT packet
    _debugPrintln("Sending DISCONNECT packet");
    uint8_t buffer[2];
    buffer[0] = MQTT_DISCONNECT << 4;
    buffer[1] = 0;  // Remaining length

    _sendPacket(buffer, 2);
    _client.stop();
    _connected = false;

    // If clean session, clear subscriptions
    if (_cleanSession) {
      _subscriptions.clear();
      _qosMessages.clear();
    }
  }
}

bool FmsMqtt::publish(const char* topic, const char* payload, bool retain) {
  return publish(topic, payload, strlen(payload), 0, retain);
}

bool FmsMqtt::publish(const char* topic, const char* payload, uint16_t length, bool retain) {
  return publish(topic, payload, length, 0, retain);
}

bool FmsMqtt::publish(const char* topic, const char* payload, uint16_t length, uint8_t qos, bool retain) {
  if (!_connected) {
    _lastError = MQTT_ERROR_NETWORK_CONNECTION;
    return false;
  }

  _debugPrintf("Publishing to %s (QoS %d, Retain %d)", topic, qos, retain);

  uint16_t packetId = 0;
  if (qos > 0) {
    // Generate packet ID for QoS > 0
    packetId = getNextPacketId();
  }

  // Calculate total length
  uint16_t topicLength = strlen(topic);
  uint16_t remainingLength = 2 + topicLength + length;  // Topic length bytes + topic + payload

  if (qos > 0) {
    remainingLength += 2;  // Add packet ID for QoS > 0
  }

  // Variable length encoding of remaining length
  uint8_t remainingLengthBytes[4];
  uint8_t remainingLengthBytesCount = 0;

  do {
    uint8_t encodedByte = remainingLength % 128;
    remainingLength = remainingLength / 128;

    if (remainingLength > 0) {
      encodedByte |= 0x80;
    }

    remainingLengthBytes[remainingLengthBytesCount++] = encodedByte;
  } while (remainingLength > 0 && remainingLengthBytesCount < 4);

  // Total packet length
  uint16_t totalLength = 1 + remainingLengthBytesCount + 2 + topicLength + (qos > 0 ? 2 : 0) + length;

  uint8_t* buffer = new uint8_t[totalLength];
  uint16_t index = 0;

  // Fixed header
  buffer[index++] = (MQTT_PUBLISH << 4) | (qos << 1) | (retain ? 1 : 0);

  // Remaining length
  for (uint8_t i = 0; i < remainingLengthBytesCount; i++) {
    buffer[index++] = remainingLengthBytes[i];
  }

  // Topic length (MSB, LSB)
  buffer[index++] = topicLength >> 8;
  buffer[index++] = topicLength & 0xFF;

  // Topic
  for (uint16_t i = 0; i < topicLength; i++) {
    buffer[index++] = topic[i];
  }

  // Packet ID for QoS > 0
  if (qos > 0) {
    buffer[index++] = packetId >> 8;
    buffer[index++] = packetId & 0xFF;
  }

  // Payload
  for (uint16_t i = 0; i < length; i++) {
    buffer[index++] = payload[i];
  }

  // Send packet
  bool result = _sendPacket(buffer, totalLength);
  delete[] buffer;

  if (!result) {
    _lastError = MQTT_ERROR_TIMEOUT;
    _debugPrintln("Failed to send PUBLISH packet");
    return false;
  }

  // For QoS1 and QoS2, store message for handling acknowledgments
  if (qos > 0) {
    QoSMessage qosMsg;
    qosMsg.packetId = packetId;
    qosMsg.topic = String(topic);
    qosMsg.payload = String(payload, length);
    qosMsg.qos = qos;
    qosMsg.retain = retain;
    qosMsg.timestamp = millis();
    qosMsg.retries = 0;

    if (qos == 1) {
      qosMsg.state = MQTT_PUBACK;  // Waiting for PUBACK
    } else if (qos == 2) {
      qosMsg.state = MQTT_PUBREC;  // Waiting for PUBREC
    }

    _qosMessages.push_back(qosMsg);
  }

  return true;
}

bool FmsMqtt::subscribe(const char* topic) {
  return subscribe(topic, 0);
}

bool FmsMqtt::subscribe(const char* topic, uint8_t qos) {
  if (!_connected) {
    _lastError = MQTT_ERROR_NETWORK_CONNECTION;
    return false;
  }

  _debugPrintf("Subscribing to %s (QoS %d)", topic, qos);

  // Generate packet ID
  uint16_t packetId = getNextPacketId();

  // Calculate total length
  uint16_t topicLength = strlen(topic);
  uint16_t remainingLength = 2 + 2 + topicLength + 1;  // Packet ID (2) + topic length (2) + topic + QoS (1)

  // Variable length encoding of remaining length
  uint8_t remainingLengthBytes[4];
  uint8_t remainingLengthBytesCount = 0;

  do {
    uint8_t encodedByte = remainingLength % 128;
    remainingLength = remainingLength / 128;

    if (remainingLength > 0) {
      encodedByte |= 0x80;
    }

    remainingLengthBytes[remainingLengthBytesCount++] = encodedByte;
  } while (remainingLength > 0 && remainingLengthBytesCount < 4);

  // Total packet length
  uint16_t totalLength = 1 + remainingLengthBytesCount + 2 + 2 + topicLength + 1;

  uint8_t* buffer = new uint8_t[totalLength];
  uint16_t index = 0;

  // Fixed header
  buffer[index++] = MQTT_SUBSCRIBE << 4 | 0x02;  // SUBSCRIBE with required flags

  // Remaining length
  for (uint8_t i = 0; i < remainingLengthBytesCount; i++) {
    buffer[index++] = remainingLengthBytes[i];
  }

  // Packet ID (MSB, LSB)
  buffer[index++] = packetId >> 8;
  buffer[index++] = packetId & 0xFF;

  // Topic length (MSB, LSB)
  buffer[index++] = topicLength >> 8;
  buffer[index++] = topicLength & 0xFF;

  // Topic
  for (uint16_t i = 0; i < topicLength; i++) {
    buffer[index++] = topic[i];
  }

  // QoS
  buffer[index++] = qos & 0x03;

  // Send packet
  bool result = _sendPacket(buffer, totalLength);
  delete[] buffer;

  if (!result) {
    _lastError = MQTT_ERROR_TIMEOUT;
    _debugPrintln("Failed to send SUBSCRIBE packet");
    return false;
  }

  // Wait for SUBACK
  unsigned long timeout = millis() + (_socketTimeout * 1000);
  while (!_client.available() && millis() < timeout) {
    delay(10);
  }

  if (!_client.available()) {
    _lastError = MQTT_ERROR_TIMEOUT;
    _debugPrintln("SUBACK timeout");
    return false;
  }

  // Process incoming packets until we get SUBACK
  while (_client.available()) {
    if (_receivePacket()) {
      // Check if we got SUBACK with our packet ID
      // For simplicity, we'll just assume success if we received any packet

      // Add to subscriptions list if not already present
      bool found = false;
      for (auto& sub : _subscriptions) {
        if (sub.topic == String(topic)) {
          sub.qos = qos;  // Update QoS if already subscribed
          found = true;
          break;
        }
      }

      if (!found) {
        Subscription sub;
        sub.topic = String(topic);
        sub.qos = qos;
        _subscriptions.push_back(sub);
      }

      return true;
    }
  }

  _lastError = MQTT_ERROR_TIMEOUT;
  _debugPrintln("Failed to receive SUBACK");
  return false;
}

bool FmsMqtt::unsubscribe(const char* topic) {
  if (!_connected) {
    _lastError = MQTT_ERROR_NETWORK_CONNECTION;
    return false;
  }

  _debugPrintf("Unsubscribing from %s", topic);

  // Generate packet ID
  uint16_t packetId = getNextPacketId();

  // Calculate total length
  uint16_t topicLength = strlen(topic);
  uint16_t remainingLength = 2 + 2 + topicLength;  // Packet ID (2) + topic length (2) + topic

  // Variable length encoding of remaining length
  uint8_t remainingLengthBytes[4];
  uint8_t remainingLengthBytesCount = 0;

  do {
    uint8_t encodedByte = remainingLength % 128;
    remainingLength = remainingLength / 128;

    if (remainingLength > 0) {
      encodedByte |= 0x80;
    }

    remainingLengthBytes[remainingLengthBytesCount++] = encodedByte;
  } while (remainingLength > 0 && remainingLengthBytesCount < 4);

  // Total packet length
  uint16_t totalLength = 1 + remainingLengthBytesCount + 2 + 2 + topicLength;

  uint8_t* buffer = new uint8_t[totalLength];
  uint16_t index = 0;

  // Fixed header
  buffer[index++] = MQTT_UNSUBSCRIBE << 4 | 0x02;  // UNSUBSCRIBE with required flags

  // Remaining length
  for (uint8_t i = 0; i < remainingLengthBytesCount; i++) {
    buffer[index++] = remainingLengthBytes[i];
  }

  // Packet ID (MSB, LSB)
  buffer[index++] = packetId >> 8;
  buffer[index++] = packetId & 0xFF;

  // Topic length (MSB, LSB)
  buffer[index++] = topicLength >> 8;
  buffer[index++] = topicLength & 0xFF;

  // Topic
  for (uint16_t i = 0; i < topicLength; i++) {
    buffer[index++] = topic[i];
  }

  // Send packet
  bool result = _sendPacket(buffer, totalLength);
  delete[] buffer;

  if (!result) {
    _lastError = MQTT_ERROR_TIMEOUT;
    _debugPrintln("Failed to send UNSUBSCRIBE packet");
    return false;
  }

  // Wait for UNSUBACK
  unsigned long timeout = millis() + (_socketTimeout * 1000);
  while (!_client.available() && millis() < timeout) {
    delay(10);
  }

  if (!_client.available()) {
    _lastError = MQTT_ERROR_TIMEOUT;
    _debugPrintln("UNSUBACK timeout");
    return false;
  }

  // Process incoming packets until we get UNSUBACK
  while (_client.available()) {
    if (_receivePacket()) {
      // Remove from subscriptions list
      for (auto it = _subscriptions.begin(); it != _subscriptions.end(); ++it) {
        if (it->topic == String(topic)) {
          _subscriptions.erase(it);
          break;
        }
      }

      return true;
    }
  }

  _lastError = MQTT_ERROR_TIMEOUT;
  _debugPrintln("Failed to receive UNSUBACK");
  return false;
}

void FmsMqtt::loop() {
  if (!_connected) return;

  // Check for incoming packets
  if (_client.available()) {
    _receivePacket();
    _lastIncomingTime = millis();
  }

  // Check QoS message timeouts and retransmit if needed
  _checkQoSTimeout();

  // Send keep alive ping if needed
  unsigned long currentTime = millis();
  if (currentTime - _lastIncomingTime > (_keepAliveInterval * 1000) && currentTime - _lastPingTime > (_keepAliveInterval * 1000) / 2) {
    _lastPingTime = currentTime;

    // Build and send PINGREQ packet
    _debugPrintln("Sending PINGREQ");
    uint8_t buffer[2];
    buffer[0] = MQTT_PINGREQ << 4;
    buffer[1] = 0;  // Remaining length

    if (!_sendPacket(buffer, 2)) {
      _connected = false;
      _lastError = MQTT_ERROR_TIMEOUT;
      _debugPrintln("Failed to send PINGREQ, connection lost");
    }
  }

  // Check if connection timed out (no response for 1.5 * keepalive)
  if (currentTime - _lastIncomingTime > (_keepAliveInterval * 1500)) {
    _connected = false;
    _lastError = MQTT_ERROR_TIMEOUT;
    _debugPrintln("Connection timeout, no response from broker");
  }
}

bool FmsMqtt::available() {
  return !_lastTopic.isEmpty() && !_lastPayload.isEmpty();
}

String FmsMqtt::getTopic() {
  String topic = _lastTopic;
  _lastTopic = "";
  return topic;
}

String FmsMqtt::getPayload() {
  String payload = _lastPayload;
  _lastPayload = "";
  _lastPayloadLength = 0;
  return payload;
}

uint16_t FmsMqtt::getPayloadLength() {
  return _lastPayloadLength;
}

void FmsMqtt::setCallback(MqttCallback callback) {
  _callback = callback;
}

bool FmsMqtt::connected() {
  return _connected && _client.connected();
}

int FmsMqtt::getLastError() {
  return _lastError;
}

String FmsMqtt::getErrorString() {
  switch (_lastError) {
    case MQTT_ERROR_NONE:
      return "No error";
    case MQTT_ERROR_NETWORK_CONNECTION:
      return "Network connection error";
    case MQTT_ERROR_PROTOCOL_VERSION:
      return "Protocol version not supported";
    case MQTT_ERROR_INVALID_ID:
      return "Client ID rejected";
    case MQTT_ERROR_SERVER_UNAVAILABLE:
      return "Server unavailable";
    case MQTT_ERROR_BAD_CREDENTIALS:
      return "Bad username or password";
    case MQTT_ERROR_NOT_AUTHORIZED:
      return "Not authorized";
    case MQTT_ERROR_MALFORMED_PACKET:
      return "Malformed packet";
    case MQTT_ERROR_TIMEOUT:
      return "Timeout";
    case MQTT_ERROR_QOS_FAILURE:
      return "QoS failure";
    default:
      return "Unknown error";
  }
}

void FmsMqtt::setKeepAlive(uint16_t keepAliveInterval) {
  _keepAliveInterval = keepAliveInterval;
}

void FmsMqtt::setSocketTimeout(uint16_t timeout) {
  _socketTimeout = timeout;
}

void FmsMqtt::setMaxPacketSize(uint16_t size) {
  _maxPacketSize = size;
}

void FmsMqtt::setCleanSession(bool cleanSession) {
  _cleanSession = cleanSession;
}

void FmsMqtt::setProtocolVersion(uint8_t version) {
  _protocolVersion = version;
}

void FmsMqtt::enableDebug(bool enable) {
  _debug = enable;
}

uint16_t FmsMqtt::getNextPacketId() {
  uint16_t id = _nextPacketId;
  _nextPacketId++;
  if (_nextPacketId == 0) _nextPacketId = 1;
  return id;
}

bool FmsMqtt::_sendPacket(uint8_t* buffer, uint16_t length) {
  if (!_client.connected()) {
    _connected = false;
    return false;
  }

  return _client.write(buffer, length) == length;
}

bool FmsMqtt::_receivePacket() {
  if (!_client.available()) return false;

  // Read fixed header
  uint8_t packetType = _client.read();
  uint8_t packetTypeCode = packetType >> 4;

  // Read remaining length
  uint16_t remainingLength = _readPacketLength();

  // Allocate buffer for payload
  uint8_t* payload = new uint8_t[remainingLength];
  uint16_t index = 0;

  // Read payload
  unsigned long timeout = millis() + (_socketTimeout * 1000);
  while (index < remainingLength && millis() < timeout) {
    if (_client.available()) {
      payload[index++] = _client.read();
    } else {
      delay(10);
    }
  }

  if (index < remainingLength) {
    delete[] payload;
    _lastError = MQTT_ERROR_TIMEOUT;
    _debugPrintln("Timeout reading packet payload");
    return false;
  }

  // Process packet based on type
  _processPacket(packetTypeCode, payload, remainingLength);

  delete[] payload;
  return true;
}

uint16_t FmsMqtt::_readPacketLength() {
  uint32_t multiplier = 1;
  uint32_t value = 0;
  uint8_t digit;
  uint8_t encodedByte;

  do {
    encodedByte = _client.read();
    digit = encodedByte & 0x7F;
    value += digit * multiplier;
    multiplier *= 128;
  } while ((encodedByte & 0x80) != 0);

  return (uint16_t)value;
}

void FmsMqtt::_processPacket(uint8_t packetType, uint8_t* payload, uint16_t length) {
  _debugPrintf("Processing packet type: %d", packetType);

  switch (packetType) {
    case MQTT_CONNACK:
      // Already handled in connect()
      break;

    case MQTT_PUBLISH:
      {
        if (length > 0) {
          // Extract topic length
          uint16_t topicLength = (payload[0] << 8) | payload[1];

          // Extract topic
          char* topic = new char[topicLength + 1];
          for (uint16_t i = 0; i < topicLength; i++) {
            topic[i] = payload[i + 2];
          }
          topic[topicLength] = '\0';

          // Determine payload start based on QoS
          uint16_t payloadStart = 2 + topicLength;
          uint8_t qos = (packetType & 0x06) >> 1;
          uint16_t packetId = 0;

          if (qos > 0) {
            // Extract packet ID for QoS > 0
            packetId = (payload[payloadStart] << 8) | payload[payloadStart + 1];
            payloadStart += 2;

            // Send acknowledgment
            if (qos == 1) {
              // Send PUBACK
              uint8_t buffer[4];
              buffer[0] = MQTT_PUBACK << 4;
              buffer[1] = 2;  // Remaining length
              buffer[2] = packetId >> 8;
              buffer[3] = packetId & 0xFF;
              _sendPacket(buffer, 4);
              _debugPrintf("Sent PUBACK for packet ID: %d", packetId);
            } else if (qos == 2) {
              // Send PUBREC
              uint8_t buffer[4];
              buffer[0] = MQTT_PUBREC << 4;
              buffer[1] = 2;  // Remaining length
              buffer[2] = packetId >> 8;
              buffer[3] = packetId & 0xFF;
              _sendPacket(buffer, 4);
              _debugPrintf("Sent PUBREC for packet ID: %d", packetId);
            }
          }

          // Extract payload
          uint16_t payloadLength = length - payloadStart;

          if (_callback) {
            _callback(topic, &payload[payloadStart], payloadLength);
          } else {
            // Store for retrieval via getTopic() and getPayload()
            _lastTopic = String(topic);
            _lastPayload = "";

            for (uint16_t i = 0; i < payloadLength; i++) {
              _lastPayload += (char)payload[payloadStart + i];
            }

            _lastPayloadLength = payloadLength;
          }

          delete[] topic;
        }
        break;
      }

    case MQTT_PUBACK:
      if (length >= 2) {
        uint16_t packetId = (payload[0] << 8) | payload[1];
        _handleQoSPubAck(packetId);
      }
      break;

    case MQTT_PUBREC:
      if (length >= 2) {
        uint16_t packetId = (payload[0] << 8) | payload[1];
        _handleQoSPubRec(packetId);
      }
      break;

    case MQTT_PUBREL:
      if (length >= 2) {
        uint16_t packetId = (payload[0] << 8) | payload[1];
        _handleQoSPubRel(packetId);
      }
      break;

    case MQTT_PUBCOMP:
      if (length >= 2) {
        uint16_t packetId = (payload[0] << 8) | payload[1];
        _handleQoSPubComp(packetId);
      }
      break;

    case MQTT_SUBACK:
      // Handle SUBACK
      break;

    case MQTT_UNSUBACK:
      // Handle UNSUBACK
      break;

    case MQTT_PINGRESP:
      // Keep alive ping response received
      _debugPrintln("Received PINGRESP");
      break;
  }
}

void FmsMqtt::_handleQoSPubAck(uint16_t packetId) {
  _debugPrintf("Received PUBACK for packet ID: %d", packetId);

  // Remove message from QoS queue
  for (auto it = _qosMessages.begin(); it != _qosMessages.end(); ++it) {
    if (it->packetId == packetId && it->state == MQTT_PUBACK) {
      _qosMessages.erase(it);
      break;
    }
  }
}

void FmsMqtt::_handleQoSPubRec(uint16_t packetId) {
  _debugPrintf("Received PUBREC for packet ID: %d", packetId);

  // Send PUBREL
  uint8_t buffer[4];
  buffer[0] = MQTT_PUBREL << 4 | 0x02;  // PUBREL with required flags
  buffer[1] = 2;                        // Remaining length
  buffer[2] = packetId >> 8;
  buffer[3] = packetId & 0xFF;
  _sendPacket(buffer, 4);

  // Update message state
  for (auto& msg : _qosMessages) {
    if (msg.packetId == packetId && msg.state == MQTT_PUBREC) {
      msg.state = MQTT_PUBCOMP;
      msg.timestamp = millis();
      break;
    }
  }
}

void FmsMqtt::_handleQoSPubRel(uint16_t packetId) {
  _debugPrintf("Received PUBREL for packet ID: %d", packetId);

  // Send PUBCOMP
  uint8_t buffer[4];
  buffer[0] = MQTT_PUBCOMP << 4;
  buffer[1] = 2;  // Remaining length
  buffer[2] = packetId >> 8;
  buffer[3] = packetId & 0xFF;
  _sendPacket(buffer, 4);
}

void FmsMqtt::_handleQoSPubComp(uint16_t packetId) {
  _debugPrintf("Received PUBCOMP for packet ID: %d", packetId);

  // Remove message from QoS queue
  for (auto it = _qosMessages.begin(); it != _qosMessages.end(); ++it) {
    if (it->packetId == packetId && it->state == MQTT_PUBCOMP) {
      _qosMessages.erase(it);
      break;
    }
  }
}

void FmsMqtt::_checkQoSTimeout() {
  unsigned long currentTime = millis();

  for (auto it = _qosMessages.begin(); it != _qosMessages.end();) {
    // Check if message has timed out (5 seconds)
    if (currentTime - it->timestamp > 5000) {
      // Increment retry count
      it->retries++;

      // If max retries reached (3), remove message
      if (it->retries > 3) {
        _debugPrintf("Max retries reached for packet ID: %d", it->packetId);
        it = _qosMessages.erase(it);
        continue;
      }

      // Resend message
      _debugPrintf("Resending QoS message, packet ID: %d, retry: %d", it->packetId, it->retries);

      if (it->state == MQTT_PUBACK || it->state == MQTT_PUBREC) {
        // Resend PUBLISH
        publish(it->topic.c_str(), it->payload.c_str(), it->payload.length(), it->qos, it->retain);
      } else if (it->state == MQTT_PUBCOMP) {
        // Resend PUBREL
        uint8_t buffer[4];
        buffer[0] = MQTT_PUBREL << 4 | 0x02;
        buffer[1] = 2;
        buffer[2] = it->packetId >> 8;
        buffer[3] = it->packetId & 0xFF;
        _sendPacket(buffer, 4);
      }

      // Update timestamp
      it->timestamp = currentTime;
    }

    ++it;
  }
}

bool FmsMqtt::_topicMatches(const char* subscription, const char* topic) {
  // Simple topic matching algorithm
  const char* sub = subscription;
  const char* top = topic;

  while (*sub && *top) {
    if (*sub == '+') {
      // Single-level wildcard
      sub++;
      while (*top && *top != '/') top++;
    } else if (*sub == '#') {
      // Multi-level wildcard
      return true;
    } else if (*sub != *top) {
      return false;
    } else {
      sub++;
      top++;
    }
  }

  return (*sub == '\0' && *top == '\0') || (*sub == '#');
}

void FmsMqtt::_debugPrint(const char* message) {
  if (_debug) {
    Serial.print("[MQTT]: ");
    Serial.print(message);
  }
}

void FmsMqtt::_debugPrintln(const char* message) {
  if (_debug) {
    Serial.print("[MQTT]: ");
    Serial.println(message);
  }
}

void FmsMqtt::_debugPrintf(const char* format, ...) {
  if (_debug) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Serial.print("[MQTT]: ");
    Serial.println(buffer);
  }
}

uint16_t FmsMqtt::_buildConnect() {
  // Fixed header + protocol name length + protocol name + protocol level +
  // connect flags + keep alive + client ID length + client ID
  uint16_t length = 2 + 2 + 4 + 1 + 1 + 2 + 2 + _clientId.length();

  // Add will topic and message length if present
  if (!_willTopic.isEmpty()) {
    length += 2 + _willTopic.length() + 2 + _willMessage.length();
  }

  // Add username length if present
  if (!_username.isEmpty()) {
    length += 2 + _username.length();
  }

  // Add password length if present
  if (!_password.isEmpty()) {
    length += 2 + _password.length();
  }

  return length;
}
