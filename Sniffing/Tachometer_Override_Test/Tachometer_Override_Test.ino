// Arduino Due - Displays all traffic found on either canbus port
// By Thibaut Viard/Wilfredo Molina/Collin Kidder 2013-2014

// Required libraries
#include "variant.h"
#include <due_can.h>

//Leave defined if you use native port, comment if using programming port
//This sketch could provide a lot of traffic so it might be best to use the
//native port
void setup()
{

  //Serial.begin(115200);
  
  // Initialize CAN0 and CAN1, Set the proper baud rates here
  //Can0.begin(CAN_BPS_250K);
  Can1.begin(CAN_BPS_125K);
  
  //By default there are 7 mailboxes for each device that are RX boxes
  //This sets each mailbox to have an open filter that will accept extended
  //or standard frames
  Can1.watchFor(0x09C050B8);
  Can1.setGeneralCallback(sendModifiedFrame);
  
}

void loop(){

}

void sendModifiedFrame(CAN_FRAME *incoming){
  CAN_FRAME modified = *incoming;
  //Can1.read((*incoming));
  modified.data.bytes[3]=0xFF;
  modified.data.bytes[4]=0xff;
  delayMicroseconds(5); 
  Can1.sendFrame(modified);
}
