#include <Wire.h>
#include <Adafruit_Si4713.h> //radio module
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define _BV(n) (1 << n)
#define RESETPIN 5
//#define FMSTATION 10230      // 10230 == 102.30 MHz

//button pins and states for moving through the spectrum
const int buttonPinFwd = A1; //forward 
const int buttonPinRev = A0; //rev

int fmFreq = 10230;      // 10230 == 102.30 MHz
int buttonFwdState = 0;
int buttonRevState = 0; // variable for reading the pushbutton status

//data freq stringbuffer
char str[4];


//make new radio
Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

// Use dedicated hardware SPI pins for the oled
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  //start the serial connection
  Serial.begin(9600);
  Serial.println("radio serial");


  
  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    while (1);
  }

  //initialize the pushbutton pin as an input:
  pinMode(buttonPinFwd, INPUT_PULLUP);
  pinMode(buttonPinRev, INPUT_PULLUP);


  //init the display, its power and backlight
  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240); // Init ST7789 240x135
  tft.setRotation(3);

  //set background color
  tft.fillScreen(ST77XX_BLACK);


  tftDrawText("Booting up....",ST77XX_WHITE,2,67,2);
  

  // begin the RDS/RDBS transmission
  radio.beginRDS();
  radio.setRDSstation("SI-Radio");
  radio.setRDSbuffer("SI4713");
  tftDrawText("READY",ST77XX_WHITE,2,67,2);

  Serial.print("\nSet TX power");
  radio.setTXpower(115);  // dBuV, 88-115 max
  tftDrawText("READY",ST77XX_WHITE,2,67,2);
}

//text func
void tftDrawText(char *text, uint16_t color,uint8_t x,uint8_t y,uint8_t fontSize) {
  //reset screen
  //set background color
  tft.fillScreen(ST77XX_BLACK);
  
  tft.setCursor(x, y);
  tft.setTextSize(fontSize);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}


void loop() {

  // read the state of the pushbuttons values
  buttonFwdState = digitalRead(buttonPinFwd);
  buttonRevState = digitalRead(buttonPinRev);

  /*
  Serial.print("\nCurrent Frequency: "); 
  Serial.print(fmFreq/100); 
  Serial.print('.'); 
  Serial.println(fmFreq % 100);

  Serial.println("button 1");
  Serial.println(buttonFwdState);
  Serial.println("button 2");
  Serial.println(buttonRevState);
  */
  
  // check if the pushbutton is pressed.
  //+10 FREQ
  if (buttonFwdState == LOW) {
      
    Serial.println("PRESSSED button 1");
    if (fmFreq <= 10780){
      fmFreq = fmFreq + 20;
    }

    radio.setTXpower(115);  // dBuV, 88-115 max
    
    Serial.print("\nTuning into "); 
    Serial.print(fmFreq/100); 
    Serial.print('.'); 
    Serial.println(fmFreq % 100);
    radio.tuneFM(fmFreq);

    sprintf(str, "Freq: %d.%d Mhz", fmFreq/100,fmFreq%100);

    tftDrawText(str,ST77XX_WHITE,2,67,2);

    // begin the RDS/RDBS transmission
    radio.beginRDS();
    radio.setRDSstation("SI-Radio");
    radio.setRDSbuffer("SI4713");
    delay(100);

    
  }

  //-20 FREQ
  if (buttonRevState == LOW) {
    Serial.println("PRESSSED button 2");
    if (fmFreq >= 8820){
      fmFreq = fmFreq - 20;
    }

    radio.setTXpower(115);  // dBuV, 88-115 max
   
    Serial.print("\nTuning into "); 
    Serial.print(fmFreq/100); 
    Serial.print('.'); 
    Serial.println(fmFreq % 100);
    radio.tuneFM(fmFreq);

    sprintf(str, "Freq: %d.%d Mhz", fmFreq/100,fmFreq%100);
    
    tftDrawText(str,ST77XX_WHITE,2,67,2);

    // begin the RDS/RDBS transmission
    radio.beginRDS();
    radio.setRDSstation("SI-Radio");
    radio.setRDSbuffer("SI4713");
    delay(100);

  }

}
