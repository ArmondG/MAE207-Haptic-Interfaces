#include <LinkedList.h>

// This script is used for tuning PID gains for DTF configuration

// Setup General Parameters

  // Include Libraries
    #include <PID_v1.h> // Include PID function library
    #include <Adafruit_MAX31865.h> // Include Adafruit RTD Circuit Breakout Board library

  // Initialize RTD measurement Circuits
    Adafruit_MAX31865 HTF = Adafruit_MAX31865(28, 26, 24, 22); // 
    
    #define RREF      4300.0 // Define reference resistance (430 for 100 ohm, 4300 for 1000 ohm)
    #define RNOMINAL  1000.0 // Define RTD resistance (100 for PT100, 1000 for PT1000)
    
  // Pins
    int fan_pin = 6; // Define pin for fan power to motor shield
    //int heater_pin = 3; // Define pin for heater pwm signal
    int RPWM_Output = 4;  // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
    int LPWM_Output = 5;  // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)
  
  // General Parameters
    float rtd_num = 1;
    float heater_volt = 48;
    int heater_power_thresh = 150;
    int flag = 0;
  
  // PID Parameters
    double Input, Output, Setpoint;
    PID myPID(&Input, &Output, &Setpoint, 5, 0, 0, DIRECT);
    
    unsigned long serialTime; //this will help us know when to talk with processing
  
void setup() {

  // Initialize Serial Processing
    Serial.begin(9600);
  
  // Initialize Pin Modes
    pinMode(fan_pin, OUTPUT);
    //pinMode(heater_pin, OUTPUT);
    pinMode(RPWM_Output, OUTPUT);
    pinMode(LPWM_Output, OUTPUT);
  
  // Initialize PID
    Setpoint = 25;
    myPID.SetOutputLimits(-255, 255);
    myPID.SetMode(AUTOMATIC);
  
  // Initialize Temperature Board
    HTF.begin(MAX31865_4WIRE); // Setup for 4 wire RTD configuration
}

void loop() {

  // Read Temperature from RTDs
    double temp_HTF = HTF.temperature(RNOMINAL, RREF);

  // Calculating Power Usage
    double rtd_res = 0.385*temp_HTF + 100; // Increase in resistance due to temperature change
    double rtd_EquivRes = 1/(1/rtd_res*rtd_num); // Equivalent Resistance with number of RTDs in parallel
    double heater_power = pow(heater_volt, 2)/rtd_EquivRes; // Heater power considering resistance change

  // Check for overpower
    if (heater_power >= heater_power_thresh){
      analogWrite(RPWM_Output, 0);
      analogWrite(LPWM_Output, 0);
    }
  
  // Check for hurting people
   if (temp_HTF > 42) {
      analogWrite(RPWM_Output, 0);
      analogWrite(LPWM_Output, 0);
   }

  // Send-receive with processing if it's time
    if(millis()>serialTime){
      SerialReceive();
      SerialSend();
      serialTime+=500;
    }

      Input = temp_HTF;
      myPID.Compute();

  // If PID output is <0 to cool down, turn off heater
    if (Output < 0) {
      analogWrite(RPWM_Output, 0);
      analogWrite(LPWM_Output, abs(Output));
    }
    else {
      // PID Actuation
        analogWrite(RPWM_Output, abs(Output));
        analogWrite(LPWM_Output, 0);
    }
}

/********************************************
 * Serial Communication functions / helpers
 ********************************************/


union {                // This Data structure lets
  byte asBytes[24];    // us take the byte array
  float asFloat[6];    // sent from processing and
}                      // easily convert it to a
foo;                   // float array



// getting float values from processing into the arduino
// was no small task.  the way this program does it is
// as follows:
//  * a float takes up 4 bytes.  in processing, convert
//    the array of floats we want to send, into an array
//    of bytes.
//  * send the bytes to the arduino
//  * use a data structure known as a union to convert
//    the array of bytes back into an array of floats

//  the bytes coming from the arduino follow the following
//  format:
//  0: 0=Manual, 1=Auto, else = ? error ?
//  1: 0=Direct, 1=Reverse, else = ? error ?
//  2-5: float setpoint
//  6-9: float input
//  10-13: float output  
//  14-17: float P_Param
//  18-21: float I_Param
//  22-245: float D_Param
void SerialReceive()
{

  // read the bytes sent from Processing
  int index=0;
  byte Auto_Man = -1;
  byte Direct_Reverse = -1;
  while(Serial.available()&&index<26)
  {
    if(index==0) Auto_Man = Serial.read();
    else if(index==1) Direct_Reverse = Serial.read();
    else foo.asBytes[index-2] = Serial.read();
    index++;
  } 
  
  // if the information we got was in the correct format, 
  // read it into the system
  if(index==26  && (Auto_Man==0 || Auto_Man==1)&& (Direct_Reverse==0 || Direct_Reverse==1))
  {
    Setpoint=double(foo.asFloat[0]);
    //Input=double(foo.asFloat[1]);       // * the user has the ability to send the 
                                          //   value of "Input"  in most cases (as 
                                          //   in this one) this is not needed.
    if(Auto_Man==0)                       // * only change the output if we are in 
    {                                     //   manual mode.  otherwise we'll get an
      Output=double(foo.asFloat[2]);      //   output blip, then the controller will 
    }                                     //   overwrite.
    
    double p, i, d;                       // * read in and set the controller tunings
    p = double(foo.asFloat[3]);           //
    i = double(foo.asFloat[4]);           //
    d = double(foo.asFloat[5]);           //
    myPID.SetTunings(p, i, d);            //
    
    if(Auto_Man==0) myPID.SetMode(MANUAL);// * set the controller mode
    else myPID.SetMode(AUTOMATIC);             //
    
    if(Direct_Reverse==0) myPID.SetControllerDirection(DIRECT);// * set the controller Direction
    else myPID.SetControllerDirection(REVERSE);

  }
  Serial.flush();                         // * clear any random data from the serial buffer
}

// unlike our tiny microprocessor, the processing ap
// has no problem converting strings into floats, so
// we can just send strings.  much easier than getting
// floats from processing to here no?
void SerialSend()
{
  Serial.print("PID ");
  Serial.print(Setpoint);   
  Serial.print(" ");
  Serial.print(Input);   
  Serial.print(" ");
  Serial.print(abs(Output));   
  Serial.print(" ");
  Serial.print(myPID.GetKp());   
  Serial.print(" ");
  Serial.print(myPID.GetKi());   
  Serial.print(" ");
  Serial.print(myPID.GetKd());   
  Serial.print(" ");
  if(myPID.GetMode()==AUTOMATIC) Serial.print("Automatic");
  else Serial.print("Manual");  
  Serial.print(" ");
  if(myPID.GetDirection()==DIRECT) Serial.println("Direct");
  else Serial.println("Reverse");
}
