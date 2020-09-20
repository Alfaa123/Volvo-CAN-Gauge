#include <SPI.h>
#include "src/can/mcp_can.h"
#include <Arduino.h>
#include <genieArduino.h>


float Boost;
long EngineSpeed, CoolantTemp, IntakeTemp, VehicleSpeed, ButtonHeld;
unsigned char BP[8] = {0xCD, 0x7a, 0xa6, 0x12, 0x9d, 0x01, 0x00, 0x00};
unsigned char RPM[8] = {0xCD, 0x7a, 0xa6, 0x10, 0x1d, 0x01, 0x00, 0x00};
unsigned char COL[8] = {0xCD, 0x7a, 0xa6, 0x10, 0xd8, 0x01, 0x00, 0x00};
unsigned char IAT[8] = {0xCD, 0x7a, 0xa6, 0x10, 0xCE, 0x01, 0x00, 0x00};
unsigned char VHS[8] = {0xCD, 0x7a, 0xa6, 0x11, 0x40, 0x01, 0x00, 0x00};
int x, Brightness;
unsigned char len = 0, flagRecv = 0, Page = 0, Index = 0;
unsigned char buf[8];
bool NightMode, Ignition;
static long updatePeriod = millis();
static int gaugeAddVal = 1;
static int gaugeVal = 0;
static int gaugeCurrentValue = 0;
Genie genie;
#define RESETLINE 8

MCP_CAN CAN(10); // Normally where the CS pin is set. Because the CAN library has been hacked to use direct port manipulation, this no longer matters.

void setup()
{
  Serial1.begin(115200);  // Serial1 @ 200000 (200K) Baud
  genie.Begin(Serial1);   // Use Serial1 for talking to the Genie Library, and to the 4D Systems display
   pinMode(RESETLINE, OUTPUT);  // Set D4 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
  digitalWrite(RESETLINE, 0);  // Reset the Display via D4
  delay(100);
  digitalWrite(RESETLINE, 1);  // unReset the Display via D4
  delay (5000); 
  while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
  {
    delay(100);
  }
  CAN.init_Mask(0, 1, 0xFFFFFFFF);                         // Masks and filters setup for the can interface. We use these to keep resource usage low by only looking for traffic we care about.
  CAN.init_Mask(1, 1, 0xFFFFFFFF);
  CAN.init_Filt(0, 1, 0x00400021);                          
  CAN.init_Filt(1, 1, 0x00000000);                          
  CAN.init_Filt(2, 1, 0x19E00006);                          
  CAN.init_Filt(3, 1, 0x0100082C);                          
  CAN.init_Filt(4, 1, 0x19000026);                          
  CAN.init_Filt(5, 1, 0x00000000);
}

void loop()    //The main loop sends all the various CAN messages to the ECU so we can get data back. It also calls the MessageRecieve and UpdateDisplay functions periodically. This method of 'multitasking' is painful and needs to be rewritten.
{
  if (Ignition == 0) {
    PowerSaveLoop();
  }
  
  if (Ignition) {
    UpdateDisplay();
  }
  x++;
  if (x > 1 && (Page==0 || Page==1)){
     CAN.sendMsgBuf(0x000FFFFE, 1, 8, BP); //Give us boost pressure!
     x = 0;
  }
  if (x > 2000 && (Page==2 || Page==3)){
     CAN.sendMsgBuf(0x000FFFFE, 1, 8, COL); //Give us coolant temp!
     x = 0;
  }
    if (x > 100 && (Page==4 || Page==5)){
     CAN.sendMsgBuf(0x000FFFFE, 1, 8, IAT); //Give us intake temp!
     x = 0;
  }
  
MessageRecieveLoop();
}

void UpdateDisplay() {               //This function takes the data retrieved in the MessageRecieveLoop and writes it to the OLED. Because the OLED is so slow, this is where we spend the majority of our loop time.
  //Boost Display
  if (Page == 0){
    gaugeVal = Boost * 0.145
    }
  //Boost Graph Display
  else if (Page == 1){}
  //Coolant Temperature Display
  else if (Page == 2){}
  //Coolant temp graph display
    else if (Page == 3){}
  //Intake temp display
  else if (Page == 4){}
  //Intake temp graph display
    else if (Page == 5){}
    if ((gaugeCurrentValue != gaugeVal)&(updatePeriod < millis()))
  {
    // Write to CoolGauge0 with the value in the gaugeVal variable
    if (gaugeCurrentValue > gaugeVal) gaugeAddVal = -1;
    if (gaugeCurrentValue < gaugeVal) gaugeAddVal = 1;
    gaugeCurrentValue += gaugeAddVal;
    genie.WriteObject(GENIE_OBJ_IANGULAR_METER, 0, gaugeCurrentValue);
    genie.WriteObject(GENIE_OBJ_ILED_DIGITS, 0, gaugeCurrentValue);

    // Simulation code, just to increment and decrement gauge value each loop, for animation
   

    updatePeriod = millis() + 2;
    // The results of this call will be available to myGenieEventHandler() after the display has responded
   
     // rerun this code to update Cool Gauge and Slider in another 50ms time.
  }
  }
  


void UpdateBrightness() {                       //This function simply updates the brightness of the OLED when we adjust the dashboard lighting pot in the car. If we set the pot to minimum brightness, it simply turns the display off completely (for safety and night vision).

  if (Ignition){
   genie.WriteContrast(Brightness);
  }
}

void PowerSaveLoop(){                          // This is where we come if the ignition message goes low. We loop here indefinitely until we sense the ignition has come back on. OLED is off and filters are activated to ignore any traffic except the ignition status broadcast.
  CAN.init_Filt(1, 1, 0x19E00006);
    while (1 == 1) {
      delay(2000);
      if (CAN_MSGAVAIL == CAN.checkReceive())           
      {
        CAN.readMsgBuf(&len, buf);
        unsigned long canId = CAN.getCanId();
        if (canId == 0x19E00006) {
          if ((buf[6] & B01000000) != Ignition) {
            Ignition = (buf[6] & B01000000);
            UpdateIgnition();
            CAN.init_Filt(1, 1, 0x00000000);
            break;
          }
        }
      }
    }
}

void MessageRecieveLoop(){                                                  //I was never able to get inturrupt based frame handling working, so this loop is here to check if we have any new messages. If we do, update variables.
    while (CAN_MSGAVAIL == CAN.checkReceive())
  {
    //flagRecv = 0;
    CAN.readMsgBuf(&len, buf);

    unsigned long canId = CAN.getCanId();
    if (canId == 0x00400021) {
      if (buf[4] == 0x9d) {
        Boost = buf[5];
      }
      if (buf[4] == 0xd8) {
        CoolantTemp = buf[5];
        CoolantTemp = CoolantTemp*0.75-48;
      }
      if (buf[4] == 0xCE) {
        IntakeTemp = buf[5];
        IntakeTemp = IntakeTemp*0.75-48;
        //IntakeTemp = IntakeTemp*1.8+32;
      }
    }
    if (canId == 0x19E00006) {
      if ((buf[6] & B01000000) == !Ignition) {
        Ignition = (buf[6] & B01000000);
        UpdateIgnition();
      }
    }
    if (canId == 0x19000026){
      if (buf[7] && B00100000){
         if (ButtonHeld < 1){
           ButtonHeld = millis();
        }
        if ((millis()-ButtonHeld) > 1000 ){
          Page++;
          if (Page > 0){
            Page = 0;
          }
          ButtonHeld = millis();
        }
      }
      else{
        ButtonHeld = 0;
      }
      }
    if (canId == 0x0100082C) {
      if ((buf[1] & B10000000) != NightMode) {
        NightMode = buf[1] & B10000000;
        if (!NightMode) {
          Brightness = Brightness * 0.3;
        }
        UpdateBrightness();
      }
      if ((buf[0] & B00001111) != Brightness) {
        Brightness = (buf[0] & B00001111) * 15;
        if (!NightMode) {
          Brightness = Brightness * 0.3;
        }
        UpdateBrightness();
      }
    }
  }
}  

void UpdateIgnition() {               //This is where we go if the ignition status changes. If it goes high to low (car turns off) then we fade the display out, blank it and put it to sleep for the power save loop. If it goes low to high (car turns on) we wake the display and write a welcome message to the display and proceed to the main loop.
  if (Ignition) {

    for (int i = 0; i < Brightness; i++) {
      genie.WriteContrast(i);
    }

  }
  else {
    for (int i = Brightness; i > 1; i--) {
     genie.WriteContrast(i);
    }
   
  }
}
