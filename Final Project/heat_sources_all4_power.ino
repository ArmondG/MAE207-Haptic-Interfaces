
/*************************************************** 
  This is a library for the Adafruit PT100/P1000 RTD Sensor w/MAX31865

  Designed specifically to work with the Adafruit RTD Sensor
  ----> https://www.adafruit.com/products/3328

  This sensor uses SPI to communicate, 4 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_MAX31865.h>

// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo1 = Adafruit_MAX31865(28, 26, 24, 22);
Adafruit_MAX31865 thermo2 = Adafruit_MAX31865(29, 27, 25, 23);
Adafruit_MAX31865 thermo3 = Adafruit_MAX31865(52, 50, 48, 46);
Adafruit_MAX31865 thermo4 = Adafruit_MAX31865(53, 51, 49, 47);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF 4300.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL 1000.0

unsigned long previousMillis = 0;  // will store last time the print statement was executed

// PELTIER
int RPWM_Output1 = 3;  // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int LPWM_Output1 = 2;  // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)
int RPWM_Output2 = 5;
int LPWM_Output2 = 4;
int RPWM_Output3 = 9; //SKIP 6,7 - USED FOR THE FAN
int LPWM_Output3 = 8;
int RPWM_Output4 = 11;
int LPWM_Output4 = 10;
int userPWM1 = 0;
int userPWM2 = 0;
int userPWM3 = 0;
int userPWM4 = 0;
int xcoor = 0;
int ycoor = 0;
float coldScale = 1;
float hotScale = 1;

// FAN
int direction_pin = 7;
int pwm_pin = 6;
float motor_value = 255;  // 0 - 255
int motor_direction = 1;  //0 or 1 (MUST BE 1 - OPPOSITE DIRECTION DOESNT WORK ON THE FAN)

// COMMS
String serial_line;
// strings[number of comma-separated data points + 1]
char *strings[7];  // an array of pointers to the pieces of the above array after strtok()
char *ptr = NULL;

void setup() {
  Serial.begin(250000);

  // TEMPERATURE
  thermo1.begin(MAX31865_4WIRE);  // set to 2WIRE or 4WIRE as necessary
  thermo2.begin(MAX31865_4WIRE);
  thermo3.begin(MAX31865_4WIRE);
  thermo4.begin(MAX31865_4WIRE);

  // PELTIER
  //set pins
  pinMode(RPWM_Output1, OUTPUT);
  pinMode(LPWM_Output1, OUTPUT);
  pinMode(RPWM_Output2, OUTPUT);
  pinMode(LPWM_Output2, OUTPUT);
  pinMode(RPWM_Output3, OUTPUT);
  pinMode(LPWM_Output3, OUTPUT);
  pinMode(RPWM_Output4, OUTPUT);
  pinMode(LPWM_Output4, OUTPUT);

  // FAN
  pinMode(direction_pin, OUTPUT);                // Set the direction pin output
  pinMode(pwm_pin, OUTPUT);                      // Set the pwm pin output
  digitalWrite(direction_pin, motor_direction);  // Set the motor in motion
  analogWrite(pwm_pin, motor_value);             // Set the motor in motion

  //Establich connection with Processing
  establishContact();
}


void loop() {
  //PELTIER
  if (Serial.available() > 0) {

    //Read the next available byte in the serial receive buffer
    serial_line = Serial.readStringUntil('\n');
    //Serial.println(serial_line);
    int length = serial_line.length() + 1;
    char array[length];
    serial_line.toCharArray(array, length);
    ptr = strtok(array, ",");  // comma delimiter
    byte index = 0;
    while (ptr != NULL) {
      strings[index] = ptr;
      index++;
      ptr = strtok(NULL, ",");
    }
    // use the atoi() and atof() functions to convert ASCII strings to numbers.
    userPWM1 = atoi(strings[0]);
    userPWM2 = atoi(strings[1]);
    userPWM3 = atoi(strings[2]);
    userPWM4 = atoi(strings[3]);
    xcoor = atoi(strings[4]);
    ycoor = atoi(strings[5]);

    if (userPWM1 <= 0) { //the 0 case applies 0 PWM to the reverse pin
    //NOTE: DOES NOT COVER ALL GRADIENT CASES. ASSUMES ALL PELTIERS ARE HOT OR COLD AT ONCE
      // reverse rotation
      if (millis()%2400 <400){
      analogWrite(LPWM_Output1, 0);
      analogWrite(RPWM_Output1, abs(userPWM1)*coldScale*2);  //takes magnitude of "negative PWM"
      }
      else{
        analogWrite(LPWM_Output1, 0);
        analogWrite(RPWM_Output1, 0);
        
      }
    }
    else if (userPWM1 >0){  //if (Serial.read() > 0)
      // forward rotation
      analogWrite(LPWM_Output1, abs(userPWM4)*hotScale); //need abs here too (idk)
      analogWrite(RPWM_Output1, 0);
    }

    if (userPWM2 <= 0) { //the 0 case applies 0 PWM to the reverse pin
    //NOTE: DOES NOT COVER ALL GRADIENT CASES. ASSUMES ALL PELTIERS ARE HOT OR COLD AT ONCE
      // reverse rotation
      if (millis()%2400 <400){
      analogWrite(LPWM_Output2, 0);
      analogWrite(RPWM_Output2, abs(userPWM2)*coldScale*2);  //takes magnitude of "negative PWM"
      }
      else{
        analogWrite(LPWM_Output2, 0);
        analogWrite(RPWM_Output2, 0);
        
      }
    }
    else if (userPWM2 >0){  //if (Serial.read() > 0)
      // forward rotation
      analogWrite(LPWM_Output2, abs(userPWM2)*hotScale); //need abs here too (idk)
      analogWrite(RPWM_Output2, 0);
    }

    if (userPWM3 <= 0) { //the 0 case applies 0 PWM to the reverse pin
    //NOTE: DOES NOT COVER ALL GRADIENT CASES. ASSUMES ALL PELTIERS ARE HOT OR COLD AT ONCE
      // reverse rotation
      if (millis()%2400 <400){
      analogWrite(LPWM_Output3, 0);
      analogWrite(RPWM_Output3, abs(userPWM3)*coldScale*2);  //takes magnitude of "negative PWM"
      }
      else{
        analogWrite(LPWM_Output3, 0);
        analogWrite(RPWM_Output3, 0);
        
      }
    }
    else if (userPWM3 >0){  //if (Serial.read() > 0)
      // forward rotation
      analogWrite(LPWM_Output3, abs(userPWM3)*hotScale); //need abs here too (idk)
      analogWrite(RPWM_Output3, 0);
    }

    if (userPWM4 <= 0) { //the 0 case applies 0 PWM to the reverse pin
    //NOTE: DOES NOT COVER ALL GRADIENT CASES. ASSUMES ALL PELTIERS ARE HOT OR COLD AT ONCE
      // reverse rotation
      if (millis()%2400 <400){
      analogWrite(LPWM_Output4, 0);
      analogWrite(RPWM_Output4, abs(userPWM4)*coldScale*2);  //takes magnitude of "negative PWM"
      }
      else{
        analogWrite(LPWM_Output4, 0);
        analogWrite(RPWM_Output4, 0);
        
      }
    }
    else if (userPWM4 >0){  //if (Serial.read() > 0)
      // forward rotation
      analogWrite(LPWM_Output4, abs(userPWM4)*hotScale); //need abs here too (idk)
      analogWrite(RPWM_Output4, 0);
    }


    if (thermo1.temperature(RNOMINAL, RREF) > 35) {
    analogWrite(LPWM_Output1, 0);
    analogWrite(RPWM_Output1, 0);
  }
  if (thermo2.temperature(RNOMINAL, RREF) > 35) {
    analogWrite(LPWM_Output2, 0);
    analogWrite(RPWM_Output2, 0);
  }
  if (thermo3.temperature(RNOMINAL, RREF) > 35) {
    analogWrite(LPWM_Output3, 0);
    analogWrite(RPWM_Output3, 0);
  }
  if (thermo4.temperature(RNOMINAL, RREF) > 35) {
    analogWrite(LPWM_Output4, 0);
    analogWrite(RPWM_Output4, 0);
  }

    String tmp1 = String(thermo1.temperature(RNOMINAL, RREF));
    String tmp2 = String(thermo2.temperature(RNOMINAL, RREF));
    String tmp3 = String(thermo3.temperature(RNOMINAL, RREF));
    String tmp4 = String(thermo4.temperature(RNOMINAL, RREF));
    // Print Comma-separated data points
    Serial.println(String(userPWM1) + "," + String(userPWM2) + "," + String(userPWM3) + "," + String(userPWM4) + "," + String(xcoor) + "," + String(ycoor) +" T1: " + tmp1 + " T2: " + tmp2 + " T3: " + tmp3 + " T4: " + tmp4);
  }

}
void establishContact() { //Bidirectional serial communication function
  while (Serial.available() <= 0) {
    Serial.println("A");
    delay(200);
  }
}
