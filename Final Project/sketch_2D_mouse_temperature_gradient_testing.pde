// Processing code for this example

// Dimmer - sends bytes over a serial port

// by David A. Mellis

// This example code is in the public domain.

import processing.serial.*;
// Constants
int Y_AXIS = 1;
int X_AXIS = 2;
color b1, b2, c1, c2;
int scale = 4;
int xsize = 256*scale;
int ysize = 256*scale;
int r;
int g;
int b;

Serial port;
void settings() {
  size(xsize, ysize);
}

void setup() {

  println("Available serial ports:");

  // if using Processing 2.1 or later, use Serial.printArray()

  println(Serial.list());

  // Uses the first port in this list (number 0). Change this to select the port

  // corresponding to your Arduino board. The last parameter (e.g. 9600) is the

  // speed of the communication. It has to correspond to the value passed to

  // Serial.begin() in your Arduino sketch.

  //port = new Serial(this, Serial.list()[0], 9600);

  // If you know the name of the port used by the Arduino board, you can specify

  // it directly like this.

  port = new Serial(this, "COM5", 115200);
  // Define colors
  b1 = color(255);
  b2 = color(0);
  c2 = color(204, 102, 0);
  c1 = color(0, 102, 153);
}

void draw() {

  // draw a gradient from black to white

  for (int i = 0; i < 2*xsize; i++) {


    float inter = map(i, 0, 2*width, 0, 1);
    color c = lerpColor(c1, c2, inter);
    stroke(c);
    if (i < xsize) {
      line(0, xsize-i, i, ysize);
    }
    else {
      line(i-xsize, 0, xsize, ysize-i+ysize);
    }
  
  }
  // write the current X-position of the mouse to the serial port as
  // a single byte
  int mX = mouseX;
  int mY = -1*mouseY+ysize;
  int mouseout = floor((mX*cos(PI/4)+mY*sin(PI/4))/scale);
  String str = mouseout + "," + mX + "," + mY;
  port.write(str); //sends mY to Arduino serial (can't have serial monitor open on Arduino)
  //String tmp = port.readString();
  //println(tmp);
  //print("Xpos: " + mX + ", "); //prints to processing console
  //println("Ypos: " + mY);
  println(port.read());
  //println(mouseout);
}


/* Max/MSP v5 patch for this example
 
 ----------begin_max5_patcher----------
 
 1008.3ocuXszaiaCD9r8uhA5rqAeHIa0aAMaAVf1S6hdoYQAsDiL6JQZHQ2M
 
 YWr+2KeX4vjnjXKKkKhhiGQ9MeyCNz+X9rnMp63sQvuB+MLa1OlOalSjUvrC
 
 ymEUytKuh05TKJWUWyk5nE9eSyuS6jesvHu4F4MxOuUzB6X57sPKWVzBLXiP
 
 xZtGj6q2vafaaT0.BzJfjj.p8ZPukazsQvpfcpFs8mXR3plh8BoBxURIOWyK
 
 rxspZ0YI.eTCEh5Vqp+wGtFXZMKe6CZc3yWZwTdCmYW.BBkdiby8v0r+ST.W
 
 sD9SdUkn8FYspPbqvnBNFtZWiUyLmleJWo0vuKzeuj2vpJLaWA7YiE7wREui
 
 FpDFDp1KcbAFcP5sJoVxp4NB5Jq40ougIDxJt1wo3GDZHiNocKhiIExx+owv
 
 AdOEAksDs.RRrOoww1Arc.9RvN2J9tamwjkcqknvAE0l+8WnjHqreNet8whK
 
 z6mukIK4d+Xknv3jstvJs8EirMMhxsZIusET25jXbX8xczIl5xPVxhPcTGFu
 
 xNDu9rXtUCg37g9Q8Yc+EuofIYmg8QdkPCrOnXsaHwYs3rWx9PGsO+pqueG2
 
 uNQBqWFh1X7qQG+3.VHcHrfO1nyR2TlqpTM9MDsLKNCQVz6KO.+Sfc5j1Ykj
 
 jzkn2jwNDRP7LVb3d9LtoWBAOnvB92Le6yRmZ4UF7YpQhiFi7A5Ka8zXhKdA
 
 4r9TRGG7V4COiSbAJKdXrWNhhF0hNUh7uBa4Mba0l7JUK+omjDMwkSn95Izr
 
 TOwkdp7W.oPRmNRQsiKeu4j3CkfVgt.NYPEYqMGvvJ48vIlPiyzrIuZskWIS
 
 xGJPcmPiWOfLodybH3wjPbMYwlbFIMNHPHFOtLBNaLSa9sGk1TxMzCX5KTa6
 
 WIH2ocxSdngM0QPqFRxyPHFsprrhGc9Gy9xoBjz0NWdR2yW9DUa2F85jG2v9
 
 FgTO4Q8qiC7fzzQNpmNpsY3BrYPVJBMJQ1uVmoItRhw9NrVGO3NMNzYZ+zS7
 
 3WTvTOnUydG5kHMKLqAOjTe7fN2bGSxOZDkMrBrGQ9J1gONBEy0k4gVo8qHc
 
 cxmfxVihWz6a3yqY9NazzUYkua9UnynadOtogW.JfsVGRVNEbWF8I+eHtcwJ
 
 +wLXqZeSdWLo+FQF6731Tva0BISKTx.cLwmgJsUTTvkg1YsnXmxDge.CDR7x
 
 D6YmX6fMznaF7kdczmJXwm.XSOOrdoHhNA7GMiZYLZZR.+4lconMaJP6JOZ8
 
 ftCs1YWHZI3o.sIXezX5ihMSuXzZtk3ai1mXRSczoCS32hAydeyXNEu5SHyS
 
 xqZqbd3ZLdera1iPqYxOm++v7SUSz
 
 -----------end_max5_patcher-----------
 
 */
