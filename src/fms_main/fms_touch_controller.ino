#ifndef USE_TOUCH

void fms_touch_init() {
eeprom_init();
default_setup_all();
mqtt_topic_change();
screen_brightness_in();

}

/* old version */
void fms_touch_main_loop() {
  if (brightness_check) screen_brightness_out();
    hmi_two_main_fun();
}

void hmi_two_main_fun() {  // for hmi two main function
  Hmi_Two_setup();
  if (rfid_check_two) rfid_setup_two();
  if (rfid_auth_check_two) rfid_auth_two();
}

void Hmi_Two_setup() {

  if (Serial2.available() > 0) {

    Hmi_time = millis() / 1000;  // for brightness time

    for (int i = 0; i < 40; i++) {
      Hmi_Buffer[i] = Serial2.read();
      delay(2);
      // Serial.print(Hmi_Buffer[i], HEX);
      // Serial.print(" ");
    }
    // Serial.println();

    if (Hmi_Buffer[4] == 0x12) {
      rfid_card_check_two = false;

      switch (Hmi_Buffer[8]) {
        case 0x01:
          buzzer_song_two();
          common_pump_Id_two = pumpid1;
          dispenser_ID_two = 1;
          break;

        case 0x02:
          buzzer_song_two();
          common_pump_Id_two = pumpid2;
          dispenser_ID_two = 2;
          break;

        case 0x03:
          buzzer_song_two();
          common_pump_Id_two = pumpid3;
          dispenser_ID_two = 3;

          break;
        case 0x04:
          buzzer_song_two();
          common_pump_Id_two = pumpid4;
          dispenser_ID_two = 4;
          break;

        case 0x05:
          buzzer_song_two();
          common_pump_Id_two = pumpid5;
          dispenser_ID_two = 5;
          break;

        case 0x06:
          buzzer_song_two();
          common_pump_Id_two = pumpid6;
          dispenser_ID_two = 6;
          break;

        case 0x07:
          buzzer_song_two();
          common_pump_Id_two = pumpid7;
          dispenser_ID_two = 7;
          break;

        case 0x08:
          buzzer_song_two();
          common_pump_Id_two = pumpid8;
          dispenser_ID_two = 8;
          break;
      }

      if (casher_lock_check == 1) {
        master_http_login_post_two(master_rfid);

        if (master_check) {
          vehicle_setup_two();
          master_check = false;

        } else {
          Page_change[9] = 0x05;
          Serial2.write(Page_change, 10);
          Serial2.write(Page_change, 10);
        }

      } else rfid_check_two = true;

    } else if (Hmi_Buffer[4] == 0x23 && Hmi_Buffer[5] == 0x00) {  // to read car no
      // Serial.println(" read car no");
      for (int j = 0; j < 15; j++) Car_no_two[j] = 0;

      for (int i = 0; i < 15; i++) {
        if (Hmi_Buffer[7 + i] == 0xff) break;
        Car_no_two[i] = Hmi_Buffer[7 + i];
        delay(1);
      }
      Serial.print("Hmi two Car number is => ");
      Serial.println(Car_no_two);

    } else if (Hmi_Buffer[4] == 0x24 && Hmi_Buffer[5] == 0x00) {  // to read member id
      for (int j = 0; j < 20; j++) Member_id_two[j] = 0;

      for (int i = 0; i < 15; i++) {
        if (Hmi_Buffer[7 + i] == 0xff) break;
        Member_id_two[i] = Hmi_Buffer[7 + i];
        delay(1);
      }
      Serial.print("Hmi two Member id is => ");
      Serial.println(Member_id_two);

    } else if (Hmi_Buffer[4] == 0x25 && Hmi_Buffer[5] == 0x00) {  // to read member name
      for (int j = 0; j < 20; j++) Member_name_two[j] = 0;

      for (int i = 0; i < 15; i++) {
        if (Hmi_Buffer[7 + i] == 0xff) break;
        Member_name_two[i] = Hmi_Buffer[7 + i];
        delay(1);
      }
      Serial.print("Hmi two Member name is => ");
      Serial.println(Member_name_two);

    } else if (Hmi_Buffer[4] == 0x22 && Hmi_Buffer[5] == 0x00) {  // to read vehicle type
      for (int j = 0; j < 4; j++) vehicle_type_two[j] = 0;

      for (int i = 0; i < 4; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        vehicle_type_two[i] = Hmi_Buffer[8 + i];
      }
      // Serial.println(vehicle_type_two);
      vehicle_value_two = vehicle_type_two[0] + 1;
      Serial.print("Hmi two vehicle type is => ");
      Serial.println(vehicle_value_two);

      // vehicle_type_name_two(vehicle_value_two);  // to setup vehicle name

    } else if (Hmi_Buffer[4] == 0x21 && Hmi_Buffer[5] == 0x00) {  // to read cash type id
      for (int j = 0; j < 4; j++) cash_type_two[j] = 0;

      for (int i = 0; i < 4; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        cash_type_two[i] = Hmi_Buffer[8 + i];
      }

      cash_value_two = cash_type_two[0] + 1;
      Serial.print("Hmi two Cash type id => ");
      Serial.println(cash_value_two);

    } else if (Hmi_Buffer[4] == 0x32 && Hmi_Buffer[5] == 0x00) {  // for liter calculation and read liter
      Serial.println(" liter count in hmi two");

      Total_check_two = false;
      over_all_li_price_check_two = true;
      liter_check_two = true;
      price_check_two = false;
      preset_check_two = false;

      for (int j = 0; j < 10; j++) Liter_count_two[j] = 0;

      for (int i = 0; i < 10; i++) {
        if (Hmi_Buffer[9 + i] == 0xff) break;
        Liter_count_two[i] = Hmi_Buffer[9 + i];
        Serial.print(Liter_count_two[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      combine_Liter_two = (Liter_count_two[0] << 8) | Liter_count_two[1];
      Serial.print("Hmi two Liter count is => ");
      Serial.println(combine_Liter_two);

      liter_calculation_check_two(combine_Liter_two);  // for calculation

    } else if (Hmi_Buffer[4] == 0x33 && Hmi_Buffer[5] == 0x00) {  // to read price data

      over_all_li_price_check_two = true;
      liter_check_two = false;
      price_check_two = true;
      Total_check_two = false;
      preset_check_two = false;

      for (int j = 0; j < 5; j++) Price_count_two[j] = 0;

      for (int i = 0; i < 5; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        Price_count_two[i] = Hmi_Buffer[8 + i];
        Serial.print(Price_count_two[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      combine_price_two = (Price_count_two[0] << 16) | (Price_count_two[1] << 8) | Price_count_two[2];
      Serial.print("Hmi two price count is => ");
      Serial.println(combine_price_two);

      price_calculation_check_two(combine_price_two);

    } else if (Hmi_Buffer[4] == 0x4A && Hmi_Buffer[5] == 0x11) {  // to read F1 liter value
      // Serial.println("f1 read liter in hmi two");
      F1_liter_check_two = true;
      F1_price_check_two = false;
      for (int j = 0; j < 5; j++) F1_liter_two[j] = 0;

      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[9 + i] == 0xff) break;
        F1_liter_two[i] = Hmi_Buffer[9 + i];
      }
      // Serial.println();

      F1_liter_value_two = (F1_liter_two[0] << 8) | F1_liter_two[1];
      Serial.print("Hmi two F1 liter value is => ");
      Serial.println(F1_liter_value_two);

      Default_preset_price[4] = 0x4A;
      Default_preset_price[5] = 0x21;

      Serial2.write(Default_preset_price, 10);
      Serial2.write(Default_preset_price, 10);

    } else if (Hmi_Buffer[4] == 0x4A && Hmi_Buffer[5] == 0x21) {  // to read F1 price
      // Serial.println("f1 read price in hmi two");
      F1_liter_check_two = false;
      F1_price_check_two = true;
      for (int j = 0; j < 8; j++) F1_price_two[j] = 0;
      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        F1_price_two[i] = Hmi_Buffer[8 + i];
      }

      F1_price_value_two = (F1_price_two[0] << 16) | (F1_price_two[1] << 8) | F1_price_two[2];
      Serial.print("Hmi two F1 price value is => ");
      Serial.println(F1_price_value_two);

      Default_preset_liter[4] = 0x4A;
      Serial2.write(Default_preset_liter, 10);
      Serial2.write(Default_preset_liter, 10);

    } else if (Hmi_Buffer[4] == 0x4B && Hmi_Buffer[5] == 0x11) {  // to read F2 liter value
      F2_liter_check_two = true;
      F2_price_check_two = false;
      for (int j = 0; j < 5; j++) F2_liter_two[j] = 0;

      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[9 + i] == 0xff) break;
        F2_liter_two[i] = Hmi_Buffer[9 + i];
      }

      F2_liter_value_two = (F2_liter_two[0] << 8) | F2_liter_two[1];
      Serial.print("Hmi two F2 liter value is => ");
      Serial.println(F2_liter_value_two);

      Default_preset_price[4] = 0x4B;
      Default_preset_price[5] = 0x21;
      Serial2.write(Default_preset_price, 10);
      Serial2.write(Default_preset_price, 10);

    } else if (Hmi_Buffer[4] == 0x4B && Hmi_Buffer[5] == 0x21) {  // to read F2 price
      Serial.println("f2 read price in hmi two");
      F2_liter_check_two = false;
      F2_price_check_two = true;
      for (int j = 0; j < 8; j++) F2_price_two[j] = 0;
      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        F2_price_two[i] = Hmi_Buffer[8 + i];
      }

      F2_price_value_two = (F2_price_two[0] << 16) | (F2_price_two[1] << 8) | F2_price_two[2];

      Serial.print("Hmi two F2 price value is => ");
      Serial.println(F2_price_value_two);

      Default_preset_liter[4] = 0x4B;
      Serial2.write(Default_preset_liter, 10);
      Serial2.write(Default_preset_liter, 10);

    } else if (Hmi_Buffer[4] == 0x4C && Hmi_Buffer[5] == 0x11) {  // to read F3 liter value
      Serial.println("f3 read liter in hmi two");
      F3_liter_check_two = true;
      F3_price_check_two = false;
      for (int j = 0; j < 5; j++) F3_liter_two[j] = 0;

      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[9 + i] == 0xff) break;
        F3_liter_two[i] = Hmi_Buffer[9 + i];
      }

      F3_liter_value_two = (F3_liter_two[0] << 8) | F3_liter_two[1];
      Serial.print("Hmi two F3 liter value is => ");
      Serial.println(F3_liter_value_two);

      Default_preset_price[4] = 0x4C;
      Default_preset_price[5] = 0x31;

      Serial2.write(Default_preset_price, 10);
      Serial2.write(Default_preset_price, 10);

    } else if (Hmi_Buffer[4] == 0x4C && Hmi_Buffer[5] == 0x31) {  // to read F3 price
      Serial.println("f3 read price in hmi two");
      F3_liter_check_two = false;
      F3_price_check_two = true;
      for (int j = 0; j < 8; j++) F3_price_two[j] = 0;
      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        F3_price_two[i] = Hmi_Buffer[8 + i];
      }

      F3_price_value_two = (F3_price_two[0] << 16) | (F3_price_two[1] << 8) | F3_price_two[2];
      Serial.print("Hmi two F3 price value is => ");
      Serial.println(F3_price_value_two);

      Default_preset_liter[4] = 0x4C;
      Serial2.write(Default_preset_liter, 10);
      Serial2.write(Default_preset_liter, 10);
      delay(5);

    } else if (Hmi_Buffer[4] == 0x4D && Hmi_Buffer[5] == 0x11) {  // to read F4 liter value
      Serial.println("f4 read liter in hmi two");
      F4_liter_check_two = true;
      F4_price_check_two = false;
      for (int j = 0; j < 5; j++) F4_liter_two[j] = 0;

      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[9 + i] == 0xff) break;
        F4_liter_two[i] = Hmi_Buffer[9 + i];
        // Serial.print(F4_liter_two[i], HEX);
        // Serial.print(" ");
      }
      // Serial.println();

      F4_liter_value_two = (F4_liter_two[0] << 8) | F4_liter_two[1];
      Serial.print("Hmi two F4 liter value is => ");
      Serial.println(F4_liter_value_two);

      Default_preset_price[4] = 0x4D;
      Default_preset_price[5] = 0x41;

      Serial2.write(Default_preset_price, 10);
      Serial2.write(Default_preset_price, 10);

    } else if (Hmi_Buffer[4] == 0x4D && Hmi_Buffer[5] == 0x41) {  // to read F4 price
      Serial.println("f4 read price in hmi two");
      F4_liter_check_two = false;
      F4_price_check_two = true;
      for (int j = 0; j < 8; j++) F4_price_two[j] = 0;
      for (int i = 0; i < 8; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        F4_price_two[i] = Hmi_Buffer[8 + i];
      }

      F4_price_value_two = (F4_price_two[0] << 16) | (F4_price_two[1] << 8) | F4_price_two[2];
      Serial.print("Hmi two F4 price value is => ");
      Serial.println(F4_price_value_two);

      Default_preset_liter[4] = 0x4D;
      Serial2.write(Default_preset_liter, 10);
      Serial2.write(Default_preset_liter, 10);

    } else if (Hmi_Buffer[4] == 0x4F && Hmi_Buffer[5] == 0x31) {  // to store eeprom F1 value
      Serial.println("F1 save function in hmi two");
      buzzer_song_two();
      if (F1_liter_check_two) {
        F1_price_value_two = 0;
        eeprom_writeInt(150, F1_liter_value_two);
        eeprom_writeInt(155, F1_price_value_two);

        EEPROM.write(154, 1);
        EEPROM.commit();

        F1_liter_check_two = false;
        F1_price_check_two = false;
      }

      if (F1_price_check_two) {
        F1_liter_value_two = 0;
        eeprom_writeInt(150, F1_liter_value_two);
        eeprom_writeInt(155, F1_price_value_two);
        EEPROM.write(154, 2);
        EEPROM.commit();

        F1_price_check_two = false;
        F1_liter_check_two = false;
      }

    } else if (Hmi_Buffer[4] == 0x4F && Hmi_Buffer[5] == 0x32) {  // to store eeprom F2 value
      Serial.println("F2 save function in hmi two");
      buzzer_song_two();
      if (F2_liter_check_two) {
        F2_price_value_two = 0;
        eeprom_writeInt(160, F2_liter_value_two);
        eeprom_writeInt(165, F2_price_value_two);
        EEPROM.write(184, 1);
        EEPROM.commit();

        F2_liter_check_two = false;
        F2_price_check_two = false;
      }

      if (F2_price_check_two) {
        F2_liter_value_two = 0;
        eeprom_writeInt(160, F2_liter_value_two);
        eeprom_writeInt(165, F2_price_value_two);
        EEPROM.write(164, 2);
        EEPROM.commit();

        F2_price_check_two = false;
        F2_liter_check_two = false;
      }
    } else if (Hmi_Buffer[4] == 0x4F && Hmi_Buffer[5] == 0x33) {  // to store eeprom F3 value
      Serial.println("F3 save function in hmi two");
      buzzer_song_two();
      if (F3_liter_check_two) {
        F3_price_value_two = 0;
        eeprom_writeInt(170, F3_liter_value_two);
        eeprom_writeInt(175, F3_price_value_two);
        EEPROM.write(174, 1);
        EEPROM.commit();

        F3_liter_check_two = false;
        F3_price_check_two = false;
      }

      if (F3_price_check_two) {
        F3_liter_value_two = 0;
        eeprom_writeInt(170, F3_liter_value_two);
        eeprom_writeInt(175, F3_price_value_two);
        EEPROM.write(174, 2);
        EEPROM.commit();

        F3_price_check_two = false;
        F3_liter_check_two = false;
      }

    } else if (Hmi_Buffer[4] == 0x4F && Hmi_Buffer[5] == 0x34) {  // to store eeprom F4 value
      Serial.println("F4 save function in hmi two");
      buzzer_song_two();
      if (F4_liter_check_two) {
        F4_price_value_two = 0;
        eeprom_writeInt(180, F4_liter_value_two);
        eeprom_writeInt(185, F4_price_value_two);
        EEPROM.write(184, 1);
        EEPROM.commit();

        F4_liter_check_two = false;
        F4_price_check_two = false;
      }

      if (F4_price_check_two) {
        F4_liter_value_two = 0;
        eeprom_writeInt(180, F4_liter_value_two);
        eeprom_writeInt(185, F4_price_value_two);
        EEPROM.write(184, 2);
        EEPROM.commit();

        F4_price_check_two = false;
        F4_liter_check_two = false;
      }

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x01) {  // F1 value return in preset function

      preset_check_two = true;
      over_all_li_price_check_two = true;

      buzzer_song_two();

      int F1_return_liter_two = eeprom_read_data(150);
      int F1_return_price_two = eeprom_read_data(155);
      Preset_total_eep_check_two = EEPROM.read(154);

      Serial.print("Hmi two F1 liter or price is ===> ");
      Serial.println(Preset_total_eep_check_two);

      Serial.print("Hmi two F1 return liter value is => ");
      Serial.println(F1_return_liter_two);

      Serial.print("Hmi two F1 return price value is => ");
      Serial.println(F1_return_price_two);

      Total_check_two = true;
      if (F1_return_liter_two > 0) {
        liter_calculation_check_two(F1_return_liter_two);
      }

      if (F1_return_price_two > 1) {
        price_calculation_check_two(F1_return_price_two);
      }

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x02) {  // F2 value return in preset function

      preset_check_two = true;
      over_all_li_price_check_two = true;

      buzzer_song_two();

      int F2_return_liter_two = eeprom_read_data(160);
      int F2_return_price_two = eeprom_read_data(165);
      Preset_total_eep_check_two = EEPROM.read(164);

      Serial.print("Hmi two F2 liter or price is => ");
      Serial.println(Preset_total_eep_check_two);

      Serial.print("Hmi two F2 return liter value is => ");
      Serial.println(F2_return_liter_two);

      Serial.print("Hmi twoF2 return price value is => ");
      Serial.println(F2_return_price_two);

      Total_check_two = true;
      if (F2_return_liter_two > 0) {
        liter_calculation_check_two(F2_return_liter_two);
      }

      if (F2_return_price_two > 1) {
        price_calculation_check_two(F2_return_price_two);
      }

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x03) {  // F3 value return in preset function

      preset_check_two = true;
      over_all_li_price_check_two = true;

      buzzer_song_two();

      int F3_return_liter_two = eeprom_read_data(170);
      int F3_return_price_two = eeprom_read_data(175);
      Preset_total_eep_check_two = EEPROM.read(174);

      Serial.print("Hmi two F3 liter or price is => ");
      Serial.println(Preset_total_eep_check_two);

      Serial.print("Hmi two F3 return liter value is => ");
      Serial.println(F3_return_liter_two);

      Serial.print("Hmi two F3 return price value is => ");
      Serial.println(F3_return_price_two);

      Total_check_two = true;
      if (F3_return_liter_two > 0) {
        liter_calculation_check_two(F3_return_liter_two);
      }

      if (F3_return_price_two > 1) {
        price_calculation_check_two(F3_return_price_two);
      }
    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x04) {  // F4 value return in preset function

      preset_check_two = true;
      over_all_li_price_check_two = true;

      buzzer_song_two();

      int F4_return_liter_two = eeprom_read_data(180);
      int F4_return_price_two = eeprom_read_data(185);
      Preset_total_eep_check_two = EEPROM.read(184);

      Serial.print("Hmi two F4 liter or price is => ");
      Serial.println(Preset_total_eep_check_two);

      Serial.print("Hmi two F4 return liter value is => ");
      Serial.println(F4_return_liter_two);

      Serial.print("Hmi two F4 return price value is => ");
      Serial.println(F4_return_price_two);

      Total_check_two = true;
      if (F4_return_liter_two > 0) {
        liter_calculation_check_two(F4_return_liter_two);
      }

      if (F4_return_price_two > 1) {
        price_calculation_check_two(F4_return_price_two);
      }

    } else if (Hmi_Buffer[4] == 0x61 && Hmi_Buffer[5] == 0x00) {  // pin code auth two

      for (int j = 0; j < 5; j++) PIN_code_two[j] = 0;
      for (int i = 0; i < 5; i++) {
        if (Hmi_Buffer[8 + i] == 0xff) break;
        PIN_code_two[i] = Hmi_Buffer[8 + i];
      }

      PIN_CODE_two = (PIN_code_two[0] << 16) | (PIN_code_two[1] << 8) | PIN_code_two[2];
      Serial.print("pin code two is => ");
      Serial.println(PIN_CODE_two);

      PIN_Auth_two();

    } else if (Hmi_Buffer[4] == 0x53 && Hmi_Buffer[5] == 0x00) {  // to read wifi password in hmi two
      Serial.println("Wifi pass read in hmi two");
      for (int j = 0; j < 40; j++) PASS_buffer[j] = 0;
      for (int i = 0; i < 40; i++) {
        if (Hmi_Buffer[7 + i] == 0xff) break;
        PASS_buffer[i] = Hmi_Buffer[7 + i];
        delay(1);
      }
      Serial.print("Wifi pass is => ");
      Serial.println(PASS_buffer);

    } else if (Hmi_Buffer[4] == 0x52 && Hmi_Buffer[5] == 0x00) {  // to read wifi ssid
      Serial.println("wifi ssid read in hmi two");
      for (int j = 0; j < 40; j++) SSID_buffer[j] = 0;

      for (int i = 0; i < 40; i++) {
        if (Hmi_Buffer[7 + i] == 0xff) break;
        SSID_buffer[i] = Hmi_Buffer[7 + i];
        delay(1);
      }
      // Serial.println();

      Serial.print("Hmi two Wifi ssid is => ");
      Serial.println(SSID_buffer);

    } else if (Hmi_Buffer[4] == 0x50 && Hmi_Buffer[6] == 0x01) {  // wifi Connect
      Serial.println("wifi connect function in Hmi two");
      buzzer_song_two();

      Page_change[9] = 0X10;

      Serial2.write(Page_change, 10);
      Serial2.write(Page_change, 10);

      isconnected = true;

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[8] == 0x05) {  // when press reset vehicle info default
      buzzer_song_two();
      vehicle_setup_two();

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[8] == 0x06) {  // when press clear liter and price default
      buzzer_song_two();
      clear_setup_two();

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[8] == 0x07 && route_protect) {  // when press start check liter or price press
      Serial.println("STart in Hmi two");
      buzzer_song_two();

      Fuel_type_name_two(dispenser_ID_two);

      cash_type_name_two(cash_value_two);
      vehicle_type_name_two(vehicle_value_two);  // to setup vehicle name

      if (over_all_li_price_check_two) {
        if (liter_check_two) {
          Serial.println("Hmi two Liter function Active.....");
          Serial.print("Hmi two Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Hmi two Press total liter is => ");
          Serial.println(combine_Liter_two);

          server_post_vehicle_info_two();

          price_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (price_check_two) {
          Serial.println("Hmi two Price function Active ......");
          Serial.print("Hmi two Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Hmi two Press total price is => ");
          Serial.println(combine_price_two);

          server_post_vehicle_info_two();

          liter_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (preset_check_two && Preset_total_eep_check_two == 1) {
          Serial.print("Hmi two Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Hmi two Press total liter is => ");
          Serial.println(Preset_eep_total_liter_two);

          server_post_vehicle_info_two();

          Preset_total_eep_check_two = 0;
          Preset_eep_total_liter_two = 0;
          preset_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (preset_check_two && Preset_total_eep_check_two == 2) {
          Serial.println("Preset price function  Active ......");
          Serial.print("Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Press total price is => ");
          Serial.println(Preset_eep_total_price_two);

          server_post_vehicle_info_two();

          Preset_total_eep_check_two = 0;
          Preset_eep_total_price_two = 0;
          preset_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

      } else {
        Serial.println("Price and Liter function is InActive in start..!");

        server_post_vehicle_info_two();

        Preset_total_eep_check_two = 0;
        Preset_eep_total_price_two = 0;
        Preset_eep_total_liter_two = 0;

        liter_check_two = false;
        price_check_two = false;
        preset_check_two = false;
        over_all_li_price_check_two = false;

        Information_page_show_two();
      }

      route_protect = false;

    } else if (Hmi_Buffer[4] == 0x30 && Hmi_Buffer[8] == 0x08 && route_protect) {  // when press Preset check liter or price press
      Serial.println("Preset");
      buzzer_song_two();

      Fuel_type_name_two(dispenser_ID_two);

      cash_type_name_two(cash_value_two);
      vehicle_type_name_two(vehicle_value_two);  // to setup vehicle name

      if (over_all_li_price_check_two) {
        if (liter_check_two) {
          Serial.println("Liter function Active.....");
          Serial.print("Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print(" press liter is => ");
          Serial.println(combine_Liter_two);

          server_post_preset_two("liter", combine_Liter_two);

          price_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (price_check_two) {
          Serial.println("Price Funtion Active......");
          Serial.print("Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Press total price is => ");
          Serial.println(combine_price_two);

          server_post_preset_two("price", combine_price_two);

          liter_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (preset_check_two && Preset_total_eep_check_two == 1) {
          Serial.println("Preset liter function Active ......");
          Serial.print("Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Press total liter is => ");
          Serial.println(Preset_eep_total_liter_two);

          server_post_preset_two("liter", Preset_eep_total_liter_two);

          Preset_total_eep_check_two = 0;
          Preset_eep_total_liter_two = 0;
          preset_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

        if (preset_check_two && Preset_total_eep_check_two == 2) {
          Serial.println("Preset price function  Active ......");
          Serial.print("Dispenser Active ID => ");
          Serial.println(dispenser_ID_two);
          Serial.print("Press total price is => ");
          Serial.println(Preset_eep_total_price_two);

          server_post_preset_two("price", Preset_eep_total_price_two);

          Preset_total_eep_check_two = 0;
          Preset_eep_total_price_two = 0;
          preset_check_two = false;
          over_all_li_price_check_two = false;

          Information_page_show_two();
        }

      } else {
        Serial.println("Price and Liter function is InActive..!");

        int ab = 0;
        server_post_preset_two("liter", ab);

        Preset_total_eep_check_two = 0;
        Preset_eep_total_price_two = 0;
        Preset_eep_total_liter_two = 0;

        liter_check_two = false;
        price_check_two = false;
        preset_check_two = false;
        over_all_li_price_check_two = false;

        Information_page_show_two();
      }
      Information_page_show_two();

      route_protect = false;

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x10) {  // screen brightness
      Serial.println("Screen brightness");
      buzzer_song_two();
      screen_brightness_in();

    } else if (Hmi_Buffer[4] == 0x41 && Hmi_Buffer[6] == 0x01) {  // page chane in Done key
      Serial.println("Done key");
      buzzer_song_two();

      Page_change[9] = 0x10;

      Serial2.write(Page_change, 10);
      Serial2.write(Page_change, 10);

    } else if (Hmi_Buffer[4] == 0x10 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x07) {
      Serial.println("Reset Button");
      buzzer_song_two();
      String dvc_id = "";
      if (devicenum < 10) dvc_id = "0" + String(devicenum);
      else dvc_id = String(devicenum);
      client.publish(Reset_topic, dvc_id.c_str());
      delay(5);
      ESP.restart();

    } else if (Hmi_Buffer[4] == 0x31 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x03) {  // security key
      Serial.println("security preset key in hmi two");
      buzzer_song_two();
      casher_lock_check = EEPROM.read(90);
      Serial.print("security key is => ");
      Serial.println(casher_lock_check);

      if (casher_lock_check == 1) {
        Show_casher_lock[7] = 0x06;
        Serial2.write(Show_casher_lock, 8);
        Serial2.write(Show_casher_lock, 8);

      } else if (casher_lock_check == 0) {
        Show_casher_lock[7] = 0x05;
        Serial2.write(Show_casher_lock, 8);
        Serial2.write(Show_casher_lock, 8);
      }

    } else if (Hmi_Buffer[4] == 0x42 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x06) {  // casher unlock
      Serial.println("chasher is unlcok in hmi two");
      buzzer_song_two();
      casher_lock_check = 1;
      EEPROM.write(90, casher_lock_check);
      EEPROM.commit();
      http_login_post_two(master_rfid);

    } else if (Hmi_Buffer[4] == 0x42 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x05) {  // casher lock
      Serial.println("chasher is lcok in hmi two");
      buzzer_song_two();
      casher_lock_check = 0;
      EEPROM.write(90, casher_lock_check);
      EEPROM.commit();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x03) {  // setting key
      Serial.println("setting key in hmi two");
      buzzer_song_two();

      // Page_change[9] = 0x18;
      // Serial2.write(Page_change, 10);
      // Serial2.write(Page_change, 10);

      Serial2.write(Default_pin_code, 10);
      Serial2.write(Default_pin_code, 10);

      Serial2.write(Clean_wifi_pass, 8);
      Serial2.write(Clean_wifi_pass, 8);

      Serial2.write(Default_wifi_ssid, 18);
      delay(5);
      Serial2.write(Default_wifi_ssid, 18);
      delay(5);

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x20 && Hmi_Buffer[8] == 0x01) {  // member card
      Serial.println("member card in hmi two");
      buzzer_song_two();
      rfid_card_check_two = true;
      rfid_check_two = true;

    } else if (Hmi_Buffer[4] == 0x31 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x03) {  // lock dispenser
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x31 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x04) {  // price setting
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x02 && Hmi_Buffer[8] == 0x01) {  // member info page
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x02 && Hmi_Buffer[8] == 0x02) {  // casher info
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x02 && Hmi_Buffer[8] == 0x03) {  // credit balance page
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x02 && Hmi_Buffer[8] == 0x04) {  // Device info page
      buzzer_song_two();

    } else if (Hmi_Buffer[4] == 0x31 && Hmi_Buffer[6] == 0x01 && Hmi_Buffer[8] == 0x02) {  // to write fun preset data in eeprom
      Serial.println("funtion preset pressed for hmi two");
      F1_liter_check_two = false;
      F1_price_check_two = false;
      buzzer_song_two();

      preset_eeprom_value();
      preset_rewrite_price_two();
      preset_rewrite_liter_two();

      Serial2.write(Clean_wifi_pass, 8);
      Serial2.write(Clean_wifi_pass, 8);

      Serial2.write(Default_wifi_ssid, 18);
      delay(5);
      Serial2.write(Default_wifi_ssid, 18);
      delay(5);

    } else if (Hmi_Buffer[4] == 0x41 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[6] == 0x01) {  // function preset page done key
      Serial.println("Done key in");
      buzzer_song_two();
      Information_page_show_two();

    } else if (Hmi_Buffer[4] == 0x43 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[6] == 0x01) {  // done key in security
      Serial.println("Done key in security");
      buzzer_song_two();
      Information_page_show_two();

    } else if (Hmi_Buffer[4] == 0x31 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x01) {  // for to reset wifi default
      buzzer_song_two();
      Serial.println("wifi setting");

    } else if (Hmi_Buffer[4] == 0x9F && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x01) {  // page change in OK key
      Serial.println("ok key in hmi two");
      route_protect = true;
      rfid_check_two = false;
      rfid_auth_check_two = false;

      buzzer_song_two();
      clear_setup_two();
      page_check_fun_two(nozzlenum);

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x01) {  // page change in Cancel key
      Serial.println("cancle key");
      rfid_check_two = false;
      rfid_auth_check_two = false;
      buzzer_song_two();
      page_check_fun_two(nozzlenum);

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x01 && Hmi_Buffer[8] == 0x01) {  // page change in next key in vehicle info page
      Serial.println("next presssed in hmi two");
      buzzer_song_two();
      Preset_setup_two();

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[8] == 0x04) {  // member card page back key
      Serial.println("back key in member card");
      rfid_check_two = false;
      rfid_auth_check_two = false;
      buzzer_song_two();

      Page_change[9] = 0X04;
      Serial2.write(Page_change, 10);
      Serial2.write(Page_change, 10);

    } else if (Hmi_Buffer[4] == 0x20 && Hmi_Buffer[5] == 0x00 && Hmi_Buffer[6] == 0x01 && Hmi_Buffer[8] == 0x00) {  // page change in back key
      Serial.println("back key in hmi two");
      route_protect = true;
      rfid_check_two = false;
      rfid_auth_check_two = false;
      buzzer_song_two();
      clear_setup_two();
      page_check_fun_two(nozzlenum);
    }
  }
}

void master_http_login_post_two(String rfid_data) {  // when access rfid roken is receive

  HTTPClient http;

  Serial.print("rfid Id is => ");
  Serial.println(rfid_data);
  String serverAddress = "http://192.168.0.100:9000/api/user/cardAuth";

  http.begin(serverAddress);
  String formDataBoundary = "----WebKitFormBoundary";
  String formData = "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"cardId\"\r\n\r\n";
  formData += rfid_data + "\r\n";
  formData += "--" + formDataBoundary + "--\r\n";

  http.addHeader("Content-Type", "multipart/form-data; boundary=" + formDataBoundary);

  int httpResponseCode = http.POST(formData);


  // Check for a successful response
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();

    // Parse the JSON response
    // Serial.print("Response is => ");
    // Serial.println(response);
    DynamicJsonDocument doc(2048);

    DeserializationError error = deserializeJson(doc, response);
    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    master_check = doc["con"];
    String token = doc["result"]["token"];

    accessToken_two = "";
    accessToken_two = token;

    // Print the token
    Serial.print("rfid condition : ");
    Serial.println(rfid_condition);

    Serial.print("Token: ");
    Serial.println(accessToken_two);

  } else {
    Serial.print("HTTP POST request failed, error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void http_login_post_two(String rfid_data) {  // when access rfid roken is receive

  HTTPClient http;

  Serial.print("rfid Id is => ");
  Serial.println(rfid_data);
  String serverAddress = "http://192.168.0.100:9000/api/user/cardAuth";

  http.begin(serverAddress);
  String formDataBoundary = "----WebKitFormBoundary";
  String formData = "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"cardId\"\r\n\r\n";
  formData += rfid_data + "\r\n";
  formData += "--" + formDataBoundary + "--\r\n";

  http.addHeader("Content-Type", "multipart/form-data; boundary=" + formDataBoundary);

  int httpResponseCode = http.POST(formData);

  // Check for a successful response
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();

    // Parse the JSON response
    // Serial.print("Response is => ");
    // Serial.println(response);
    DynamicJsonDocument doc(2048);

    DeserializationError error = deserializeJson(doc, response);
    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    rfid_condition = doc["con"];
    String token = doc["result"]["token"];

    accessToken_two = "";
    accessToken_two = token;

    // Print the token
    Serial.print("rfid condition : ");
    Serial.println(rfid_condition);

    Serial.print("Token: ");
    Serial.println(accessToken_two);

  } else {
    Serial.print("HTTP POST request failed, error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void server_post_vehicle_info_two() {  /// for http post for vehicle info

  String nozzleNo = "";
  if (common_pump_Id_two < 10) nozzleNo = "0" + String(common_pump_Id_two);
  else nozzleNo = String(common_pump_Id_two);

  String Dev_ID = String(devicenum);


  String serverAddress = "http://192.168.0.100:9000/api/detail-sale?depNo=" + Dev_ID + "&nozzleNo=" + nozzleNo;

  // Serial.print("server Address two => ");
  // Serial.println(serverAddress);

  String carNo;
  if (strlen(Car_no_two) == 0) carNo = "-";
  else carNo = String(Car_no_two);

  String member_id;
  if (strlen(Member_id_two) == 0) member_id = "-";
  else member_id = String(Member_id_two);

  String member_name;
  if (strlen(Member_name_two) == 0) member_name = "-";
  else member_name = String(Member_name_two);

  String vehicleType = vehicle_name_two;
  String fuelType = Nozzle_name_two;
  String cashType = cash_name_two;
  String device = "HMI";

  HTTPClient http;

  http.begin(serverAddress);

  http.addHeader("Authorization", "Bearer " + String(accessToken_two));

  String formDataBoundary = "----WebKitFormBoundary";
  String formData = "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"nozzleNo\"\r\n\r\n";
  formData += nozzleNo + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"vehicleType\"\r\n\r\n";
  formData += vehicleType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"carNo\"\r\n\r\n";
  formData += carNo + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"cashType\"\r\n\r\n";
  formData += cashType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"fuelType\"\r\n\r\n";
  formData += fuelType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"device\"\r\n\r\n";
  formData += device + "\r\n";
  formData += "--" + formDataBoundary + "--\r\n";

  http.addHeader("Content-Type", "multipart/form-data; boundary=" + formDataBoundary);

  int httpResponseCode = http.POST(formData);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    Serial.println(response);

    DynamicJsonDocument doc(2048);

    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    rfid_condition = doc["con"];
    String Server_reply = doc["msg"];

    Serial.print("server condi  is => ");
    Serial.println(rfid_condition);

    Serial.print("server reply from start is => ");
    Serial.println(Server_reply);

    if (rfid_condition) {
      char my_chr[30];
      Server_reply.toCharArray(my_chr, Server_reply.length() + 1);

      Serial2.write(Show_server_reply, 6);
      Serial2.write(Show_server_data, 20);

      Serial2.write(Show_infor, 6);
      Serial2.write(my_chr, 30);

    } else {

      Serial2.write(Show_server_Error, 21);
      Serial2.write(Show_server_Error, 21);
    }

  } else {
    Serial.print("HTTP POST request failed, error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  Serial.println("http post is complete");
  accessToken_two = "";
}

void server_post_preset_two(String check_data, int main_data) {

  String nozzleNo = "";
  if (common_pump_Id_two < 10) nozzleNo = "0" + String(common_pump_Id_two);
  else nozzleNo = String(common_pump_Id_two);

  String Dev_ID = String(devicenum);
  String serverAddress = "http://192.168.0.100:9000/api/detail-sale/preset?depNo=" + Dev_ID + "&nozzleNo=" + nozzleNo;

  String carNo;
  if (strlen(Car_no_two) == 0) carNo = "-";
  else carNo = String(Car_no_two);

  String member_id;
  if (strlen(Member_id_two) == 0) member_id = "-";
  else member_id = String(Member_id_two);

  String member_name;
  if (strlen(Member_name_two) == 0) member_name = "-";
  else member_name = String(Member_name_two);

  String price_dd = "";
  String liter_data = "";
  if (check_data == "price") {
    price_dd = String(main_data);

  } else if (check_data == "liter") {
    float liter_dd = main_data + 0.0;
    liter_data = String(liter_dd);
  }

  String fuelType = Nozzle_name_two;
  String salePrice = String(common_dispenser_price_two);
  String vehicleType = vehicle_name_two;
  String cashType = cash_name_two;
  String device = "HMI";

  Serial.print("Server address is => ");
  Serial.println(serverAddress);

  Serial.print("price or liter => ");
  Serial.println(check_data);

  Serial.print("float vlaue is => ");
  Serial.println(liter_data);

  Serial.print("Price vlaue is => ");
  Serial.println(price_dd);

  Serial.print("sale price => ");
  Serial.println(salePrice);

  HTTPClient http;

  http.begin(serverAddress);

  // Set authorization header with the token
  http.addHeader("Authorization", "Bearer " + String(accessToken_two));

  // Create the multipart/form-data payload
  String formDataBoundary = "----WebKitFormBoundary";
  String formData = "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"nozzleNo\"\r\n\r\n";
  formData += nozzleNo + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"vehicleType\"\r\n\r\n";
  formData += vehicleType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"salePrice\"\r\n\r\n";
  formData += salePrice + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"carNo\"\r\n\r\n";
  formData += carNo + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"cashType\"\r\n\r\n";
  formData += cashType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  formData += "Content-Disposition: form-data; name=\"fuelType\"\r\n\r\n";
  formData += fuelType + "\r\n";
  formData += "--" + formDataBoundary + "\r\n";
  if (check_data == "price") {
    formData += "Content-Disposition: form-data; name=\"kyat\"\r\n\r\n";
    formData += price_dd + "\r\n";
    formData += "--" + formDataBoundary + "\r\n";
  } else if (check_data == "liter") {
    formData += "Content-Disposition: form-data; name=\"liter\"\r\n\r\n";
    formData += liter_data + "\r\n";
    formData += "--" + formDataBoundary + "\r\n";
  }
  formData += "Content-Disposition: form-data; name=\"device\"\r\n\r\n";
  formData += device + "\r\n";
  formData += "--" + formDataBoundary + "--\r\n";

  // Set content type for form data
  http.addHeader("Content-Type", "multipart/form-data; boundary=" + formDataBoundary);

  int httpResponseCode = http.POST(formData);

  // Check for a successful response
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Print the response payload
    String response = http.getString();
    Serial.println(response);

    DynamicJsonDocument doc(2048);

    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    rfid_condition = doc["con"];
    String Server_reply = doc["msg"];

    Serial.print("server condi  is => ");
    Serial.println(rfid_condition);

    Serial.print("server reply is => ");
    Serial.println(Server_reply);

    if (rfid_condition) {
      char my_chr[30];
      Server_reply.toCharArray(my_chr, Server_reply.length() + 1);

      Serial2.write(Show_server_reply, 6);
      Serial2.write(Show_server_data, 20);

      Serial2.write(Show_infor, 6);
      Serial2.write(my_chr, 30);

    } else {

      Serial2.write(Show_server_Error, 21);
      Serial2.write(Show_server_Error, 21);
    }

  } else {
    Serial.print("HTTP POST request failed, error: ");
    Serial.println(httpResponseCode);
  }

  // Close the connection
  http.end();
  Serial.println("http post is complete");
  accessToken_two = "";
}

void Fuel_type_name_two(int Fu_Id) {  // to get nozzle name and send server

  Nozzle_name_two = "";
  switch (Fu_Id) {
    case 1:
      Nozzle_name_two = String(Fuel_Type_1);
      Serial.print("fuel type 1 is => ");
      Serial.println(Nozzle_name_two);
      break;
    case 2:
      Nozzle_name_two = String(Fuel_Type_2);
      Serial.print("fuel type 2 is => ");
      Serial.println(Nozzle_name_two);
      break;
    case 3:
      Nozzle_name_two = String(Fuel_Type_3);
      Serial.print("fuel type 3 is => ");
      Serial.println(Nozzle_name_two);
      break;
    case 4:
      Nozzle_name_two = String(Fuel_Type_4);
      Serial.print("fuel type 4 is => ");
      Serial.println(Nozzle_name_two);
      break;
    case 5:
      Nozzle_name_two = String(Fuel_Type_5);
      break;
    case 6:
      Nozzle_name_two = String(Fuel_Type_6);
      break;
    case 7:
      Nozzle_name_two = String(Fuel_Type_7);
      break;
    case 8:
      Nozzle_name_two = String(Fuel_Type_8);
      break;
    default:
      Nozzle_name_two = "-";
      break;
  }
}

void vehicle_type_name_two(int type_id) {  // to get vehicle name and send server
  Serial.println("Vehicle type choose in Hmi two");
  vehicle_name_two = "";
  switch (type_id) {
    case 1:
      vehicle_name_two = "Cycle";
      break;
    case 2:
      vehicle_name_two = "Cycle ( 3 Wheel )";
      break;
    case 3:
      vehicle_name_two = "Car";
      break;
    case 4:
      vehicle_name_two = "Bus ( City )";
      break;
    case 5:
      vehicle_name_two = "Light Truck ( City )";
      break;
    case 6:
      vehicle_name_two = "Light Truck ( High way )";
      break;
    case 7:
      vehicle_name_two = "Heavy Truck ( City )";
      break;
    case 8:
      vehicle_name_two = "Heavy Truck ( High way )";
      break;
    case 9:
      vehicle_name_two = "Trailer ( City )";
      break;
    case 10:
      vehicle_name_two = "Trailer ( High way )";
      break;
    case 11:
      vehicle_name_two = "Htawlargyi";
      break;
    case 12:
      vehicle_name_two = "Tractor";
      break;
    case 13:
      vehicle_name_two = "Small Tractor";
      break;
    case 14:
      vehicle_name_two = "Heavy Machinery";
      break;
    case 15:
      vehicle_name_two = "Commercial Vehicle";
      break;
    case 16:
      vehicle_name_two = "Phone Tower";
      break;
    case 17:
      vehicle_name_two = "Industrial Zone";
      break;
    case 18:
      vehicle_name_two = "Generator Industry";
      break;
    case 19:
      vehicle_name_two = "Agriculture Machine";
      break;
    case 20:
      vehicle_name_two = "Generator ( Home Use )";
      break;
    case 21:
      vehicle_name_two = "Hospital";
      break;
    case 22:
      vehicle_name_two = "School";
      break;
    case 23:
      vehicle_name_two = "Super Market";
      break;
    case 24:
      vehicle_name_two = "Hotel";
      break;
    case 25:
      vehicle_name_two = "Housing";
      break;
    case 26:
      vehicle_name_two = "Boat";
      break;
    case 27:
      vehicle_name_two = "Pump Test";
      break;
    case 28:
      vehicle_name_two = "Office Use ( Bowser Car )";
      break;
    case 29:
      vehicle_name_two = "Station Use";
      break;
    default:
      vehicle_name_two = "-";
      break;
  }
}

void cash_type_name_two(int type_id) {  // to get cash name and send server
  cash_name_two = "";
  // Serial.println("cash name in Hmi two");
  switch (type_id) {
    case 1:
      cash_name_two = "Cash";
      break;
    case 2:
      cash_name_two = "KBZ Pay";
      break;
    case 3:
      cash_name_two = "Credit Card";
      break;
    case 4:
      cash_name_two = "Member Card";
      break;
    case 5:
      cash_name_two = "KBZ Banking";
      break;
    case 6:
      cash_name_two = "AYA Pay";
      break;
    case 7:
      cash_name_two = "AYA Banking";
      break;
    case 8:
      cash_name_two = "CB Banking";
      break;
    case 9:
      cash_name_two = "Other";
      break;
    default:
      cash_name_two = "-";
      break;
  }
}

void price_calculation_check_two(int price_data) {  // if price data input liter calculation
  Serial.println("price calculation check function in Hmi two");
  Preset_eep_total_price_two = price_data;  // for return when press reset and start

  switch (dispenser_ID_two) {
    case 1:
      Serial.print("dispendser one price is => ");
      Serial.println(dispenser_1_price);
      price_calculation_two(price_data, dispenser_1_price);
      break;

    case 2:
      price_calculation_two(price_data, dispenser_2_price);
      break;

    case 3:
      price_calculation_two(price_data, dispenser_3_price);
      break;

    case 4:
      price_calculation_two(price_data, dispenser_4_price);
      break;

    case 5:
      price_calculation_two(price_data, dispenser_5_price);
      break;

    case 6:
      price_calculation_two(price_data, dispenser_6_price);
      break;

    case 7:
      price_calculation_two(price_data, dispenser_7_price);
      break;

    case 8:
      price_calculation_two(price_data, dispenser_8_price);
      break;
  }
}

void price_calculation_two(int price_total_data, int price_data_dispenser) {
  common_dispenser_price_two = price_data_dispenser;

  if (price_data_dispenser > 0 && !Total_check_two) {
    Serial.println("price total check is false in Hmi two");
    int total_liter_two = price_total_data / price_data_dispenser;

    Total_check_two = false;

    Show_liter[7] = (total_liter_two >> 16) & 0xff;
    Show_liter[8] = (total_liter_two >> 8) & 0xff;
    Show_liter[9] = total_liter_two & 0xff;


    Serial2.write(Show_liter, 10);
    delay(10);
    Serial2.write(Show_liter, 10);
    delay(10);
  }

  if (price_data_dispenser > 0 && Total_check_two) {

    Serial.println("price total check is true in Hmi two");
    int total_liter_two = price_total_data / price_data_dispenser;

    Total_check_two = false;

    Show_liter[7] = (total_liter_two >> 16) & 0xff;
    Show_liter[8] = (total_liter_two >> 8) & 0xff;
    Show_liter[9] = total_liter_two & 0xff;

    Show_price[7] = (price_total_data >> 16) & 0xff;
    Show_price[8] = (price_total_data >> 8) & 0xff;
    Show_price[9] = price_total_data & 0xff;

    Serial2.write(Show_price, 10);
    delay(10);
    Serial2.write(Show_price, 10);
    delay(10);

    Serial2.write(Show_liter, 10);
    delay(10);
    Serial2.write(Show_liter, 10);
    delay(10);
  }
}

void liter_calculation_check_two(int liter_data) {  //if liter input each dispenser price calculation
  Serial.println("liter calculation check function in Hmi two");
  Preset_eep_total_liter_two = liter_data;

  switch (dispenser_ID_two) {
    case 1:
      liter_calculation_two(liter_data, dispenser_1_price);
      break;

    case 2:
      liter_calculation_two(liter_data, dispenser_2_price);
      break;

    case 3:
      liter_calculation_two(liter_data, dispenser_3_price);
      break;

    case 4:
      liter_calculation_two(liter_data, dispenser_4_price);
      break;

    case 5:
      liter_calculation_two(liter_data, dispenser_5_price);
      break;

    case 6:
      liter_calculation_two(liter_data, dispenser_6_price);
      break;

    case 7:
      liter_calculation_two(liter_data, dispenser_7_price);
      break;

    case 8:
      liter_calculation_two(liter_data, dispenser_8_price);
      break;
  }
}

void liter_calculation_two(int liter_total_data, int price_data) {  // to show total price if input liter

  common_dispenser_price_two = price_data;

  if (price_data > 0 && !Total_check_two) {
    Serial.println("liter total check is false in Hmi two");
    int total_price_two = liter_total_data * price_data;

    Total_check_two = false;

    Show_price[7] = (total_price_two >> 16) & 0xff;
    Show_price[8] = (total_price_two >> 8) & 0xff;
    Show_price[9] = total_price_two & 0xff;

    Serial2.write(Show_price, 10);
    delay(10);
    Serial2.write(Show_price, 10);
    delay(10);
  }

  if (price_data > 0 && Total_check_two) {
    Serial.println("liter total check is true in Hmi two");

    int total_price_two = liter_total_data * price_data;

    Total_check_two = false;

    Show_price[7] = (total_price_two >> 16) & 0xff;
    Show_price[8] = (total_price_two >> 8) & 0xff;
    Show_price[9] = total_price_two & 0xff;

    Show_liter[7] = (liter_total_data >> 16) & 0xff;
    Show_liter[8] = (liter_total_data >> 8) & 0xff;
    Show_liter[9] = liter_total_data & 0xff;

    Serial2.write(Show_liter, 10);
    delay(10);
    Serial2.write(Show_liter, 10);
    delay(10);

    Serial2.write(Show_price, 10);
    delay(10);
    Serial2.write(Show_price, 10);
    delay(10);
  }
}

void preset_rewrite_price_two() {

  Show_F1_price_two[7] = (F1_eep_price_value >> 16) & 0xff;
  Show_F1_price_two[8] = (F1_eep_price_value >> 8) & 0xff;
  Show_F1_price_two[9] = F1_eep_price_value & 0xff;

  Serial2.write(Show_F1_price_two, 10);
  delay(10);
  Serial2.write(Show_F1_price_two, 10);
  delay(10);

  Show_F2_price_two[7] = (F2_eep_price_value >> 16) & 0xff;
  Show_F2_price_two[8] = (F2_eep_price_value >> 8) & 0xff;
  Show_F2_price_two[9] = F2_eep_price_value & 0xff;

  Serial2.write(Show_F2_price_two, 10);
  delay(10);
  Serial2.write(Show_F2_price_two, 10);
  delay(10);

  Show_F3_price_two[7] = (F3_eep_price_value >> 16) & 0xff;
  Show_F3_price_two[8] = (F3_eep_price_value >> 8) & 0xff;
  Show_F3_price_two[9] = F3_eep_price_value & 0xff;

  Serial2.write(Show_F3_price_two, 10);
  delay(10);
  Serial2.write(Show_F3_price_two, 10);
  delay(10);

  Show_F4_price_two[7] = (F4_eep_price_value >> 16) & 0xff;
  Show_F4_price_two[8] = (F4_eep_price_value >> 8) & 0xff;
  Show_F4_price_two[9] = F4_eep_price_value & 0xff;

  Serial2.write(Show_F4_price_two, 10);
  delay(10);
  Serial2.write(Show_F4_price_two, 10);
  delay(10);

  Serial.println("Rewrite price fun complete in Hmi two....!");
}

void preset_rewrite_liter_two() {
  Serial.println("rewrite reset function liter in Hmi two");
  Show_F1_liter_two[7] = (F1_eep_liter_value >> 16) & 0xff;
  Show_F1_liter_two[8] = (F1_eep_liter_value >> 8) & 0xff;
  Show_F1_liter_two[9] = F1_eep_liter_value & 0xff;

  Serial2.write(Show_F1_liter_two, 10);
  Serial2.write(Show_F1_liter_two, 10);

  Show_F2_liter_two[7] = (F2_eep_liter_value >> 16) & 0xff;
  Show_F2_liter_two[8] = (F2_eep_liter_value >> 8) & 0xff;
  Show_F2_liter_two[9] = F2_eep_liter_value & 0xff;

  Serial2.write(Show_F2_liter_two, 10);
  delay(50);
  Serial2.write(Show_F2_liter_two, 10);
  delay(50);

  Show_F3_liter_two[7] = (F3_eep_liter_value >> 16) & 0xff;
  Show_F3_liter_two[8] = (F3_eep_liter_value >> 8) & 0xff;
  Show_F3_liter_two[9] = F3_eep_liter_value & 0xff;

  Serial2.write(Show_F3_liter_two, 10);
  delay(50);
  Serial2.write(Show_F3_liter_two, 10);
  delay(50);

  Show_F4_liter_two[7] = (F4_eep_liter_value >> 16) & 0xff;
  Show_F4_liter_two[8] = (F4_eep_liter_value >> 8) & 0xff;
  Show_F4_liter_two[9] = F4_eep_liter_value & 0xff;

  Serial2.write(Show_F4_liter_two, 10);
  delay(50);
  Serial2.write(Show_F4_liter_two, 10);
  delay(50);

  Serial.println("Rewrite liter fun complete in Hmi two....!");
}

void clear_setup_two() {  // to clear data in preset
  route_protect = true;
  combine_Liter_two = 0;
  combine_price_two = 0;
  Serial2.write(clean_infor, 8);
  Serial2.write(clean_infor, 8);
  Serial2.write(clean_infor_one, 8);
  Serial2.write(clean_infor_one, 8);
  Serial2.write(Show_default_liter, 10);
  Serial2.write(Show_default_liter, 10);
  Serial2.write(Show_default_price, 10);
  Serial2.write(Show_default_price, 10);
}

void Preset_setup_two() {  // to reset all default in preset page
  Total_check_two = false;
  over_all_li_price_check_two = false;
  liter_check_two = false;
  price_check_two = false;
  preset_check_two = false;
  rfid_card_check_two = false;

  Preset_total_eep_check_two = 0;
  Preset_eep_total_price_two = 0;
  Preset_eep_total_liter_two = 0;
  combine_Liter_two = 0;
  combine_price_two = 0;

  Serial.print("Hmi two vehicle value => ");
  Serial.println(vehicle_value_two);
  Serial.print("Hmi two cash value => ");
  Serial.println(cash_value_two);
  Serial.print("Hmi two car no => ");
  Serial.println(Car_no_two);
  Serial.print("Hmi two member id => ");
  Serial.println(Member_id_two);
  Serial.print("Hmi two member name => ");
  Serial.println(Member_name_two);

  Serial2.write(Show_default_liter, 10);
  Serial2.write(Show_default_liter, 10);

  Serial2.write(Show_default_price, 10);
  Serial2.write(Show_default_price, 10);

  Page_change[9] = 0X07;
  Serial2.write(Page_change, 10);
  Serial2.write(Page_change, 10);
}

void vehicle_setup_two() {  // to reset all default in vehicle page
  // Serial.println(" vehicle defaultin Hmi two ");

  route_protect = true;

  for (int j = 0; j < 20; j++) Member_id_two[j] = 0;
  for (int j = 0; j < 20; j++) Car_no_two[j] = 0;
  for (int j = 0; j < 20; j++) Member_name_two[j] = 0;
  for (int j = 0; j < 4; j++) vehicle_type_two[j] = 0;
  for (int j = 0; j < 4; j++) cash_type_two[j] = 0;

  vehicle_value_two = 1;
  cash_value_two = 1;

  Serial2.write(Show_car_no, 15);
  // delay(5);
  Serial2.write(Show_car_no, 15);
  // delay(5);

  Serial2.write(Show_member_id, 17);
  // delay(5);
  Serial2.write(Show_member_id, 17);
  // delay(5);

  Serial2.write(Show_member_name, 19);
  // delay(5);
  Serial2.write(Show_member_name, 19);
  // delay(5);
  Serial2.write(Default_cash, 8);
  // delay(5);
  Serial2.write(Default_cash, 8);
  // delay(5);

  Serial2.write(Default_vehicle, 8);
  Serial2.write(Default_vehicle, 8);

  Page_change[9] = 0X04;
  Serial2.write(Page_change, 10);
  Serial2.write(Page_change, 10);
}

void rfid_setup_two() {  // rfid auth
  Serial.println(" rfid setup in Hmi two");
  if (Hmi_mfrc522.PICC_IsNewCardPresent() && Hmi_mfrc522.PICC_ReadCardSerial()) {
    Serial.print("UID tag :");

    rfid_content_two = "";

    for (byte i = 0; i < Hmi_mfrc522.uid.size; i++) {
      Serial.print(Hmi_mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(Hmi_mfrc522.uid.uidByte[i], HEX);
      rfid_content_two.concat(String(Hmi_mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      rfid_content_two.concat(String(Hmi_mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    rfid_content_two.toUpperCase();

    tone(buzzer_pin_two, 5000);
    delay(50);
    noTone(buzzer_pin_two);

    Hmi_mfrc522.PICC_HaltA();
    Hmi_mfrc522.PCD_StopCrypto1();

    http_login_post_two(rfid_content_two);  // when get server api to open

    tone(buzzer_pin_two, 5000);
    delay(50);
    noTone(buzzer_pin_two);

    rfid_auth_check_two = true;
    rfid_check_two = false;
  }
}

void rfid_auth_two() {  // rfid auth one
  Serial.println("rfid auth function Hmi two");

  if (rfid_condition && !rfid_card_check_two) {
    Serial.println("rfid auth in rfid card check false");

    tone(buzzer_pin_two, 5000);
    delay(50);

    rfid_condition = false;
    rfid_check_two = false;
    rfid_card_check_two = false;

    Page_change[9] = 0X04;
    Serial2.write(Page_change, 10);
    vehicle_setup_two();

    Serial2.write(Page_change, 10);
    vehicle_setup_two();

    noTone(buzzer_pin_two);
    rfid_auth_check_two = false;

  } else if (rfid_condition && rfid_card_check_two) {  // when card no not access
    Serial.println("rfid auth in rfid card check true line");

    tone(buzzer_pin_two, 5000);
    delay(50);

    rfid_condition = false;
    rfid_check_two = false;
    rfid_card_check_two = false;

    Page_change[9] = 0X07;
    Serial2.write(Page_change, 10);
    Preset_setup_two();

    Serial2.write(Page_change, 10);
    Preset_setup_two();

    noTone(buzzer_pin_two);
    rfid_auth_check_two = false;

  } else if (!rfid_condition) {

    Page_change[9] = 0x05;
    Serial2.write(Page_change, 10);
    Serial2.write(Page_change, 10);
    Serial2.write(Page_change, 10);

    delay(50);
    tone(buzzer_pin_two, 5000);
    delay(50);
    noTone(buzzer_pin_two);
    delay(50);
    tone(buzzer_pin_two, 5000);
    delay(50);
    noTone(buzzer_pin_two);

    rfid_auth_check_two = false;
    rfid_condition = false;
  }
}

void Information_page_show_two() {  // to show information page

  Page_change[9] = 0x10;
  Serial2.write(Page_change, 10);
  Serial2.write(Page_change, 10);

  Serial.println("information page done");
}

void page_check_fun_two(int page_data) {
  Serial.println("page check fun in hmi two");

  if (page_data == 0 || page_data != nozzlenum) {
    page_data = nozzlenum;
  }

  switch (page_data) {
    case 1:
      Serial2.write(two_Page_change, 10);
      delay(2);
      break;
    case 2:
      Serial2.write(two_Page_change, 10);
      delay(2);
      break;
    case 3:
      Serial2.write(four_Page_change, 10);
      delay(2);
      break;
    case 4:
      Serial2.write(four_Page_change, 10);
      delay(2);
      break;
    case 5:
      Serial2.write(six_Page_change, 10);
      delay(2);
      break;
    case 6:
      Serial2.write(six_Page_change, 10);
      delay(2);
      break;
    case 7:
      Serial2.write(eight_Page_change, 10);
      delay(2);
      break;
    case 8:
      Serial2.write(eight_Page_change, 10);
      delay(2);
      break;
  }
}

void PIN_Auth_two() {  // pin code auth two
  if (PIN_CODE_two == eep_PIN_CODE) {
    Page_change[9] = 0x0D;

    Serial2.write(Page_change, 10);
    Serial2.write(Page_change, 10);

    Serial2.write(Default_pin_code, 10);
  } else {

    Page_change[9] = 0x17;

    Serial2.write(Page_change, 10);
    Serial2.write(Page_change, 10);

    Serial2.write(Default_pin_code, 10);
  }
}

void buzzer_song_two() {
  tone(buzzer_pin_two, 5000);
  delay(50);
  noTone(buzzer_pin_two);
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$----common function----$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


void wifi_init() {

  Serial.println("wifi init function");
  wifi_try = 0;

  WiFi.begin("POS_Server", "asdffdsa");  ///akkkkk
  // WiFi.begin(SSID_buffer, PASS_buffer);
  while (WiFi.status() != WL_CONNECTED && wifi_try++ != 15) {

    Serial2.write(Hide_wifi_icon, 8);
    delay(250);
    Serial2.write(Show_wifi_icon, 8);
    delay(250);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Wifi is connected..!");

    Serial2.write(Show_wifi_icon, 8);
    Serial2.write(Show_wifi_icon, 8);

    Icon_fun();  // icon fun

    Serial.println(WiFi.localIP());
  }
}

void wifi_save_connect() {
  Serial.println("wifi save connect function");
  wifi_try = 0;
  Serial.println(SSID_buffer);
  Serial.println(PASS_buffer);
  Serial.print("Wifi is connecting to ");
  Serial.println(SSID_buffer);

  WiFi.begin(SSID_buffer, PASS_buffer);
  while (WiFi.status() != WL_CONNECTED && wifi_try++ != 15) {
    Serial2.write(Hide_wifi_icon, 8);
    delay(250);
    Serial2.write(Show_wifi_icon, 8);
    delay(250);
  }

  common_page_check_fun(nozzlenum);  // to check page for screen brightness

  isconnected = false;

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Wifi is connected from ");
    Serial.println(SSID_buffer);
    Serial.println(WiFi.localIP());

    writeStringToEEPROM(0, SSID_buffer);  // for eeprom write
    writeStringToEEPROM(40, PASS_buffer);

    Serial2.write(Show_wifi_icon, 8);
    Serial2.write(Show_wifi_icon, 8);

    Icon_fun();

    isconnected = false;
  }
}

void preset_eeprom_value() {
  F1_eep_liter_value = eeprom_read_data(150);
  F1_eep_price_value = eeprom_read_data(155);

  F2_eep_liter_value = eeprom_read_data(160);
  F2_eep_price_value = eeprom_read_data(165);

  F3_eep_liter_value = eeprom_read_data(170);
  F3_eep_price_value = eeprom_read_data(175);

  F4_eep_liter_value = eeprom_read_data(180);
  F4_eep_price_value = eeprom_read_data(185);

  Serial.println(F1_eep_liter_value);
  Serial.println(F1_eep_price_value);
  Serial.println(F2_eep_liter_value);
  Serial.println(F2_eep_price_value);
  Serial.println(F3_eep_liter_value);
  Serial.println(F3_eep_price_value);
  Serial.println(F4_eep_liter_value);
  Serial.println(F4_eep_price_value);
}

void eeprom_init() {  // to read wifi data and pump id from eeprom

  // readStringFromEEPROM(0, SSID_buffer, sizeof(SSID_buffer));
  // readStringFromEEPROM(40, PASS_buffer, sizeof(SSID_buffer));
  char eep_ssid;
  char eep_pass;

  for (int i = 0; i < 40; i++) {
    eep_ssid = EEPROM.read(i);
    if (eep_ssid == '\0') break;
    SSID_buffer[i] = eep_ssid;
  }

  for (int j = 40; j < 80; j++) {
    eep_pass = EEPROM.read(j);
    if (eep_pass == '\0') break;
    PASS_buffer[j - 40] = eep_pass;
  }

  casher_lock_check = EEPROM.read(90);
  eep_PIN_CODE = eeprom_read_data(92);

  pumpid1 = EEPROM.read(101);
  pumpid2 = EEPROM.read(102);
  pumpid3 = EEPROM.read(103);
  pumpid4 = EEPROM.read(104);
  pumpid5 = EEPROM.read(105);
  pumpid6 = EEPROM.read(106);
  pumpid7 = EEPROM.read(107);
  pumpid8 = EEPROM.read(108);

  devicenum = EEPROM.read(109);
  nozzlenum = EEPROM.read(110);

  dispenser_1_price = eeprom_read_data(114);
  dispenser_2_price = eeprom_read_data(118);
  dispenser_3_price = eeprom_read_data(122);
  dispenser_4_price = eeprom_read_data(81);
  dispenser_5_price = eeprom_read_data(85);
  dispenser_6_price = eeprom_read_data(132);
  dispenser_7_price = eeprom_read_data(138);
  dispenser_8_price = eeprom_read_data(144);

  Serial.println("dispenser price is =>");
  Serial.println(dispenser_1_price);
  Serial.println(dispenser_2_price);
  Serial.println(dispenser_3_price);
  Serial.println(dispenser_4_price);
  Serial.println(dispenser_5_price);
  Serial.println(dispenser_6_price);

  F1_eep_liter_value = eeprom_read_data(150);
  F1_eep_price_value = eeprom_read_data(155);

  F2_eep_liter_value = eeprom_read_data(160);
  F2_eep_price_value = eeprom_read_data(165);

  F3_eep_liter_value = eeprom_read_data(170);
  F3_eep_price_value = eeprom_read_data(175);

  F4_eep_liter_value = eeprom_read_data(180);
  F4_eep_price_value = eeprom_read_data(185);

  Fuel_Type_1 = read_Fuel_type(200, 220);
  Fuel_Type_2 = read_Fuel_type(230, 260);
  Fuel_Type_3 = read_Fuel_type(260, 290);
  Fuel_Type_4 = read_Fuel_type(290, 320);
  Fuel_Type_5 = read_Fuel_type(330, 360);
  Fuel_Type_6 = read_Fuel_type(360, 390);
  Fuel_Type_7 = read_Fuel_type(390, 420);
  Fuel_Type_8 = read_Fuel_type(420, 450);

  // Serial.print("EEPROM read fuel types=> ");
  // Serial.println(Fuel_Type_1);
  // Serial.println(Fuel_Type_2);
  // Serial.println(Fuel_Type_3);
  // Serial.println(Fuel_Type_4);
  // Serial.println(Fuel_Type_5);
  // Serial.println(Fuel_Type_6);
  // Serial.println(Fuel_Type_7);
  // Serial.println(Fuel_Type_8);

  char* cut_Fuel_Type_1 = cut_fuel_name(Fuel_Type_1);
  char* cut_Fuel_Type_2 = cut_fuel_name(Fuel_Type_2);
  char* cut_Fuel_Type_3 = cut_fuel_name(Fuel_Type_3);
  char* cut_Fuel_Type_4 = cut_fuel_name(Fuel_Type_4);
  char* cut_Fuel_Type_5 = cut_fuel_name(Fuel_Type_5);
  char* cut_Fuel_Type_6 = cut_fuel_name(Fuel_Type_6);
  char* cut_Fuel_Type_7 = cut_fuel_name(Fuel_Type_7);
  char* cut_Fuel_Type_8 = cut_fuel_name(Fuel_Type_8);

  Serial.print("EEPROM cut fuel types=> ");
  Serial.println(cut_Fuel_Type_1);
  Serial.println(cut_Fuel_Type_2);
  Serial.println(cut_Fuel_Type_3);
  Serial.println(cut_Fuel_Type_4);
  Serial.println(cut_Fuel_Type_5);
  Serial.println(cut_Fuel_Type_6);
  Serial.println(cut_Fuel_Type_7);
  Serial.println(cut_Fuel_Type_8);

  Show_Fueltype(cut_Fuel_Type_1, 1);
  Show_Fueltype(cut_Fuel_Type_2, 2);
  Show_Fueltype(cut_Fuel_Type_3, 3);
  Show_Fueltype(cut_Fuel_Type_4, 4);
  Show_Fueltype(cut_Fuel_Type_5, 5);
  Show_Fueltype(cut_Fuel_Type_6, 6);
  Show_Fueltype(cut_Fuel_Type_7, 7);
  Show_Fueltype(cut_Fuel_Type_8, 8);

  Serial.print("Pin Code is => ");
  Serial.println(eep_PIN_CODE);

  Serial.print("Casher lock Id 1 is => ");
  Serial.println(casher_lock_check);

  Serial.print("Pump Id 1 is => ");
  Serial.println(pumpid1);
  Serial.print("Pump Id 2 is => ");
  Serial.println(pumpid2);
  Serial.print("Pump Id 3 is => ");
  Serial.println(pumpid3);
  Serial.print("Pump Id 4 is => ");
  Serial.println(pumpid4);
  Serial.print("Pump Id 5 is => ");
  Serial.println(pumpid5);
  Serial.print("Pump Id 6 is => ");
  Serial.println(pumpid6);
  Serial.print("Pump Id 7 is => ");
  Serial.println(pumpid7);
  Serial.print("Pump Id 8 is => ");
  Serial.println(pumpid8);

  Serial.print("device Id is => ");
  Serial.println(devicenum);
  Serial.print("nozzlenum is => ");
  Serial.println(nozzlenum);


  Serial.print("EEPROM read ssid is => ");
  Serial.println(SSID_buffer);
  Serial.print("EEPROM read pass is => ");
  Serial.println(PASS_buffer);

  Serial.println("EEprom init is Done ......!");
}

// void initial_setup() {  // initial setup for page swith and show device name

//   if (pumpid1 == 0 && pumpid2 == 0) {
//     Serial.println("One nozzle page is called.");
//     page_check = 1;
//     Serial2.write(two_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 == 0 && pumpid3 == 0) {
//     Serial.println("One nozzle page is called.");
//     page_check = 1;
//     Serial2.write(two_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 == 0 && pumpid4 == 0) {
//     Serial.println("Two nozzle page is called.");
//     page_check = 2;
//     Serial2.write(two_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 == 0 && pumpid5 == 0) {
//     Serial.println("Three nozzle page is called.");
//     page_check = 3;
//     Serial2.write(four_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 > 0 && pumpid5 == 0 && pumpid6 == 0) {
//     Serial.println("Four nozzle page is called.");
//     page_check = 4;
//     Serial2.write(four_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 > 0 && pumpid5 > 0 && pumpid6 == 0 && pumpid7 == 0) {
//     Serial.println("Five nozzle page is called.");
//     page_check = 5;
//     Serial2.write(six_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 > 0 && pumpid5 > 0 && pumpid6 > 0 && pumpid7 == 0 && pumpid8 == 0) {
//     Serial.println("Six nozzle page is called.");
//     page_check = 6;
//     Serial2.write(six_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 > 0 && pumpid5 > 0 && pumpid6 > 0 && pumpid7 > 0 && pumpid8 == 0) {
//     Serial.println("Seven nozzle page is called.");
//     page_check = 7;
//     Serial2.write(eight_Page_change, 10);

//   } else if (pumpid1 > 0 && pumpid2 > 0 && pumpid3 > 0 && pumpid4 > 0 && pumpid5 > 0 && pumpid6 > 0 && pumpid7 > 0 && pumpid8 > 0) {  // eight page change
//     Serial.println("eight nozzle page is called.");
//     page_check = 8;
//     Serial2.write(eight_Page_change, 10);
//   }
// }

void mqtt_topic_change() {

  if (devicenum == 1) {
    Show_device_id[6] = highByte(1);
    Show_device_id[7] = lowByte(1);
    pump_permit[21] = '1';
    final_data[20] = '1';
    pin_Code[19] = '1';
    device_Id_topic[38] = '1';
    FuelType_topic[38] = '1';

  } else if (devicenum == 2) {
    Show_device_id[6] = highByte(2);
    Show_device_id[7] = lowByte(2);
    pump_permit[21] = '2';
    final_data[20] = '2';
    pin_Code[19] = '2';
    device_Id_topic[38] = '2';
    FuelType_topic[38] = '2';

  } else if (devicenum == 3) {
    Show_device_id[6] = highByte(3);
    Show_device_id[7] = lowByte(3);
    pump_permit[21] = '3';
    final_data[20] = '3';
    pin_Code[19] = '3';
    device_Id_topic[38] = '3';
    FuelType_topic[38] = '3';

  } else if (devicenum == 4) {
    Show_device_id[6] = highByte(4);
    Show_device_id[7] = lowByte(4);
    pump_permit[21] = '4';
    final_data[20] = '4';
    pin_Code[19] = '4';
    device_Id_topic[38] = '4';
    FuelType_topic[38] = '4';

  } else if (devicenum == 5) {
    Show_device_id[6] = highByte(5);
    Show_device_id[7] = lowByte(5);
    pump_permit[21] = '5';
    final_data[20] = '5';
    pin_Code[19] = '5';
    device_Id_topic[38] = '5';
    FuelType_topic[38] = '5';

  } else if (devicenum == 6) {
    Show_device_id[6] = highByte(6);
    Show_device_id[7] = lowByte(6);
    pump_permit[21] = '6';
    final_data[20] = '6';
    pin_Code[19] = '6';
    device_Id_topic[38] = '6';
    FuelType_topic[38] = '6';

  } else if (devicenum == 7) {
    Show_device_id[6] = highByte(7);
    Show_device_id[7] = lowByte(7);
    pump_permit[21] = '7';
    final_data[20] = '7';
    pin_Code[19] = '7';
    device_Id_topic[38] = '7';
    FuelType_topic[38] = '7';

  } else if (devicenum == 8) {
    Show_device_id[6] = highByte(8);
    Show_device_id[7] = lowByte(8);
    pump_permit[21] = '8';
    final_data[20] = '8';
    pin_Code[19] = '8';
    device_Id_topic[38] = '8';
    FuelType_topic[38] = '8';
  }

  delay(5);

  Serial2.write(Show_device_id, 8);
}

void show_nozzle_no_fun(int noz_data, int noz_num) {

  if (noz_data == 0) {
    Show_Nozzle_NO[6] = 0x00;
    Show_Nozzle_NO[7] = 0x00;

  } else {
    Show_Nozzle_NO[6] = highByte(noz_data);
    Show_Nozzle_NO[7] = lowByte(noz_data);
  }

  if (noz_num == 1) Show_Nozzle_NO[5] = 0x11;
  else if (noz_num == 2) Show_Nozzle_NO[5] = 0x12;
  else if (noz_num == 3) Show_Nozzle_NO[5] = 0x13;
  else if (noz_num == 4) Show_Nozzle_NO[5] = 0x14;
  else if (noz_num == 5) Show_Nozzle_NO[5] = 0x15;
  else if (noz_num == 6) Show_Nozzle_NO[5] = 0x16;
  else if (noz_num == 7) Show_Nozzle_NO[5] = 0x17;
  else if (noz_num == 8) Show_Nozzle_NO[5] = 0x18;

  Serial2.write(Show_Nozzle_NO, 8);
}

void reconnect() {
  while (!client.connected()) {

    Serial.println("MQtt is connecting ");
    String clientId = "Rfid_client-";
    clientId += String(random(0xffff), HEX);
    // Serial.println(clientId);
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Mqtt connected");
      cloud_icon_check = true;

      client.subscribe("Connect");                    // subscribe the topics here
      client.publish("connect", "Device1Connected");  // subscribe the topics here
      client.subscribe("detpos/#");

    } else {
      Serial.print("mqtt connection failed, rc=");
      Serial.print(client.state());
      Serial.print(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  incommingMessage = "";

  for (int i = 0; i < length; i++) {
    incommingMessage += (char)payload[i];
  }

  Serial.println(topic);
  Serial.println(incommingMessage);

  if (String(topic) == String("detpos/device/req")) {
    Serial.println("cancle data from dispenser");
    incommingMessage.toCharArray(finaldataArry, incommingMessage.length() + 1);

    charArray[0] = finaldataArry[0];
    charArray[1] = finaldataArry[1];

    pumpmqttnumchange();
    show_dispenser_icon_fun();

    if (!brightness_check) screen_brightness_in();
  }

  if (String(topic) == String(pump_permit)) {  // pump permit

    incommingMessage.toCharArray(permitArry, incommingMessage.length() + 1);
    Serial.print("Permit number is => ");
    Serial.println(permitArry);

    charArray[0] = permitArry[0];
    charArray[1] = permitArry[1];

    pumpmqttnumchange();
    hide_dispenser_icon_fun();

    if (!brightness_check) screen_brightness_in();
  }

  if (String(topic) == String(final_data)) {

    Serial.println("Final data response");
    incommingMessage.toCharArray(finaldataArry, incommingMessage.length() + 1);
    Serial.print("Final Data is => ");
    Serial.println(finaldataArry);

    charArray[0] = finaldataArry[0];
    charArray[1] = finaldataArry[1];

    pumpmqttnumchange();
    show_dispenser_icon_fun();

    if (!brightness_check) screen_brightness_in();
  }


  if (String(topic) == String(pin_Code)) {  // pin code topic
    Serial.println("pin code topic");

    incommingMessage.toCharArray(pinCodeArry, incommingMessage.length() + 1);
    char PIN_Arry[6];
    for (int j = 0; j < 6; j++) PIN_Arry[j] = 0;

    for (int i = 0; i < 6; i++) {
      PIN_Arry[i] = pinCodeArry[i];
    }

    String pin_str = String(PIN_Arry);
    int pin_Int = pin_str.toInt();
    Serial.print("Pin code form mqtt is => ");
    Serial.println(pin_Int);

    eeprom_writeInt(92, pin_Int);
    Serial.println("Done");

    eep_PIN_CODE = eeprom_read_data(92);
    Serial.print("pin code return is => ");
    Serial.println(eep_PIN_CODE);
  }

  if (String(topic) == String(device_Id_topic)) {
    DynamicJsonDocument doc(4096);  // Adjust the size based on your JSON data size
    DeserializationError error = deserializeJson(doc, incommingMessage);

    if (error) {
      Serial.print(F("JSON parsing failed: "));
      Serial.println(error.c_str());
      return;
    }

    int pumpid_1 = 0;
    int pumpid_2 = 0;
    int pumpid_3 = 0;
    int pumpid_4 = 0;
    int pumpid_5 = 0;
    int pumpid_6 = 0;
    int pumpid_7 = 0;
    int pumpid_8 = 0;

    devicenum = doc["devicenum"].as<const int>();
    nozzlenum = doc["nozzlenum"].as<const int>();
    pumpid_1 = doc["pumpid1"].as<const int>();
    pumpid_2 = doc["pumpid2"].as<const int>();
    pumpid_3 = doc["pumpid3"].as<const int>();
    pumpid_4 = doc["pumpid4"].as<const int>();
    pumpid_5 = doc["pumpid5"].as<const int>();
    pumpid_6 = doc["pumpid6"].as<const int>();
    pumpid_7 = doc["pumpid7"].as<const int>();
    pumpid_8 = doc["pumpid8"].as<const int>();

    Serial.println(devicenum);
    Serial.println(nozzlenum);
    Serial.println(pumpid_1);
    Serial.println(pumpid_2);
    Serial.println(pumpid_3);
    Serial.println(pumpid_4);
    Serial.println(pumpid_5);
    Serial.println(pumpid_6);
    Serial.println(pumpid_7);
    Serial.println(pumpid_8);

    EEPROM.write(101, pumpid_1);
    EEPROM.commit();
    EEPROM.write(102, pumpid_2);
    EEPROM.commit();
    EEPROM.write(103, pumpid_3);
    EEPROM.commit();
    EEPROM.write(104, pumpid_4);
    EEPROM.commit();
    EEPROM.write(105, pumpid_5);
    EEPROM.commit();
    EEPROM.write(106, pumpid_6);
    EEPROM.commit();
    EEPROM.write(107, pumpid_7);
    EEPROM.commit();
    EEPROM.write(108, pumpid_8);
    EEPROM.commit();
    EEPROM.write(109, devicenum);  // device id
    EEPROM.commit();
    EEPROM.write(110, nozzlenum);  // nozzle count
    EEPROM.commit();
    Serial.println("yep all save");

    pumpid1 = EEPROM.read(101);
    pumpid2 = EEPROM.read(102);
    pumpid3 = EEPROM.read(103);
    pumpid4 = EEPROM.read(104);
    pumpid5 = EEPROM.read(105);
    pumpid6 = EEPROM.read(106);
    pumpid7 = EEPROM.read(107);
    pumpid8 = EEPROM.read(108);

    devicenum = EEPROM.read(109);
    nozzlenum = EEPROM.read(110);

    Serial.print("Pump Id 1 is => ");
    Serial.println(pumpid1);
    Serial.print("Pump Id 2 is => ");
    Serial.println(pumpid2);
    Serial.print("Pump Id 3 is => ");
    Serial.println(pumpid3);
    Serial.print("Pump Id 4 is => ");
    Serial.println(pumpid4);
    Serial.print("Pump Id 5 is => ");
    Serial.println(pumpid5);
    Serial.print("Pump Id 6 is => ");
    Serial.println(pumpid6);
    Serial.print("Pump Id 7 is => ");
    Serial.println(pumpid7);
    Serial.print("Pump Id 8 is => ");
    Serial.println(pumpid8);

    Serial.print("device id is => ");
    Serial.println(devicenum);
    Serial.print("nozz no id is => ");
    Serial.println(nozzlenum);

    // initial_setup(); //ak ch

    Show_device_id[6] = highByte(devicenum);
    Show_device_id[7] = lowByte(devicenum);
    Serial2.write(Show_device_id, 8);
    delay(2);
    Serial2.write(Show_device_id, 8);

    show_nozzle_no_fun(pumpid1, 1);
    show_nozzle_no_fun(pumpid2, 2);
    show_nozzle_no_fun(pumpid3, 3);
    show_nozzle_no_fun(pumpid4, 4);
    show_nozzle_no_fun(pumpid5, 5);
    show_nozzle_no_fun(pumpid6, 6);
    show_nozzle_no_fun(pumpid7, 7);
    show_nozzle_no_fun(pumpid8, 8);

    if (!brightness_check) screen_brightness_in();
  }

  if (String(topic) == String(FuelType_topic)) {
    Serial.println("this is fuel type change");

    DynamicJsonDocument doc(1024);  // Adjust the size based on your JSON data size
    DeserializationError error = deserializeJson(doc, incommingMessage);

    if (error) {
      Serial.print(F("JSON parsing failed: "));
      Serial.println(error.c_str());
      return;
    }

    String fuelType_1 = "";
    String fuelType_2 = "";
    String fuelType_3 = "";
    String fuelType_4 = "";
    String fuelType_5 = "";
    String fuelType_6 = "";
    String fuelType_7 = "";
    String fuelType_8 = "";

    fuelType_1 = doc["fuelid1"].as<const char*>();
    fuelType_2 = doc["fuelid2"].as<const char*>();
    fuelType_3 = doc["fuelid3"].as<const char*>();
    fuelType_4 = doc["fuelid4"].as<const char*>();
    fuelType_5 = doc["fuelid5"].as<const char*>();
    fuelType_6 = doc["fuelid6"].as<const char*>();
    fuelType_7 = doc["fuelid7"].as<const char*>();
    fuelType_8 = doc["fuelid8"].as<const char*>();

    Serial.println("mqtt fuel type");
    Serial.println(fuelType_1);
    Serial.println(fuelType_2);
    Serial.println(fuelType_3);
    Serial.println(fuelType_4);
    Serial.println(fuelType_5);
    Serial.println(fuelType_6);
    Serial.println(fuelType_7);
    Serial.println(fuelType_8);

    writeStringToEEPROM(200, fuelType_1);
    writeStringToEEPROM(230, fuelType_2);
    writeStringToEEPROM(260, fuelType_3);
    writeStringToEEPROM(290, fuelType_4);
    writeStringToEEPROM(330, fuelType_5);
    writeStringToEEPROM(360, fuelType_6);
    writeStringToEEPROM(390, fuelType_7);
    writeStringToEEPROM(420, fuelType_8);

    Serial.println("yep all save");

    Fuel_Type_1 = read_Fuel_type(200, 220);
    Fuel_Type_2 = read_Fuel_type(230, 260);
    Fuel_Type_3 = read_Fuel_type(260, 290);
    Fuel_Type_4 = read_Fuel_type(290, 320);
    Fuel_Type_5 = read_Fuel_type(330, 360);
    Fuel_Type_6 = read_Fuel_type(360, 390);
    Fuel_Type_7 = read_Fuel_type(390, 420);
    Fuel_Type_8 = read_Fuel_type(420, 450);

    Serial.println("from mqtt fuel type");
    Serial.println(Fuel_Type_1);
    Serial.println(Fuel_Type_2);
    Serial.println(Fuel_Type_3);
    Serial.println(Fuel_Type_4);
    Serial.println(Fuel_Type_5);
    Serial.println(Fuel_Type_6);
    Serial.println(Fuel_Type_7);
    Serial.println(Fuel_Type_8);

    show_fuel_type_name();
    mqtt_topic_change();

    if (!brightness_check) screen_brightness_in();
  }

  if (String(topic) == String(pricechange)) {
    incommingMessage.toCharArray(pricechangeary, incommingMessage.length() + 1);

    Serial.print("price change is => ");
    Serial.println(pricechangeary);

    charArray[0] = pricechangeary[0];
    charArray[1] = pricechangeary[1];

    pumpmqttnumchange();

    price_show_hmi();

    if (!brightness_check) screen_brightness_in();
  }
}

void pumpmqttnumchange() {
  Serial.println("pumpmqttnumchange function");

  if (charArray[0] == 0x30 && charArray[1] == 0x31) {
    pumpmqttnum = 1;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x32) {
    pumpmqttnum = 2;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x33) {
    pumpmqttnum = 3;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x34) {
    pumpmqttnum = 4;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x35) {
    pumpmqttnum = 5;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x36) {
    pumpmqttnum = 6;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x37) {
    pumpmqttnum = 7;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x38) {
    pumpmqttnum = 8;
  } else if (charArray[0] == 0x30 && charArray[1] == 0x39) {
    pumpmqttnum = 9;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x30) {
    pumpmqttnum = 10;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x31) {
    pumpmqttnum = 11;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x32) {
    pumpmqttnum = 12;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x33) {
    pumpmqttnum = 13;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x34) {
    pumpmqttnum = 14;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x35) {
    pumpmqttnum = 15;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x36) {
    pumpmqttnum = 16;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x37) {
    pumpmqttnum = 17;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x38) {
    pumpmqttnum = 18;
  } else if (charArray[0] == 0x31 && charArray[1] == 0x39) {
    pumpmqttnum = 19;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x30) {
    pumpmqttnum = 20;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x31) {
    pumpmqttnum = 21;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x32) {
    pumpmqttnum = 22;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x33) {
    pumpmqttnum = 23;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x34) {
    pumpmqttnum = 24;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x35) {
    pumpmqttnum = 25;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x36) {
    pumpmqttnum = 26;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x37) {
    pumpmqttnum = 27;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x38) {
    pumpmqttnum = 28;
  } else if (charArray[0] == 0x32 && charArray[1] == 0x39) {
    pumpmqttnum = 29;
  } else if (charArray[0] == 0x33 && charArray[1] == 0x30) {
    pumpmqttnum = 30;
  } else if (charArray[0] == 0x33 && charArray[1] == 0x31) {
    pumpmqttnum = 31;
  } else if (charArray[0] == 0x33 && charArray[1] == 0x32) {
    pumpmqttnum = 32;
  }
}

void price_show_hmi() {  // to show price input from server
  // Serial.println("price show hmi function");

  if (pumpid1 == pumpmqttnum) {  // Pump Id 1
    for (int j = 0; j < 4; j++) priceArray[j] = 0;

    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }

    String uniquePrice = String(priceArray);
    dispenser_1_price = uniquePrice.toInt();

    eeprom_writeInt(114, dispenser_1_price);

    Serial.print("pumpid 1 price is => ");
    Serial.println(dispenser_1_price);

    int Pump_Id = 1;

    ShowDisplay(Pump_Id, dispenser_1_price);

  } else if (pumpid2 == pumpmqttnum) {  // Pump Id 2
    for (int j = 0; j < 4; j++) priceArray[j] = 0;
    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 2 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_2_price = uniquePrice.toInt();

    eeprom_writeInt(118, dispenser_2_price);

    int Pump_Id = 2;

    ShowDisplay(Pump_Id, dispenser_2_price);

  } else if (pumpid3 == pumpmqttnum) {  // pump Id 3

    for (int j = 0; j < 4; j++) priceArray[j] = 0;
    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 3 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_3_price = uniquePrice.toInt();

    eeprom_writeInt(122, dispenser_3_price);

    int Pump_Id = 3;

    ShowDisplay(Pump_Id, dispenser_3_price);

  } else if (pumpid4 == pumpmqttnum) {  // pump Id 4
    for (int j = 0; j < 4; j++) priceArray[j] = 0;

    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 4 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_4_price = uniquePrice.toInt();

    eeprom_writeInt(81, dispenser_4_price);

    int Pump_Id = 4;

    ShowDisplay(Pump_Id, dispenser_4_price);

  } else if (pumpid5 == pumpmqttnum) {  // pump Id 5
    for (int j = 0; j < 4; j++) priceArray[j] = 0;

    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 5 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_5_price = uniquePrice.toInt();
    eeprom_writeInt(85, dispenser_5_price);
    int Pump_Id = 5;

    ShowDisplay(Pump_Id, dispenser_5_price);

  } else if (pumpid6 == pumpmqttnum) {  // pump Id 6
    for (int j = 0; j < 4; j++) priceArray[j] = 0;

    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 6 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_6_price = uniquePrice.toInt();
    eeprom_writeInt(132, dispenser_6_price);
    int Pump_Id = 6;

    ShowDisplay(Pump_Id, dispenser_6_price);

  } else if (pumpid7 == pumpmqttnum) {  // pump id 7
    for (int j = 0; j < 4; j++) priceArray[j] = 0;

    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 7 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_7_price = uniquePrice.toInt();
    eeprom_writeInt(138, dispenser_7_price);
    int Pump_Id = 7;

    ShowDisplay(Pump_Id, dispenser_7_price);

  } else if (pumpid8 == pumpmqttnum) {  // pump Id 8

    for (int j = 0; j < 4; j++) priceArray[j] = 0;
    for (int i = 0; i < 4; i++) {
      priceArray[i] = pricechangeary[i + 2];
    }
    Serial.print("pumpid 8 price is => ");
    Serial.println(priceArray);

    String uniquePrice = String(priceArray);
    dispenser_8_price = uniquePrice.toInt();
    eeprom_writeInt(144, dispenser_8_price);
    int Pump_Id = 8;

    ShowDisplay(Pump_Id, dispenser_8_price);
  }
}

void ShowDisplay(int dispenser, int decimal_price) {  // to show price in
  // Serial.println("show display function");

  Show_dispenser_price[6] = highByte(decimal_price);
  Show_dispenser_price[7] = lowByte(decimal_price);

  if (dispenser == 1) Show_dispenser_price[5] = 0x11;
  else if (dispenser == 2) Show_dispenser_price[5] = 0x12;
  else if (dispenser == 3) Show_dispenser_price[5] = 0x13;
  else if (dispenser == 4) Show_dispenser_price[5] = 0x14;
  else if (dispenser == 5) Show_dispenser_price[5] = 0x15;
  else if (dispenser == 6) Show_dispenser_price[5] = 0x16;
  else if (dispenser == 7) Show_dispenser_price[5] = 0x17;
  else if (dispenser == 8) Show_dispenser_price[5] = 0x18;

  Serial2.write(Show_dispenser_price, 8);
  Serial2.write(Show_dispenser_price, 8);
}

void show_dispenser_icon_fun() {  // dispenser icon main function
  if (pumpid1 == pumpmqttnum) Show_dispenser_icon[5] = 0x11;
  else if (pumpid2 == pumpmqttnum) Show_dispenser_icon[5] = 0x12;
  else if (pumpid3 == pumpmqttnum) Show_dispenser_icon[5] = 0x13;
  else if (pumpid4 == pumpmqttnum) Show_dispenser_icon[5] = 0x14;
  else if (pumpid5 == pumpmqttnum) Show_dispenser_icon[5] = 0x15;
  else if (pumpid6 == pumpmqttnum) Show_dispenser_icon[5] = 0x16;
  else if (pumpid7 == pumpmqttnum) Show_dispenser_icon[5] = 0x17;
  else if (pumpid8 == pumpmqttnum) Show_dispenser_icon[5] = 0x18;

  Serial2.write(Show_dispenser_icon, 8);
  Serial2.write(Show_dispenser_icon, 8);
}

void hide_dispenser_icon_fun() {  // dispenser icon main function

  if (pumpid1 == pumpmqttnum) Hide_dispenser_icon[5] = 0x11;
  else if (pumpid2 == pumpmqttnum) Hide_dispenser_icon[5] = 0x12;
  else if (pumpid3 == pumpmqttnum) Hide_dispenser_icon[5] = 0x13;
  else if (pumpid4 == pumpmqttnum) Hide_dispenser_icon[5] = 0x14;
  else if (pumpid5 == pumpmqttnum) Hide_dispenser_icon[5] = 0x15;
  else if (pumpid6 == pumpmqttnum) Hide_dispenser_icon[5] = 0x16;
  else if (pumpid7 == pumpmqttnum) Hide_dispenser_icon[5] = 0x17;
  else if (pumpid8 == pumpmqttnum) Hide_dispenser_icon[5] = 0x18;

  Serial2.write(Hide_dispenser_icon, 8);
  Serial2.write(Hide_dispenser_icon, 8);
}

void show_dispenser_icon_initial(int icon_int) {  // dispenser icon main function
  if (icon_int == 1) Show_dispenser_icon[5] = 0x11;
  else if (icon_int == 2) Show_dispenser_icon[5] = 0x12;
  else if (icon_int == 3) Show_dispenser_icon[5] = 0x13;
  else if (icon_int == 4) Show_dispenser_icon[5] = 0x14;
  else if (icon_int == 5) Show_dispenser_icon[5] = 0x15;
  else if (icon_int == 6) Show_dispenser_icon[5] = 0x16;
  else if (icon_int == 7) Show_dispenser_icon[5] = 0x17;
  else if (icon_int == 8) Show_dispenser_icon[5] = 0x18;

  Serial2.write(Show_dispenser_icon, 8);
  Serial2.write(Show_dispenser_icon, 8);
}

void Show_Fueltype(char* Fuel_data, int fuelid) {  //show Fuel type

  if (fuelid == 1) Show_fuel[4] = 0x91;
  else if (fuelid == 2) Show_fuel[4] = 0x92;
  else if (fuelid == 3) Show_fuel[4] = 0x93;
  else if (fuelid == 4) Show_fuel[4] = 0x94;
  else if (fuelid == 5) Show_fuel[4] = 0x95;
  else if (fuelid == 6) Show_fuel[4] = 0x96;
  else if (fuelid == 7) Show_fuel[4] = 0x97;
  else if (fuelid == 8) Show_fuel[4] = 0x98;

  Serial2.write(Show_fuel, 6);
  Serial2.write(Fuel_data, 20);
}

// void Default_Show_Fueltype(int fuelid) {  //show Fuel type

//   if (fuelid == 1) Default_Show_fuel[4] = 0x91;
//   else if (fuelid == 2) Default_Show_fuel[4] = 0x92;
//   else if (fuelid == 3) Default_Show_fuel[4] = 0x93;
//   else if (fuelid == 4) Default_Show_fuel[4] = 0x94;
//   else if (fuelid == 5) Default_Show_fuel[4] = 0x95;
//   else if (fuelid == 6) Default_Show_fuel[4] = 0x96;
//   else if (fuelid == 7) Default_Show_fuel[4] = 0x97;
//   else if (fuelid == 8) Default_Show_fuel[4] = 0x98;

//   Serial2.write(Default_Show_fuel, 8);
//   Serial2.write(Default_Show_fuel, 8);
// }

void Icon_fun() {  // to hide wifi icon

  // Serial2.write(Hide_wifi_icon, 8);
  Serial2.write(Show_icon_one, 8);
  Serial2.write(Show_icon_three, 8);
  Serial2.write(Hide_cloud_icon, 8);

  Serial.println("icon function done");
}

void screen_brightness_out() {  // screen brightness out

  if (millis() / 1000 - Hmi_time > 300) {

    Page_change[9] = 0x11;


    Serial.println("hmi one brightness out");

    Serial2.write(Page_change, 10);
    Serial2.write(Page_change, 10);

    Serial2.write(brightout, 8);
    Serial2.write(brightout, 8);

    rfid_check_two = false;
    rfid_auth_check_two = false;
    brightness_check = false;
    Serial.println("brightness out");
  }
}

void screen_brightness_in() {  // screen brightness


  Serial2.write(brightin, 8);
  Serial2.write(brightin, 8);

  common_page_check_fun(nozzlenum);  // to check page for screen brightness

  brightness_check = true;

  rfid_check_two = false;
  rfid_auth_check_two = false;
}

void common_page_check_fun(int page_data) {
  Serial.println("coommon page check");

  if (page_data == 0 || page_data != nozzlenum) {
    page_data = nozzlenum;
  }

  switch (page_data) {
    case 1:
      Serial2.write(two_Page_change, 10);
      delay(2);
      break;
    case 2:
      Serial2.write(two_Page_change, 10);
      delay(2);
      break;
    case 3:
      Serial2.write(four_Page_change, 10);
      delay(2);
      break;
    case 4:
      Serial2.write(four_Page_change, 10);
      delay(2);
      break;
    case 5:
      Serial2.write(six_Page_change, 10);
      delay(2);
      break;
    case 6:
      Serial2.write(six_Page_change, 10);
      delay(2);
      break;
    case 7:
      Serial2.write(eight_Page_change, 10);
      delay(2);
      break;
    case 8:
      Serial2.write(eight_Page_change, 10);
      delay(2);
      break;
  }
}

void default_setup_all() {  // power on setup all

  for (int i = 1; i < 9; i++) {
    ShowDisplay(i, 0);
  }

  for (int i = 1; i < 9; i++) {
    show_dispenser_icon_initial(i);
  }

  // initial_setup();  // intitial setup in hmi ak ch
  Icon_fun();

  ShowDisplay(1, dispenser_1_price);
  ShowDisplay(2, dispenser_2_price);
  ShowDisplay(3, dispenser_3_price);
  ShowDisplay(4, dispenser_4_price);
  ShowDisplay(5, dispenser_5_price);
  ShowDisplay(6, dispenser_6_price);
  ShowDisplay(7, dispenser_7_price);
  ShowDisplay(8, dispenser_8_price);

  show_nozzle_no_fun(pumpid1, 1);
  show_nozzle_no_fun(pumpid2, 2);
  show_nozzle_no_fun(pumpid3, 3);
  show_nozzle_no_fun(pumpid4, 4);
  show_nozzle_no_fun(pumpid5, 5);
  show_nozzle_no_fun(pumpid6, 6);
  show_nozzle_no_fun(pumpid7, 7);
  show_nozzle_no_fun(pumpid8, 8);

  char* cut_Fuel_Type_1 = cut_fuel_name(Fuel_Type_1);
  char* cut_Fuel_Type_2 = cut_fuel_name(Fuel_Type_2);
  char* cut_Fuel_Type_3 = cut_fuel_name(Fuel_Type_3);
  char* cut_Fuel_Type_4 = cut_fuel_name(Fuel_Type_4);
  char* cut_Fuel_Type_5 = cut_fuel_name(Fuel_Type_5);
  char* cut_Fuel_Type_6 = cut_fuel_name(Fuel_Type_6);
  char* cut_Fuel_Type_7 = cut_fuel_name(Fuel_Type_7);
  char* cut_Fuel_Type_8 = cut_fuel_name(Fuel_Type_8);

  Show_Fueltype(cut_Fuel_Type_1, 1);
  Show_Fueltype(cut_Fuel_Type_2, 2);
  Show_Fueltype(cut_Fuel_Type_3, 3);
  Show_Fueltype(cut_Fuel_Type_4, 4);
  Show_Fueltype(cut_Fuel_Type_5, 5);
  Show_Fueltype(cut_Fuel_Type_6, 6);
  Show_Fueltype(cut_Fuel_Type_7, 7);
  Show_Fueltype(cut_Fuel_Type_8, 8);

  // Serial.print("EEPROM cut fuel type one is => ");
  // Serial.println(cut_Fuel_Type_1);
  // Serial.println(cut_Fuel_Type_2);
  // Serial.println(cut_Fuel_Type_3);
  // Serial.println(cut_Fuel_Type_4);
  // Serial.println(cut_Fuel_Type_5);
  // Serial.println(cut_Fuel_Type_6);
  // Serial.println(cut_Fuel_Type_7);
  // Serial.println(cut_Fuel_Type_8);
}

void show_fuel_type_name() {  // to show fuel type setup

  char* cut_Fuel_Type_1 = cut_fuel_name(Fuel_Type_1);
  char* cut_Fuel_Type_2 = cut_fuel_name(Fuel_Type_2);
  char* cut_Fuel_Type_3 = cut_fuel_name(Fuel_Type_3);
  char* cut_Fuel_Type_4 = cut_fuel_name(Fuel_Type_4);
  char* cut_Fuel_Type_5 = cut_fuel_name(Fuel_Type_5);
  char* cut_Fuel_Type_6 = cut_fuel_name(Fuel_Type_6);
  char* cut_Fuel_Type_7 = cut_fuel_name(Fuel_Type_7);
  char* cut_Fuel_Type_8 = cut_fuel_name(Fuel_Type_8);

  Show_Fueltype(cut_Fuel_Type_1, 1);
  Show_Fueltype(cut_Fuel_Type_2, 2);
  Show_Fueltype(cut_Fuel_Type_3, 3);
  Show_Fueltype(cut_Fuel_Type_4, 4);
  Show_Fueltype(cut_Fuel_Type_5, 5);
  Show_Fueltype(cut_Fuel_Type_6, 6);
  Show_Fueltype(cut_Fuel_Type_7, 7);
  Show_Fueltype(cut_Fuel_Type_8, 8);

  // Serial.print("EEPROM cut fuel type one is => ");
  // Serial.println(cut_Fuel_Type_1);
  // Serial.println(cut_Fuel_Type_2);
  // Serial.println(cut_Fuel_Type_3);
  // Serial.println(cut_Fuel_Type_4);
  // Serial.println(cut_Fuel_Type_5);
  // Serial.println(cut_Fuel_Type_6);
  // Serial.println(cut_Fuel_Type_7);
  // Serial.println(cut_Fuel_Type_8);
}


////////// EEPROm functions ////////


char* cut_fuel_name(char* cut_data) {  // cut fuel type name

  char* common_data = (char*)malloc(20);  // Dynamically allocate memory

  if (common_data == NULL) {
    return NULL;
  }

  for (int i = 0; i < 20; i++) common_data[i] = 0;

  for (int j = 0; j < 20; j++) {
    if (cut_data[4 + j] == '\0') break;
    common_data[j] = cut_data[4 + j];
  }

  // Serial.print("common cut data is => ");
  // Serial.println(common_data);

  return common_data;
}

void eeprom_writeInt(int add, long data) {
  byte byte0 = (data >> 24) & 0xff;
  byte byte1 = (data >> 16) & 0xff;
  byte byte2 = (data >> 8) & 0xff;
  byte byte3 = data & 0xff;

  EEPROM.write(add, byte0);
  EEPROM.write(add + 1, byte1);
  EEPROM.write(add + 2, byte2);
  EEPROM.write(add + 3, byte3);
  EEPROM.commit();
}

int eeprom_read_data(int address) {
  return ((long)EEPROM.read(address) << 24) + ((long)EEPROM.read(address + 1) << 16) + ((long)EEPROM.read(address + 2) << 8) + (long)EEPROM.read(address + 3);
}


void writeStringToEEPROM(int address, String data) {  // to store wifi ssid and pass in EEprom
  int data_length = data.length();
  for (int i = 0; i < data_length; i++) {
    EEPROM.write(address + i, data[i]);
    EEPROM.commit();
  }
  EEPROM.write(address + data_length, '\0');
  EEPROM.commit();
  Serial.println("EEPROM write complete");
}

char* read_Fuel_type(int fuel_start_address, int fuel_end_address) {  // to save fuel type in eeprom
  char tem_data;
  char* common_data = (char*)malloc(20);  // Dynamically allocate memory

  if (common_data == NULL) {
    return NULL;
  }

  for (int i = 0; i < 20; i++) common_data[i] = 0;

  for (int j = fuel_start_address; j < fuel_end_address; j++) {
    tem_data = EEPROM.read(j);
    if (tem_data == '\0') break;
    common_data[j - fuel_start_address] = tem_data;
  }

  // Serial.print("common data is => ");
  // Serial.println(common_data);

  return common_data;
}

// void eeprom_writeInt(int add, int data) {

//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);

//   Wire.write((uint8_t)(add >> 8));    // MSB
//   Wire.write((uint8_t)(add & 0xFF));  // LSB

//   // Write the value bytes
//   Wire.write((uint8_t)(data >> 24));   // Byte 3
//   Wire.write((uint8_t)(data >> 16));   // Byte 2
//   Wire.write((uint8_t)(data >> 8));    // Byte 1
//   Wire.write((uint8_t)(data & 0xFF));  // Byte 0

//   Wire.endTransmission();

//   delay(5);
// }


// int eeprom_read_data(int address) {
//   int value = 0;

//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);

//   Wire.write((uint8_t)(address >> 8));
//   Wire.write((uint8_t)(address & 0xFF));  // LSB
//   Wire.endTransmission(false);            // Send repeated start condition

//   Wire.requestFrom(EEPROM_I2C_ADDRESS, 4);

//   if (Wire.available() >= 4) {
//     value = Wire.read() << 24;   // Byte 3
//     value |= Wire.read() << 16;  // Byte 2
//     value |= Wire.read() << 8;   // Byte 1
//     value |= Wire.read();        // Byte 0
//   }

//   return value;
// }


// void writeStringToEEPROM(int address, String data) {
//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);

//   Wire.write((uint8_t)(address >> 8));
//   Wire.write((uint8_t)(address & 0xFF));

//   int i = 0;
//   while (data[i]) {
//     Wire.write(data[i]);
//     i++;
//   }
//   Wire.write('\0');  // Null-terminate the string
//   Wire.endTransmission();

//   delay(5);  // Delay for EEPROM write cycle
// }

// void readStringFromEEPROM(int address, char* buffer, size_t bufferSize) {
//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);

//   Wire.write((uint8_t)(address >> 8));
//   Wire.write((uint8_t)(address & 0xFF));
//   Wire.endTransmission(false);

//   Wire.requestFrom(EEPROM_I2C_ADDRESS, bufferSize);

//   size_t i = 0;
//   while (Wire.available() && i < bufferSize - 1) {
//     buffer[i++] = Wire.read();
//   }
//   buffer[i] = '\0';
// }

// void setDataEEPROM(int alamat, byte data) {
//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);
//   Wire.write((int)(alamat >> 8));    //MSB
//   Wire.write((int)(alamat & 0xFF));  //LSB
//   Wire.write(data);
//   Wire.endTransmission();
//   delay(5);
// }

// byte getDataEEPROM(int alamat) {
//   byte data = 0xFF;
//   Wire.beginTransmission(EEPROM_I2C_ADDRESS);
//   Wire.write((int)(alamat >> 8));
//   Wire.write((int)(alamat & 0xFF));
//   Wire.endTransmission();
//   Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);
//   data = Wire.read();
//   return data;
// }

#endif