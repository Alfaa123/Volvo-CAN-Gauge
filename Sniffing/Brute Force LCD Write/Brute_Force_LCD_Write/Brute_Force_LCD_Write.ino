//Reads all traffic on CAN0 and forwards it to CAN1 (and in the reverse direction) but modifies some frames first.
// Required libraries
#include "variant.h"
#include <due_can.h>

//Leave defined if you use native port, comment if using programming port
//#define Serial SerialUSB
unsigned int ID = 0x00000000;
int count = 0;
int count2 = 0;
void setup()
{

  Serial.begin(115200);
  
  // Initialize CAN0 and CAN1, Set the proper baud rates here
  Can1.begin(CAN_BPS_125K);
  
  //By default there are 7 mailboxes for each device that are RX boxes
  //This sets each mailbox to have an open filter that will accept extended
  //or standard frames
  int filter;
  //extended
  for (filter = 0; filter < 3; filter++) {
	Can1.setRXFilter(filter, 0, 0, true);
  }  
  //standard
  //for (int filter = 3; filter < 7; filter++) {
	//Can0.setRXFilter(filter, 0, 0, false);
	//Can1.setRXFilter(filter, 0, 0, false);
  //}  
  delay(5000);
}

void loop(){
  CAN_FRAME first;
  first.extended=1;
  first.id=0x03C3F7FC;
  first.length=8;
  first.data.bytes[0]=0xf8;
  first.data.bytes[1]=0x00;
  first.data.bytes[2]=0x00;
  first.data.bytes[3]=0x11;
  first.data.bytes[4]=0x40;
  first.data.bytes[5]=0x24;
  first.data.bytes[6]=0x38;
  first.data.bytes[7]=0x15;
  CAN_FRAME second;
  second.extended=1;
  second.length=8;
  second.id=0x09c050b8;
  second.data.bytes[0]=0x4f;
  second.data.bytes[1]=0x6c;
  second.data.bytes[2]=0x0a;
  second.data.bytes[3]=0x00;
  second.data.bytes[4]=0x00;
  second.data.bytes[5]=0x9e;
  second.data.bytes[6]=0x00;
  second.data.bytes[7]=0x4c;
  CAN_FRAME third;
  third.extended=1;
  third.length=8;
  third.id=0x0ee0500a;
  third.data.bytes[0]=0x00;
  third.data.bytes[1]=0x30;
  third.data.bytes[2]=0x00;
  third.data.bytes[3]=0x01;
  third.data.bytes[4]=0x80;
  third.data.bytes[5]=0x07;
  third.data.bytes[6]=0x01;
  third.data.bytes[7]=0x24;
  CAN_FRAME fourth;
  fourth.extended=1;
  fourth.length=8;
  fourth.id=0x1093f7fc;
  fourth.data.uint64=8790989332831993857;
  CAN_FRAME fifth;
  fifth.extended=1;
  fifth.length=8;
  fifth.id=0x000ffffe;
  fifth.data.uint64=4037104074;

if (count > 200){
second.data.bytes[0]=0x48;
}

count++;
if (count > 400){count=0;}
  
  CAN_FRAME part1;
  part1.extended=1;
  part1.length=8;
  part1.id=ID;
  part1.data.bytes[1]=0xe1;
  part1.data.bytes[2]=0xfe;

  CAN_FRAME part2;
  part2.extended=1;
  part2.length=8;
  part2.id=ID;
  part2.data.uint64=3403156113903321255;

  CAN_FRAME part3;
  part3.extended=1;
  part3.length=8;
  part3.id=ID;
  part3.data.uint64=7956008291306843937;

  CAN_FRAME part4;
  part4.extended=1;
  part4.length=8;
  part4.id=ID;
  part4.data.uint64=3346023256258144034;

  CAN_FRAME part5;
  part5.extended=1;
  part5.length=8;
  part5.id=ID;
  part5.data.uint64=8030045023689794083;
  
  CAN_FRAME part6;
  part6.extended=1;
  part6.length=8;
  part6.id=ID;
  part6.data.uint64=120325172786277;

	Can1.sendFrame(first);
  Can1.sendFrame(second);
  Can1.sendFrame(third);
  Can1.sendFrame(fourth);
  Can1.sendFrame(fifth);
  count2++;
if (count2 > 2){
  Can1.sendFrame(part1);
  Can1.sendFrame(part2);
  Can1.sendFrame(part3);
  Can1.sendFrame(part4);
  Can1.sendFrame(part5);
  Can1.sendFrame(part6);
  ID++;
  count2 = 0;
}
  delay(5);
  Serial.print("ID is currently: ");
  Serial.println(ID,HEX);
}
