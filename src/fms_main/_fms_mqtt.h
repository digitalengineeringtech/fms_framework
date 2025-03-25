/*
* fms mqtt client 
* maintained by : Trion (nck)
* copyright :@2025 iih
* guided by :Sir Thiha Kyaw (Senior Engineer)
*/

#ifndef FMS_MQTT_H
#define FMS_MQTT_H

#include <Arduino.h>
#include <WiFi.h>
#include <vector>
#include <map>

// MQTT Constants
#define MQTT_VERSION_3_1      3
#define MQTT_VERSION_3_1_1    4
#define MQTT_VERSION_5_0      5

// MQTT Message Types
#define MQTT_CONNECT      1
#define MQTT_CONNACK      2
#define MQTT_PUBLISH      3
#define MQTT_PUBACK       4
#define MQTT_PUBREC       5
#define MQTT_PUBREL       6
#define MQTT_PUBCOMP      7
#define MQTT_SUBSCRIBE    8
#define MQTT_SUBACK       9
#define MQTT_UNSUBSCRIBE  10
#define MQTT_UNSUBACK     11
#define MQTT_PINGREQ      12
#define MQTT_PINGRESP     13
#define MQTT_DISCONNECT   14

// MQTT QoS Levels
#define MQTT_QOS0         0
#define MQTT_QOS1         1
#define MQTT_QOS2         2

// Connect flags
#define MQTT_CLEAN_SESSION    0x02
#define MQTT_WILL_FLAG        0x04
#define MQTT_WILL_QOS_0       0x00
#define MQTT_WILL_QOS_1       0x08
#define MQTT_WILL_QOS_2       0x10
#define MQTT_WILL_RETAIN      0x20
#define MQTT_USERNAME_FLAG    0x40
#define MQTT_PASSWORD_FLAG    0x80

// Default values
#define MQTT_DEFAULT_KEEPALIVE    60  // 60 seconds
#define MQTT_MAX_PACKET_SIZE      2048
#define MQTT_MAX_TOPIC_LENGTH     128
#define MQTT_SOCKET_TIMEOUT       15  // 15 seconds

// MQTT Connection return codes
#define MQTT_CONNECTION_ACCEPTED                0
#define MQTT_CONNECTION_REFUSED_PROTOCOL        1
#define MQTT_CONNECTION_REFUSED_IDENTIFIER      2
#define MQTT_CONNECTION_REFUSED_SERVER          3
#define MQTT_CONNECTION_REFUSED_USER_PASS       4
#define MQTT_CONNECTION_REFUSED_NOT_AUTHORIZED  5

// MQTT Error codes
#define MQTT_ERROR_NONE                 0
#define MQTT_ERROR_NETWORK_CONNECTION   1
#define MQTT_ERROR_PROTOCOL_VERSION     2
#define MQTT_ERROR_INVALID_ID           3
#define MQTT_ERROR_SERVER_UNAVAILABLE   4
#define MQTT_ERROR_BAD_CREDENTIALS      5
#define MQTT_ERROR_NOT_AUTHORIZED       6
#define MQTT_ERROR_MALFORMED_PACKET     7
#define MQTT_ERROR_TIMEOUT              8
#define MQTT_ERROR_QOS_FAILURE          9

class FmsMqtt {
public:
    // Constructor
    FmsMqtt();
    
    // Connection methods
    bool connect(const char* host, uint16_t port, const char* clientId);
    bool connect(const char* host, uint16_t port, const char* clientId, 
                const char* username, const char* password);
    bool connect(const char* host, uint16_t port, const char* clientId,
                const char* username, const char* password,
                const char* willTopic, uint8_t willQos, bool willRetain, 
                const char* willMessage);
    void disconnect();
    
    // Publishing methods
    bool publish(const char* topic, const char* payload, bool retain = false);
    bool publish(const char* topic, const char* payload, uint16_t length, bool retain = false);
    bool publish(const char* topic, const char* payload, uint16_t length, uint8_t qos, bool retain = false);
    
    // Subscription methods
    bool subscribe(const char* topic);
    bool subscribe(const char* topic, uint8_t qos);
    bool unsubscribe(const char* topic);
    
    // Message handling
    void loop();
    bool available();
    String getTopic();
    String getPayload();
    uint16_t getPayloadLength();
    
    // Callback function type definition
    typedef void (*MqttCallback)(char* topic, byte* payload, unsigned int length);
    void setCallback(MqttCallback callback);
    
    // Connection status
    bool connected();
    int getLastError();
    String getErrorString();
    
    // Configuration
    void setKeepAlive(uint16_t keepAliveInterval);
    void setSocketTimeout(uint16_t timeout);
    void setMaxPacketSize(uint16_t size);
    void setCleanSession(bool cleanSession);
    void setProtocolVersion(uint8_t version);
    
    // Debug
    void enableDebug(bool enable = true);
    
private:
    WiFiClient _client;
    String _host;
    uint16_t _port;
    String _clientId;
    String _username;
    String _password;
    String _willTopic;
    String _willMessage;
    uint8_t _willQos;
    bool _willRetain;
    bool _connected;
    bool _cleanSession;
    uint8_t _protocolVersion;
    
    // Configuration
    uint16_t _keepAliveInterval;
    uint16_t _socketTimeout;
    uint16_t _maxPacketSize;
    
    // Callback function
    MqttCallback _callback;
    
    // Last message received
    String _lastTopic;
    String _lastPayload;
    uint16_t _lastPayloadLength;
    
    // Keep alive tracking
    unsigned long _lastPingTime;
    unsigned long _lastIncomingTime;
    
    // Error tracking
    int _lastError;
    
    // Debug
    bool _debug;
    
    // Packet ID management
    uint16_t _nextPacketId;
    uint16_t getNextPacketId();
    
    // QoS message tracking
    struct QoSMessage {
        uint16_t packetId;
        String topic;
        String payload;
        uint8_t qos;
        bool retain;
        uint8_t state;
        unsigned long timestamp;
        uint8_t retries;
    };
    std::vector<QoSMessage> _qosMessages;
    
    // Subscription tracking
    struct Subscription {
        String topic;
        uint8_t qos;
    };
    std::vector<Subscription> _subscriptions;
    
    // Internal methods
    uint16_t _buildConnect();
    uint16_t _buildPublish(const char* topic, const char* payload, uint16_t length, uint8_t qos, bool retain, uint16_t& packetId);
    uint16_t _buildSubscribe(const char* topic, uint8_t qos, uint16_t& packetId);
    uint16_t _buildUnsubscribe(const char* topic, uint16_t& packetId);
    uint16_t _buildDisconnect();
    uint16_t _buildPingReq();
    uint16_t _buildPubAck(uint16_t packetId);
    uint16_t _buildPubRec(uint16_t packetId);
    uint16_t _buildPubRel(uint16_t packetId);
    uint16_t _buildPubComp(uint16_t packetId);
    
    bool _sendPacket(uint8_t* buffer, uint16_t length);
    bool _receivePacket();
    uint16_t _readPacketLength();
    void _processPacket(uint8_t packetType, uint8_t* payload, uint16_t length);
    
    // QoS handling
    void _handleQoSPublish(uint16_t packetId, uint8_t qos);
    void _handleQoSPubAck(uint16_t packetId);
    void _handleQoSPubRec(uint16_t packetId);
    void _handleQoSPubRel(uint16_t packetId);
    void _handleQoSPubComp(uint16_t packetId);
    void _checkQoSTimeout();
    
    // Topic matching
    bool _topicMatches(const char* subscription, const char* topic);
    
    // Debug helpers
    void _debugPrint(const char* message);
    void _debugPrintln(const char* message);
    void _debugPrintf(const char* format, ...);
};

#endif

