#define PIN_RST 7
#define PIN_RDY 6

#define ADDR_APP 0x74
#define ADDR_BOOT 0x74^0x40

#include <Mouse.h>
#include <Wire.h>
#include <stdint.h>

void setup()
{
  Serial.begin(57600);

  Serial.println("setup");
  
  pinMode(PIN_RST, OUTPUT);
  pinMode(PIN_RDY, INPUT);
  
  digitalWrite(PIN_RST, LOW);
  delay(200);
  digitalWrite(PIN_RST, HIGH);
  delay(200);
  
  Wire.begin();
  Mouse.begin();
  
  while (!digitalRead(PIN_RDY)) {
    delay(1);
  }
  
#if 1
  // config
  Wire.beginTransmission(ADDR_APP);
  Wire.write(0x10);
  Wire.write((1<<7)|(0<<6)|(1<<5)|(0<<4)|(0<<3)|(0<<2)|(0<<1)|0); //10100000
  Wire.write((1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)|(0<<2)|(0<<1)|0); //11111000 
  Wire.endTransmission(false);
#endif  
  while (!digitalRead(PIN_RDY)) {
    delay(1);
  }
  
  // channel setup
  Wire.beginTransmission(ADDR_APP);
  Wire.write(0x15);
  // disable tx channel 10 and rx channels 14,15 as they contain noise 
  Wire.write(9);
  Wire.write(13);
  Wire.write(9);
  Wire.write(13);
  Wire.write(0x40);
  Wire.write(0x7f);
  Wire.write(0xff);
  Wire.endTransmission(false);

  //Serial.print("setup");
}

byte addr = 1;
byte buf[80];
int16_t lastx = 0, lasty = 0;
byte ignore_next = 1;

void loop()
{
  int rdy = digitalRead(PIN_RDY);
  //int rdy = true;

  //Serial.println("NO FORMAT");
  
  if (rdy) {
    Serial.println("ready");
    Wire.beginTransmission(ADDR_APP);
    Wire.write(byte(addr));
    Wire.endTransmission(false);
    Wire.requestFrom(ADDR_APP,12);
  
    int i=0;
    while (Wire.available())
    {
      int f = Wire.read();
      //Serial.print(f);
      //Serial.print('\t');
      buf[i++] = f;
      if (i>=36) break;
    }
    //Serial.print("\r\n");
    
    int16_t xpos = ((uint16_t)buf[4]<<8)|((uint16_t)buf[5]);
    int16_t ypos = ((uint16_t)buf[2]<<8)|((uint16_t)buf[3]);
    
    Serial.print(xpos);
    Serial.print('\t');
    Serial.print(ypos);
    Serial.print("\r\n");
    
    if (xpos>0 && ypos>0) {
      //if (xpos>=7) xpos-=6;
      //else if (xpos<7) xpos=-xpos;
      int16_t dx = xpos-lastx;
      int16_t dy = -(ypos-lasty);
      
      dx/=3;
      dy/=2;
      
      int sgnx=1;
      if (dx<0) sgnx=-1;
      int sgny=1;
      if (dy<0) sgny=-1;
      
      //dx*=(dx/2)*sgnx;
      //dy*=(dy/2)*sgny;
      
      if (!ignore_next) {
        if (dx || dy) {
          Mouse.move(dx,dy);
        }
      }
      ignore_next = 0;
      lastx = xpos;
      lasty = ypos;
    } else {
      ignore_next = 1;
    }
  }
}