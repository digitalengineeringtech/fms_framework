# Simulate incoming message from serial or some source
incoming_message = "03P0001000" #01approv

# Convert to bytes (like C-style char array)
pump_appro_array = incoming_message.encode('utf-8')  # result: b'01approv'

# Print each byte as hex (like Serial.print(buffer[i], HEX) in Arduino)
print("Hex output:")
for i, byte in enumerate(pump_appro_array):
    print(f"{i}/FE/0x{byte:02X}", end=" ")
print()

# If you want to split the parts (e.g., '01' and 'approv')
pump_id = incoming_message[:2]
command = incoming_message[2:]

print(f"\nPump ID: {pump_id}")
print(f"Command: {command}")
