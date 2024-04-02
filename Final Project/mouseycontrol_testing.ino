/*

  Dimmer

  Demonstrates sending data from the computer to the Arduino board, in this case

  to control the brightness of an LED. The data is sent in individual bytes,

  each of which ranges from 0 to 255. Arduino reads these bytes and uses them to

  set the brightness of the LED.

  The circuit:

  - LED attached from digital pin 9 to ground.

  - Serial connection to Processing, Max/MSP, or another serial application

  created 2006

  by David A. Mellis

  modified 30 Aug 2011

  by Tom Igoe and Scott Fitzgerald

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/Dimmer

*/

const int ledPin = 5;      // the pin that the LED is attached to
const unsigned int MAX_MESSAGE_LENGTH = 15;

void setup() {

  // initialize the serial communication:

  Serial.begin(115200);

  // initialize the ledPin as an output:

  pinMode(ledPin, OUTPUT);
}

void loop() {

  byte brightness;

  // check if data has been sent from the computer:

  if (Serial.available()) {

    // read the most recent byte (which will be from 0 to 255):

     //Check to see if anything is available in the serial receive buffer

   //Create a place to hold the incoming message
   static char message[MAX_MESSAGE_LENGTH];
   static unsigned int message_pos = 0;

   //Read the next available byte in the serial receive buffer
   char inByte = Serial.read();

   //Message coming in (check not terminating character) and guard for over message size
   if ( inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1) )
   {
     //Add the incoming byte to our message
     message[message_pos] = inByte;
     message_pos++;
   }
   //Full message received...
   else
   {
     //Add null character to string
     message[message_pos] = '\0';
     

     //Reset for the next message
     message_pos = 0;
   }

   char token = strtok(message,",");
   while (token != NULL) {
      Serial.println(token);
      token=strtok(NULL, ",");
   }
   //int temperture = toInt(token[0]);
   //int xpos = toInt(token[1]);
   //int ypos = toInt(token[2]);
 }


    // set the brightness of the LED:

    //analogWrite(ledPin, temperture);

  }
