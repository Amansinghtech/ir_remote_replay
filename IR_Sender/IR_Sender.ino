/*
 * SimpleSender.cpp
 *
 *  Demonstrates sending IR codes in standard format with address and command
 *  An extended example for sending can be found as SendDemo.
 *
 *  Copyright (C) 2020-2022  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 *  MIT License
 */
#include <Arduino.h>

#if !defined(ARDUINO_ESP32C3_DEV) // This is due to a bug in RISC-V compiler, which requires unused function sections :-(.
#define DISABLE_CODE_FOR_RECEIVER // Disables static receiver code like receive timer ISR handler and static IRReceiver and irparams data. Saves 450 bytes program memory and 269 bytes RAM if receiving functions are not required.
#endif
//#define SEND_PWM_BY_TIMER         // Disable carrier PWM generation in software and use (restricted) hardware PWM.
//#define USE_NO_SEND_PWM           // Use no carrier PWM, just simulate an active low receiver signal. Overrides SEND_PWM_BY_TIMER definition

/*
 * This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
 */
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp> // include the library

bool completed = false;

uint8_t codes[] = {
  0x3, // mute
  0x9, // power
  0x1c, // menu
  0x51, // audio
  0x5c, // view 
  0xd, // 01
  0x19, // 02
  0x1b, // 03
  0x1, // 04
  0x11, // 05
  0x15, // 06
  0x17, // 07 
  0x6, // 08
  0x12, // 09
  0x4c, // signal
  0x16, // 0
  0x54, // pause
  0xe, // up
  0x1a, // down
  0x1e, // right
  0xa, // left
  0xc, // vol down
  0x5, // ok
  0x2 // vol up
};



void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    while (!Serial)
        ; // Wait for Serial to become available. Is optimized away for some cores.

    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));
    Serial.print(F("Send IR signals at pin "));
    Serial.println(IR_SEND_PIN);

    /*
     * The IR library setup. That's all!
     */
    IrSender.begin(); // Start with IR_SEND_PIN -which is defined in PinDefinitionsAndMore.h- as send pin and enable feedback LED at default feedback LED pin
    disableLEDFeedback(); // Disable feedback LED at default feedback LED pin
}


/*
 * Set up the data to be sent.
 * For most protocols, the data is build up with a constant 8 (or 16 byte) address
 * and a variable 8 bit command.
 * There are exceptions like Sony and Denon, which have 5 bit address.
 */
uint8_t sCommand = 0x09;
uint8_t sRepeats = 0;

void printCommandName(uint8_t command) {
  if (command == 0x3) {
      Serial.println("mute");
  } // mute
  if (command == 0x9) {
    Serial.println("power");
  } // power
  if (command == 0x1c) {
    Serial.println("menu");
  } // menu
  if (command == 0x51) {
    Serial.println("audio");
  } // audio
  if (command == 0x5c) {
    Serial.println("view ");
  } // view 
  if (command == 0xd) {
    Serial.println("01");
  } // 01
  if (command == 0x19) {
    Serial.println("02");
  } // 02
  if (command == 0x1b) {
    Serial.println("03");
  } // 03
  if (command == 0x1) {
    Serial.println("04");
  } // 04
  if (command == 0x11) {
    Serial.println("05");
  } // 05
  if (command == 0x15) {
    Serial.println("06");
  } // 06
  if (command == 0x17) {
    Serial.println("07 ");
  } // 07 
  if (command == 0x6) {
    Serial.println("08");
  } // 08
  if (command == 0x12) {
    Serial.println("09");
  } // 09
  if (command == 0x4c) {
    Serial.println("signal");
  } // signal
  if (command == 0x16) {
    Serial.println("0");
  } // 0
  if (command == 0x54) {
    Serial.println("pause");
  } // pause
  if (command == 0xe) {
    Serial.println("up");
  } // up
  if (command == 0x1a) {
    Serial.println("down");
  } // down
  if (command == 0x1e) {
    Serial.println("right");
  } // right
  if (command == 0xa) {
    Serial.println("left");
  } // left
  if (command == 0xc) {
    Serial.println("down");
  } // vol down
  if (command == 0x5) {
    Serial.println("ok");
  } // ok
  if (command == 0x2) {
    Serial.println("up");
  } // vol up
  }

void send_commands() {
  uint8_t code_sequence[] = { 0x1C, 0x1A, 0x1A, 0x1A, 0x5, 0x1A, 0x5, 0x1E, 0x5, 0x1C, 0xE, 0x5, 0x5, 0x16, 0x12, 0x17, 0x11, 0x16, 0x16, 0x12, 0x17, 0x11, 0x16, 0xD, 0xD, 0x16, 0x12, 0x16, 0x19, 0x12, 0x11, 0x16, 0x16, 0xE, 0x5, 0x5, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0xD, 0x17, 0x5, 0x5, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1, 0x5, 0x5, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x11, 0xD, 0x5, 0x5, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x11, 0x5, 0x1C, 0x1C, 0x1B, 0x1B, 0x4C };
  long timings[] = { 3685, 1968, 956, 1704, 899, 771, 806, 829, 738, 3084, 2186, 5571, 1058, 2371, 1429, 1611, 1589, 1358, 1782, 784, 1060, 1311, 1336, 1519, 715, 3037, 1221, 1265, 2509, 1243, 1749, 1553, 1128, 1451, 3936, 5847, 2268, 853, 922, 829, 898, 898, 807, 795, 829, 944, 1151, 830, 1289, 2854, 2531, 5984, 1750, 944, 898, 853, 1105, 794, 967, 876, 898, 759, 2301, 898, 1497, 2669, 6476, 2726, 1335, 1358, 1289, 1290, 1152, 1127, 1312, 1219, 1129, 1311, 1266, 1979, 1771, 2968, 7808, 2058, 1082, 1036, 921, 944, 852, 806, 884, 876, 885, 863, 805, 943, 1197, 3312, 7838, 897, 1472, 679, 3933 };
  if (!completed) {
      int total_commands = sizeof(code_sequence);
      int total_timings = sizeof(timings);
      Serial.print("Total commands:");
      Serial.print(total_commands);
      Serial.print(" timings:");
      Serial.println(total_timings);
      delay(5000);
      
      for (int i =0; i < total_commands; i++) {
          delay(timings[i]);
          printCommandName(code_sequence[i]);
          IrSender.sendNEC(0x00, code_sequence[i], sRepeats);
        }
    }
  completed = true;
}

void loop() {
    /*
     * Print current send values
     */
//    Serial.println();
//    Serial.print(F("Send now: address=0x00, command=0x"));
//    Serial.print(sCommand, HEX);
//    Serial.print(F(", repeats="));
//    Serial.print(sRepeats);
//    Serial.println();

    Serial.println("Starting Commands after 5s");

//    Serial.println(F("Send standard NEC with 8 bit address"));
    Serial.flush();

    send_commands();

    // Receiver output for the first loop must be: Protocol=NEC Address=0x102 Command=0x34 Raw-Data=0xCB340102 (32 bits)
//    IrSender.sendNEC(0x00, sCommand, sRepeats);

    /*
     * If you want to send a raw HEX value directly like e.g. 0xCB340102 you must use sendNECRaw()
     */
//    Serial.println(F("Send 32 bit LSB 0xCB340102 with NECRaw()"));
//    IrSender.sendNECRaw(0xCB340102, sRepeats);

    /*
     * If you want to send an "old" MSB HEX value used by IRremote versions before 3.0 like e.g. 0x40802CD3 you must use sendNECMSB()
     */
//    Serial.println(F("Send old 32 bit MSB 0x40802CD3 with sendNECMSB()"));
//    IrSender.sendNECMSB(0x40802CD3, 32, sRepeats);

    /*
     * Increment send values
     */
//    sCommand += 0x11;
//    sRepeats++;
//    // clip repeats at 4
//    if (sRepeats > 4) {
//        sRepeats = 4;
//    }

//    delay(1000);  // delay must be greater than 5 ms (RECORD_GAP_MICROS), otherwise the receiver sees it as one long signal
}
