#include <Arduino.h>
#include <WS2812B.h>
#include <EEPROM.h>

#define NUM_LEDS 30
#define LED_PIN PC13

void serial1Event();
void DisplayConfig(void);
void colorWipe(uint32_t c, uint8_t wait);
void freshColor(void);
void DisplayPages(uint32 endIndex);
void DisplayPagesEnd(uint32 endIndex);
void DisplayHex(uint16 value);


String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
char getString[5] = "";
String lastInputString = "";

uint16 RedAddress = 0x10;
uint16 GreenAddress = 0x11;
uint16 BlueAddress = 0x12;
uint16_t RedValue;
uint16_t GreenValue;
uint16_t BlueValue;
uint16 readData;
uint16 Status;
uint8_t RedByte, GreenByte, BlueByte;

uint RedMillis = 0;
uint GreenMillis = 0;
uint BlueMillis = 0;
uint freshMillis = 0;
uint sendMillis = 0;
uint ledMillis = 0;
bool RedWriteFlag = false;
bool GreenWriteFlag = false;
bool BlueWriteFlag = false;
bool freshFlag = false;
bool sendFlag = false;
uint interval = 1000; 
uint freshdelay = 100;
uint sendDelay = 500;
uint ledDelay = 1000;



WS2812B strip = WS2812B(NUM_LEDS);


void setup() {
  // initialize serial:
  delay(10000);
  pinMode(LED_PIN,OUTPUT);
  Serial1.begin(115200);
  Serial.begin(115200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  DisplayConfig();

  Status = EEPROM.read(RedAddress,&readData);
  RedValue = readData;
  Serial.println("RedValue");
  Serial.println(RedValue);
  Status = EEPROM.read(GreenAddress,&readData);
  GreenValue = readData;
  Serial.println("GreenValue");
  Serial.println(GreenValue);
  Status = EEPROM.read(BlueAddress,&readData);
  BlueValue = readData;
  Serial.println("BlueValue");
  Serial.println(BlueValue);

  strip.begin();
  strip.show();

  RedByte = map(RedValue, 0, 100, 0, 255);
  BlueByte = map(BlueValue, 0, 100, 0, 255);
  GreenByte = map(GreenValue, 0, 100, 0, 255);
  freshColor();
  DisplayPages(0x40);
  DisplayPagesEnd(0x40);

}

void loop() {
  // print the string when a newline arrives:

  if((millis() - ledMillis) > ledDelay)
  {
    ledMillis = millis();
    digitalWrite(LED_PIN,!digitalRead(LED_PIN));
    
  }
  serial1Event();
  if (stringComplete) {
    
   if((inputString.charAt(0)=='R')&&(inputString.charAt(1)==':'))
   {
     int i = 2;
     RedValue = 0;
     RedMillis = millis();
     freshMillis = RedMillis;
     RedWriteFlag = true;
     freshFlag = true;
     
     
     while ((inputString.charAt(i) >= '0')&&(inputString.charAt(i) <= '9'))
     {
       RedValue = RedValue * 10 + (inputString.charAt(i) - '0');
       i++;
     }

     Serial.println("Bluetooth RedValue:");
     Serial.println(RedValue);
     freshColor();
     

     

   }
   else if((inputString.charAt(0)=='G')&&(inputString.charAt(1)==':'))
   {
     int i = 2;
     GreenValue = 0;
     GreenMillis = millis();
     GreenWriteFlag = true;
     freshMillis = GreenMillis;
     freshFlag = true;
     
     
     while ((inputString.charAt(i) >= '0')&&(inputString.charAt(i) <= '9'))
     {
       GreenValue = GreenValue * 10 + (inputString.charAt(i) - '0');
       i++;
     }

     Serial.println("Bluetooth GreenValue:");
     Serial.println(GreenValue);
     freshColor();

     
   }
   else if ((inputString.charAt(0)=='B')&&(inputString.charAt(1)==':'))
   {
     int i = 2;
     BlueValue = 0;
     BlueMillis = millis();
     BlueWriteFlag = true;
     freshMillis = BlueMillis;
     freshFlag = true;
     
     
     while ((inputString.charAt(i) >= '0')&&(inputString.charAt(i) <= '9'))
     {
       BlueValue = BlueValue * 10 + (inputString.charAt(i) - '0');
       i++;
     }

     Serial.println("Bluetooth BlueValue:");
     Serial.println(BlueValue);
     freshColor();

     
   }
   else if (inputString == "#flash\n")
   {
     DisplayPages(0x40);
     DisplayPagesEnd(0x40 );
   }
   else 
   {
     Serial.println(inputString);
   }
    
    lastInputString = inputString;
    inputString = "";
    stringComplete = false;
    sendMillis = millis();
    sendFlag = true;

    

  }

  if (((millis() - RedMillis) > interval)&&(RedWriteFlag == true)) {
    Status = EEPROM.write(RedAddress, RedValue);
    Status = EEPROM.read(RedAddress, &readData);
    Serial.println("Write RedValue to Flash:");
    Serial.println(RedValue);
    RedWriteFlag = false;
  }
  if (((millis() - GreenMillis) > interval)&&(GreenWriteFlag == true)) {
    Status = EEPROM.write(GreenAddress, GreenValue);
    Status = EEPROM.read(GreenAddress, &readData);
    Serial.println("Write GreenValue to Flash:");
    Serial.println(GreenValue);
    GreenWriteFlag = false;
  }
  if (((millis() - BlueMillis) > interval)&&(BlueWriteFlag == true)) {
    Status = EEPROM.write(BlueAddress, BlueValue);
    Status = EEPROM.read(BlueAddress, &readData);
    Serial.println("Write BlueValue to Flash:");
    Serial.println(BlueValue);
    BlueWriteFlag = false;



  }  

  if(((millis() - freshMillis) > freshdelay)&&freshFlag)
  {
    freshColor();
    freshFlag = false;

  }
  if(((millis() - sendMillis) > sendDelay)&&sendFlag)
  {
    Serial1.print(lastInputString);
    sendFlag = false;
    lastInputString = "";

  }

}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/

void freshColor(void) {
  RedByte = map(RedValue, 0, 100, 0, 255);
  BlueByte = map(BlueValue, 0, 100, 0, 255);
  GreenByte = map(GreenValue, 0, 100, 0, 255);
  Serial.println("BYTE");
  Serial.println(RedByte);
  Serial.println(GreenByte);
  Serial.println(BlueByte);

  colorWipe(strip.Color(RedByte, GreenByte, BlueByte),0);

}
void colorWipe(uint32_t c, uint8_t wait) 
{
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
      strip.setPixelColor(i, c);
      
  }
  strip.show();
  //delay(wait);

}
void serial1Event() {
  while (Serial1.available()) {
    // get the new byte:
    char inChar = (char)Serial1.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void DisplayConfig(void)
{
	Serial.print  ("EEPROM.PageBase0 : 0x");
	Serial.println(EEPROM.PageBase0, HEX);
	Serial.print  ("EEPROM.PageBase1 : 0x");
	Serial.println(EEPROM.PageBase1, HEX);
	Serial.print  ("EEPROM.PageSize  : 0x");
	Serial.print  (EEPROM.PageSize, HEX);
	Serial.print  (" (");
	Serial.print  (EEPROM.PageSize, DEC);
	Serial.println(")");
}

void DisplayPages(uint32 endIndex)
{
	Serial.println("Page 0     Top         Page 1");

	for (uint32 idx = 0; idx < endIndex; idx += 4)
	{
		Serial.print  (EEPROM.PageBase0 + idx, HEX);
		Serial.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx));
		Serial.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx + 2));
		Serial.print  ("    ");
		Serial.print  (EEPROM.PageBase1 + idx, HEX);
		Serial.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx));
		Serial.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx + 2));
		Serial.println();
	}
}

void DisplayPagesEnd(uint32 endIndex)
{
	Serial.println("Page 0     Bottom      Page 1");

	for (uint32 idx = EEPROM.PageSize - endIndex; idx < EEPROM.PageSize; idx += 4)
	{
		Serial.print  (EEPROM.PageBase0 + idx, HEX);
		Serial.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx));
		Serial.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx + 2));
		Serial.print  ("    ");
		Serial.print  (EEPROM.PageBase1 + idx, HEX);
		Serial.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx));
		Serial.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx + 2));
		Serial.println();
	}
}
void DisplayHex(uint16 value)
{
	if (value <= 0xF)
		Serial.print("000");
	else if (value <= 0xFF)
		Serial.print("00");
	else if (value <= 0xFFF)
		Serial.print("0");
	Serial.print(value, HEX);
}

