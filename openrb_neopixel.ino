/*******************************************************************************
* Copyright 2025 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include <Dynamixel2Arduino.h>
#include <Adafruit_NeoPixel.h>

// Please modify it to suit your hardware.
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560) // When using DynamixelShield
  #include <SoftwareSerial.h>
  SoftwareSerial soft_serial(7, 8); // DYNAMIXELShield UART RX/TX
  #define DXL_SERIAL   Serial
  #define DEBUG_SERIAL soft_serial
  const int DXL_DIR_PIN = 2; // DYNAMIXEL Shield DIR PIN
#elif defined(ARDUINO_SAM_DUE) // When using DynamixelShield
  #define DXL_SERIAL   Serial
  #define DEBUG_SERIAL SerialUSB
  const int DXL_DIR_PIN = 2; // DYNAMIXEL Shield DIR PIN
#elif defined(ARDUINO_SAM_ZERO) // When using DynamixelShield
  #define DXL_SERIAL   Serial1
  #define DEBUG_SERIAL SerialUSB
  const int DXL_DIR_PIN = 2; // DYNAMIXEL Shield DIR PIN
#elif defined(ARDUINO_OpenCM904) // When using official ROBOTIS board with DXL circuit.
  #define DXL_SERIAL   Serial3 //OpenCM9.04 EXP Board's DXL port Serial. (Serial1 for the DXL port on the OpenCM 9.04 board)
  #define DEBUG_SERIAL Serial
  const int DXL_DIR_PIN = 22; //OpenCM9.04 EXP Board's DIR PIN. (28 for the DXL port on the OpenCM 9.04 board)
#elif defined(ARDUINO_OpenCR) // When using official ROBOTIS board with DXL circuit.
  // For OpenCR, there is a DXL Power Enable pin, so you must initialize and control it.
  // Reference link : https://github.com/ROBOTIS-GIT/OpenCR/blob/master/arduino/opencr_arduino/opencr/libraries/DynamixelSDK/src/dynamixel_sdk/port_handler_arduino.cpp#L78
  #define DXL_SERIAL   Serial3
  #define DEBUG_SERIAL Serial
  const int DXL_DIR_PIN = 84; // OpenCR Board's DIR PIN.    
#elif defined(ARDUINO_OpenRB)  // When using OpenRB-150
  //OpenRB does not require the DIR control pin.
  #define DXL_SERIAL Serial1
  #define DEBUG_SERIAL Serial
  const int DXL_DIR_PIN = -1;
#else // Other boards when using DynamixelShield
  #define DXL_SERIAL   Serial1
  #define DEBUG_SERIAL Serial
  const int DXL_DIR_PIN = 2; // DYNAMIXEL Shield DIR PIN
#endif
 

// Create a port object for DYNAMIXEL communication.
// The list of available classes is as follows.
// 1) DYNAMIXEL::SerialPortHandler     (HardwareSerial only)
//     -note: If you do not want to use half duplex communication, do not enter the second parameter.
// 2) DYNAMIXEL::USBSerialPortHandler  (Only USB port on each board)
DYNAMIXEL::SerialPortHandler dxl_port(DXL_SERIAL, DXL_DIR_PIN);

// Create a Slave object to communicate with the master.
const float DXL_PROTOCOL_VER_1_0 = 1.0;
const float DXL_PROTOCOL_VER_2_0 = 2.0;
const uint16_t DXL_MODEL_NUM = 0x5005; // Modify it to what you want.
DYNAMIXEL::Slave dxl(dxl_port, DXL_MODEL_NUM);

// NeoPixel configuration
#define PIN        2 // Pin connected to NeoPixels
#define NUMPIXELS 16 // Number of NeoPixels
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Control table addresses for RGB values
const uint16_t ADDR_CONTROL_ITEM_RED = 10;
const uint16_t ADDR_CONTROL_ITEM_GREEN = 11;
const uint16_t ADDR_CONTROL_ITEM_BLUE = 12;

// Variables to store RGB values
uint8_t control_item_red;
uint8_t control_item_green;
uint8_t control_item_blue;

// Function to update all pixels
void updatePixels(uint16_t addr, uint8_t &err, void* arg) {
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(control_item_red, control_item_green, control_item_blue));
  }
  pixels.show();
}

//This namespace is required to use Control table item names
using namespace ControlTableItem;

void setup() {
  // put your setup code here, to run once:
  DEBUG_SERIAL.begin(115200);

  // Speed setting for communication (not necessary for USB)
  dxl_port.begin(1000000);

  dxl.setPortProtocolVersion(DXL_PROTOCOL_VER_2_0);
  dxl.setFirmwareVersion(1);
  dxl.setID(200);

  // Initialize NeoPixels
  pixels.begin();
  pixels.clear();
  pixels.show();

  // Add control items for RGB values
  dxl.addControlItem(ADDR_CONTROL_ITEM_RED, control_item_red);
  dxl.addControlItem(ADDR_CONTROL_ITEM_GREEN, control_item_green);
  dxl.addControlItem(ADDR_CONTROL_ITEM_BLUE, control_item_blue);

  // Set write callback function
  dxl.setWriteCallbackFunc(updatePixels, nullptr);
}

void loop() {
  // Process Dynamixel packets
  if(dxl.processPacket() == false){
    DEBUG_SERIAL.print("Last lib err code: ");
    DEBUG_SERIAL.print(dxl.getLastLibErrCode());
    DEBUG_SERIAL.print(", ");
    DEBUG_SERIAL.print("Last status packet err code: ");
    DEBUG_SERIAL.println(dxl.getLastStatusPacketError());
    DEBUG_SERIAL.println();
  }
}