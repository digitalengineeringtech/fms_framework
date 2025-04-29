#ifdef USE_MUX_PC817
// reference from 74hc lib channel chooser
 void selectMuxChannel(uint8_t channel){
    digitalWrite(MUX_S0, channel & 0x01); // on 
    digitalWrite(MUX_S1, (channel >> 1) & 0x01);// off
    vTaskDelay(pdMS_TO_TICKS(1)); // switch time on off time
 }

 void enable_mux(int pin){
    digitalWrite(pin,LOW);
 }

 void disable_mux(int pin){
    digitalWrite(pin,HIGH);
 }

 void test_mux(){
    for (int device = 0; device < 4; device++) {
        selectMuxChannel(device);
        fms_uart2_serial.println("Hello device " + String(device));
        vTaskDelay(pdMS_TO_TICKS(1000)); // switch time on off time
        if (fms_uart2_serial.available()) {
          String reply = fms_uart2_serial.readStringUntil('\n');
          FMS_LOG_DEBUG("FROM DEVICE :%d,[%s]\n", device, reply.c_str());
        }
      }
 }


#endif