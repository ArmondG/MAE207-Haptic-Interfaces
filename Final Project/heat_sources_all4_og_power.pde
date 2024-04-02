
import processing.serial.*;

Serial port;
// Constants
int Y_AXIS = 1;
int X_AXIS = 2;
color b1, b2, c1, c2, c3;
int scale = 4;
int xsize = 256*scale;
int ysize = 256*scale;
float prepos = 0;
float pos = 0;
String inString;
boolean firstContact = false;
float mouseout = 0;
int virt_offset = 50;

//Point Sources
float innerDiam = 22;
float outerDiam = 200;
float detail = 10;
int centerX[] = {500, 200, 800, 200, 800};
int centerY[] = {-500 + ysize, -200+ ysize, -200+ ysize, -800+ ysize, -800+ ysize}; // circle if =centerX
float offset = (outerDiam * 0.5) / sqrt(2);


// Flags
int grad2D = 0;
int hotspot = 0;
int hotandcold =1;

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

  port = new Serial(this, "COM5", 250000);
  port.bufferUntil('\n');
  // Define colors
  b1 = color(255);
  b2 = color(0);
  c2 = color(255, 100, 0); //red hot
  c1 = color(0, 100, 255); //blue cold
  c3 = color(127);
  background(c3);
}

void draw() {
  if (grad2D == 1) { //flag
    // draw a gradient from blue to red

    for (int i = 0; i < 2*xsize; i++) {


      float inter = map(i, 0, 2*width, 0, 1);
      color c = lerpColor(c1, c2, inter);
      stroke(c);
      if (i < xsize) {
        line(0, xsize-i, i, ysize);
      } else {
        line(i-xsize, 0, xsize, ysize-i+ysize);
      }
    }
  }

 /////////////////////////////////////////////////////////////////////////////
  if (hotspot == 1) {

    for (int j = 0; j < centerX.length; j++) {
      for (float i = outerDiam; i >= innerDiam; i -= detail) {
        float step = map(i, innerDiam, outerDiam, 0, 1);

        color c = lerpColor(c2, c3, step);

        fill(c);
        stroke(c);
        ellipse(centerX[j], centerY[j], i, i);
      }
    }
  }
  /////////////////////////////////////////////////////////////////////////
  if (hotandcold == 1) {
     int numOfCold = 2;
    for (int j = 0; j < centerX.length - numOfCold; j++) {
      for (float i = outerDiam; i >= innerDiam; i -= detail) {
        float step = map(i, innerDiam, outerDiam, 0, 1);

        color c = lerpColor(c2, c3, step);

        fill(c);
        stroke(c);
        ellipse(centerX[j], centerY[j], i, i);
      }
    }
    for (int j = centerX.length - numOfCold; j < centerX.length; j++) {
      for (float i = outerDiam; i >= innerDiam; i -= detail) {
        float step = map(i, innerDiam, outerDiam, 0, 1);

        color c = lerpColor(c1, c3, step);

        fill(c);
        stroke(c);
        ellipse(centerX[j], centerY[j], i, i);
      }
    }
  }
  /////////////////////////////////////////////////////////////////////////////
}

void serialEvent(Serial port) {
  inString = port.readStringUntil('\n');
  if (inString != null) {

    //trim formatting characters
    inString = trim(inString);
    //println(inString);

    //look for or 'A' string to start handshake
    if (firstContact == false) {
      if (inString.equals("A")) {
        port.clear();
        firstContact = true;
        port.write("A");
        println("contact");
      }
    } else {
      println(inString);
      // write the current X-position of the mouse to the serial port as
      // a single byte
      int mX = mouseX;
      int mY = mouseY;
      int mY_rot = -1*mY+ysize; // reflects ycoord so that bottom left is origin (for printing only)
      float r1 = red(get(mouseX-virt_offset, mouseY-virt_offset)); // gets red pixel data only
      float r2 = red(get(mouseX-virt_offset, mouseY+virt_offset));
      float r3 = red(get(mouseX+virt_offset, mouseY-virt_offset));
      float r4 = red(get(mouseX+virt_offset, mouseY+virt_offset));
      float mouseout1 = 100*(2*r1/255 - 1);
      float mouseout2 = 100*(2*r2/255 - 1);
      float mouseout3 = 100*(2*r3/255 - 1);
      float mouseout4 = 100*(2*r4/255 - 1);
      
      String str = mouseout1 + "," + mouseout2 + "," + mouseout3 + "," + mouseout4 + "," +mX + "," + mY_rot + "\r\n";
      port.write(str); //sends str to Arduino serial (can't have serial monitor open on Arduino)
    }
  }
}
