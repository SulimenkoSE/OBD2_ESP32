/*
 Name:		ESP_32_TFT_18.ino
 Created:	31.03.2019 8:09:16
 Author:	Serhii
*/

//#include <BluetoothSerial.h>
//#include <BLEDevice.h>
//#include <EEPROM.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <gfxfont.h>
//BH1750
#include <ErriezBH1750.h>


//OBD2
#include "OBD2.h"

// set up pins we are going to use to talk to the screen
#define TFT_DC_A0     2       // register select (stands for Data Control perhaps!) A0
#define TFT_RST_RESET 4     // Display reset pin, you can also connect this to the ESP32 reset
							  // in which case, set this #define pin to -1!
#define TFT_CS   5            // Display enable (Chip select), if not enabled will not talk on SPI bus
#define TFT_MOSI_SCS  23
#define TFT_SCLK_SDA  18

// initialise the routine to talk to this display with these pin connections (as we've missed off
// TFT_SCLK and TFT_MOSI the routine presumes we are using hardware SPI and internally uses 13 and 11

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC_A0, TFT_MOSI_SCS, TFT_SCLK_SDA, TFT_RST_RESET);

extern "C" {
	uint8_t temprature_sens_read();
}

float p = 3.1415926;

const unsigned short HomeFrog[256] PROGMEM = {
0x0000, 0x0000, 0x0000, 0x26E0, 0x26E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x26E0, 0x26E0, 0x0000, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x0000, 0x26E0, 0x0000, 0x0000, 0x26E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x26E0, 0x0000, 0x0000, 0x26E0, 0x0000, 0x0000,   // 0x0020 (32) pixels
0x0000, 0x0000, 0x26E0, 0xFA20, 0xFA20, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0xFA20, 0xFA20, 0x26E0, 0x0000, 0x0000,   // 0x0030 (48) pixels
0x0000, 0x0000, 0x0000, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x0000, 0x0000, 0x0000,   // 0x0040 (64) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0050 (80) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x26E0, 0x26E0, 0xFA20, 0x26E0, 0x26E0, 0xFA20, 0x26E0, 0x26E0, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0060 (96) pixels
0x0000, 0x26E0, 0x0000, 0x0000, 0x26E0, 0x26E0, 0x26E0, 0xFA20, 0xFA20, 0x26E0, 0x26E0, 0x26E0, 0x0000, 0x0000, 0x26E0, 0x0000,   // 0x0070 (112) pixels
0x26E0, 0x26E0, 0x26E0, 0x0000, 0x06FE, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x06FE, 0x0000, 0x26E0, 0x26E0, 0x26E0,   // 0x0080 (128) pixels
0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x06FE, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x06FE, 0x26E0, 0x26E0, 0x26E0, 0x26E0,   // 0x0090 (144) pixels
0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x06FE, 0x06FE, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x06FE, 0x06FE, 0x26E0, 0x26E0, 0x26E0, 0x26E0,   // 0x00A0 (160) pixels
0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x06FE, 0x06FE, 0x06FE, 0x06FE, 0x06FE, 0x06FE, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x26E0,   // 0x00B0 (176) pixels
0x0000, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x06FE, 0xFA20, 0xFA20, 0xFA20, 0x06FE, 0x06FE, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x0000,   // 0x00C0 (192) pixels
0x0000, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x06FE, 0xFA20, 0xFA20, 0xFA20, 0x06FE, 0x06FE, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x0000,   // 0x00D0 (208) pixels
0x0000, 0x0000, 0x0000, 0x26E0, 0x26E0, 0x26E0, 0x06FE, 0x06FE, 0x06FE, 0x06FE, 0x26E0, 0x26E0, 0x26E0, 0x0000, 0x0000, 0x0000,   // 0x00E0 (224) pixels
0x0000, 0x0000, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x26E0, 0x26E0, 0x26E0, 0x26E0, 0x0000, 0x0000,   // 0x00F0 (240) pixels
0x0000, 0x26E0, 0x26E0, 0x26E0, 0x0000, 0x26E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x26E0, 0x0000, 0x26E0, 0x26E0, 0x26E0, 0x0000,   // 0x0100 (256) pixels
};

int y = 127;

 OBD2 K_Line;
 uint16_t FR_max = 0, FR_min = 999;
 uint8_t TE = 0, TP, RI, U_MAF, TE_M;
 float V;
 uint16_t RPM = 0, RPM_XX = 0,FR = 0, AF = 0, MAF = 0, IT = 0, Fuel_100 = 0, Speed = 0;
 uint8_t U = 0;
 uint8_t Opros_ = 0;
 uint8_t Light_ = 0;
 
 int Erors_ECU_Answer = 0;
 uint8_t OBDBuffer[256];

 int StatusFAN = 0;

 //Status LED (OnBoard)
#define BOARD_LED 2

// Settings BH1750
// ADDR line LOW/open:  I2C address 0x23 (0x46 including R/W bit) [default]
// ADDR line HIGH:      I2C address 0x5C (0xB8 including R/W bit)
BH1750 sensor(LOW);
uint16_t lux;

//"OBD2UART.h"
//OBD2 Serial2 10400
//COBD obd;
//volatile unsigned int StartSend=0;


#ifdef ESP32
extern HardwareSerial Serial2;
#endif

//Tiemrs
hw_timer_t* timer0 = NULL;
hw_timer_t* timer1 = NULL;

volatile uint8_t LedStatus = 0;
portMUX_TYPE timerMux0 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer0() {
	Opros_ = 1;
	endTimer();
	//StartTimer0();
}

void IRAM_ATTR onTimer1() {
	portENTER_CRITICAL_ISR(&timerMux0);
	Light_ = 1;
	portEXIT_CRITICAL_ISR(&timerMux0);
}
void StartTimer0() {
	timer0 = timerBegin(0, 80, true);
	timerAttachInterrupt(timer0, &onTimer0, true);
	timerAlarmWrite(timer0, 2000000, true); //250 ms
	timerAlarmEnable(timer0);
}
void StartTimer1() {
	timer1 = timerBegin(0, 80, true);
	timerAttachInterrupt(timer1, &onTimer1, true);
	timerAlarmWrite(timer1, 2000000, true); //2 s
	timerAlarmEnable(timer1);
}

void endTimer() {
	timerEnd(timer0);
	timer0 = NULL;
}

void endTimer1() {
	timerEnd(timer1);
	timer1 = NULL;
}

void PrintTextColor(int x, int y, int SizeText, char *text, uint16_t color, bool TextWrap = true) {
	tft.setCursor(x, y);
	tft.setTextColor(color);
	tft.setTextSize(SizeText);
	tft.setTextWrap(TextWrap);
	tft.println(text);
}

void Dagnostic_ECU() {
	// start communication with OBD-II UART adapter
	if (OBD2::Ecu_Status == ECU_Communication_OK)
	{
		K_Line.ReadDataByLocalIdentifier();
	}
	else
	{
		PrintTextColor(0, 70, 1, "START OBD-II...", ST7735_WHITE);
		K_Line.Start_diagnostic();
		if (OBD2::Ecu_Status == ECU_Communication_OK)
		{
			PrintTextColor(0, 70, 1, "START OBD-II...OK!!!", ST7735_WHITE);
			StartTimer1();
		}
	}
}

void setup(void) {
	//Serial2.begin(10400);
	Serial.begin(115200);
	
	while (!Serial) { ;	}

	Serial.println(F("BH1750 continues measurement high resolution example"));

	tft.initR(INITR_BLACKTAB);   // initialize a ST7735R chip,
	
	tft.setRotation(3);
	tft.fillScreen(ST7735_BLACK);
	PrintTextColor(0, 0, 1, "Display is OK!!!.", ST7735_WHITE);
	delay(1000);
	/*mediabuttons();
	tft.fillScreen(ST7735_BLACK);
	PrintTextColor(0, 0, 1, "Display is OK!!!.", ST7735_WHITE);
	delay(2000);*/

	//tft.fillScreen(ST7735_BLACK);
	//for (int x = 128; x >= 0; x -= 16)
	//	tft.drawRGBBitmap(x, y, HomeFrog, 16, 16);
	//y -= 2;
	//if (y < -16)
	//	y = 127;
	uint8_t tf = temprature_sens_read();
	float   tc = (tf - 32) / 1.8;
	tft.print("Core temperature is ");
	tft.println(tc, 2);
	PrintTextColor(0, 20, 1, "TEMPERATURE -         oC", ST7735_WHITE);
	PrintTextColor(0,30, 1,  "THROTTLE    -         %", ST7735_WHITE);
	PrintTextColor(0, 40, 1, "TURNS       -         RPM", ST7735_WHITE);
	PrintTextColor(0, 50, 1, "SPEED       -         km/h", ST7735_WHITE);
	PrintTextColor(0, 60, 1, "VOLTAGE     -         V", ST7735_WHITE);

	// define board status led
	pinMode(BOARD_LED, OUTPUT);
	// status led off
	digitalWrite(BOARD_LED, LOW);

	Wire.begin();
	// Initialize sensor in continues mode, high 0.5 lx resolution
	sensor.begin(ModeContinuous, ResolutionLow);
	// Start conversion
	sensor.startConversion();
	
	// start communication with OBD-II UART adapter
	Dagnostic_ECU();
}

void loop() {
	int value;
	char str_[10];
	//for (uint16_t rotation = 0; rotation < 4; rotation++)
	//	Demo(rotation);

	// Diagnistic with OBD-II UART adapter
	Dagnostic_ECU();
	

	//Работаем от показаний таймера 0
	if (Opros_ == 1)
	{
		Opros_ = 0;
		//K_Line.Communication_Init();
		sprintf(str_, "%i", TE);
		Serial.println(str_);
		PrintTextColor(100, 20, 1, str_, ST7735_WHITE);
		StartTimer0();
	}
	if (Light_ == 1)
	{
		Light_ = 0;
		// Read light without wait
		lux = sensor.read();
		tft.setCursor(0, 80);
		tft.println("           ");
		tft.setCursor(0, 80);
		tft.println(lux);
		//Serial.printf("Light:  %i LUX \n", lux);
		//endTimer1();
		
		if (Erors_ECU_Answer >= 5)
		{
			Serial.printf("Diagnostic start >>> Erors_ECU_Answer %i\n", Erors_ECU_Answer);
			Erors_ECU_Answer = 0;
			OBD2::Ecu_Status = ECU_Communication_NO;
		}
		tft.fillRoundRect(80, 20, 50, 50, 0, ST7735_BLACK);
		
		if (TE > 61 && StatusFAN == 0)
		{
			Serial.printf("On FAN >------------>  \n");
			StatusFAN = 1;
			//K_Line.Fan(StatusFAN);
			digitalWrite(BOARD_LED, HIGH);
		}
		if (TE < 60 && StatusFAN == 1)
		{
			Serial.printf("Off FAN >----------->  \n");
			StatusFAN = 0;
			//K_Line.Fan(StatusFAN);
			digitalWrite(BOARD_LED, LOW);
		}

		sprintf(str_, "%i", TE);
		PrintTextColor(100, 20, 1, str_, ST7735_WHITE);
		
		sprintf(str_, "%i", RPM);
		PrintTextColor(100, 40, 1, str_, ST7735_WHITE);
		
		sprintf(str_, "%i", Speed);
		PrintTextColor(100, 50, 1, str_, ST7735_WHITE);
		
		sprintf(str_, "%.1f", V);
		PrintTextColor(100, 60, 1, str_, ST7735_WHITE);

		sprintf(str_, "%.1f", U);
		PrintTextColor(100, 70, 1, str_, ST7735_WHITE);

		//StartTimer1();
	}
}

void Demo(uint16_t rotation)
{

	tft.setRotation(rotation);
	uint16_t time = millis();
	tft.fillScreen(ST7735_BLACK);
	time = millis() - time;

	delay(500);

	// large block of text
	tft.fillScreen(ST7735_BLACK);

	testdrawtext("This is a test of some very log text that I written for the sole purpose of checking if the display displays very long text correctly.", ST7735_WHITE);
	delay(1000);

	// tft print function!
	tftPrintTest();
	delay(4000);

	// a single pixel
	tft.drawPixel(tft.width() / 2, tft.height() / 2, ST7735_GREEN);
	delay(500);

	testdrawrects(ST7735_GREEN);
	delay(500);

	testfillrects(ST7735_YELLOW, ST7735_MAGENTA);
	delay(500);

	tft.fillScreen(ST7735_BLACK);
	testfillcircles(10, ST7735_BLUE);
	testdrawcircles(10, ST7735_WHITE);
	delay(500);

	testroundrects();
	delay(500);

	testtriangles();
	delay(1000);
	for (int i = 2; i > 0; i--)
	{
		tft.invertDisplay(true);
		delay(500);
		tft.invertDisplay(false);
		delay(500);
	}
}

void testdrawtext(char *text, uint16_t color) {
	tft.setCursor(0, 0);
	tft.setTextColor(color);
	tft.setTextWrap(true);
	tft.print(text);
}

void testdrawrects(uint16_t color) {
	tft.fillScreen(ST7735_BLACK);
	for (int16_t x = 0; x < tft.width(); x += 6) {
		tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color);
	}
}

void tftPrintTest() {
	tft.setTextWrap(false);
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(0, 0);
	tft.setTextColor(ST7735_WHITE);
	tft.setTextSize(1);
	tft.println("Hello World!");
	tft.setTextColor(ST7735_YELLOW);
	tft.setTextSize(2);
	tft.println("I'm running");
	tft.setTextColor(ST7735_GREEN);
	tft.setTextSize(3);
	tft.println("on a");
	tft.setTextColor(ST7735_BLUE);
	tft.println("ESP32");
	tft.setTextSize(4);
	tft.setTextColor(ST7735_CYAN);
	tft.print("Ha ha!");
	delay(1500);
	tft.setCursor(0, 0);
	tft.fillScreen(ST7735_BLACK);
	tft.setTextColor(ST7735_WHITE);
	tft.setTextSize(0);
	tft.println("Hello World!");
	tft.setTextSize(1);
	tft.setTextColor(ST7735_GREEN);
	tft.print(p, 6);
	tft.println(" Want pi?");
	tft.println(" ");
	tft.print(8675309, HEX); // print 8,675,309 out in HEX!
	tft.println(" Print HEX!");
	tft.println(" ");
	tft.setTextColor(ST7735_WHITE);
	tft.println("Sketch has been");
	tft.println("running for: ");
	tft.setTextColor(ST7735_MAGENTA);
	tft.print(millis() / 1000);
	tft.setTextColor(ST7735_WHITE);
	tft.print(" seconds.");
}

void mediabuttons() {
	// play
	tft.fillScreen(ST7735_BLACK);
	tft.fillRoundRect(25, 10, 78, 60, 8, ST7735_WHITE);
	tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_RED);
	delay(500);
	// pause
	tft.fillRoundRect(25, 90, 78, 60, 8, ST7735_WHITE);
	tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_GREEN);
	tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_GREEN);
	delay(500);
	// play color
	tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_BLUE);
	delay(50);
	// pause color
	tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_RED);
	tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_RED);
	// play color
	tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_GREEN);
}
void testfillrects(uint16_t color1, uint16_t color2) {
	tft.fillScreen(ST7735_BLACK);
	for (int16_t x = tft.width() - 1; x > 6; x -= 6) {
		tft.fillRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color1);
		tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color2);
	}
}
void testfillcircles(uint8_t radius, uint16_t color) {
	for (int16_t x = radius; x < tft.width(); x += radius * 2) {
		for (int16_t y = radius; y < tft.height(); y += radius * 2) {
			tft.fillCircle(x, y, radius, color);
		}
	}
}

void testdrawcircles(uint8_t radius, uint16_t color) {
	for (int16_t x = 0; x < tft.width() + radius; x += radius * 2) {
		for (int16_t y = 0; y < tft.height() + radius; y += radius * 2) {
			tft.drawCircle(x, y, radius, color);
		}
	}
}

void testtriangles() {
	tft.fillScreen(ST7735_BLACK);
	int color = 0xF800;
	int t;
	int w = tft.width() / 2;
	int x = tft.height() - 1;
	int y = 0;
	int z = tft.width();
	for (t = 0; t <= 15; t++) {
		tft.drawTriangle(w, y, y, x, z, x, color);
		x -= 4;
		y += 4;
		z -= 4;
		color += 100;
	}
}

void testroundrects() {
	tft.fillScreen(ST7735_BLACK);
	int color = 100;
	int i;
	int t;
	for (t = 0; t <= 4; t += 1) {
		int x = 0;
		int y = 0;
		int w = tft.width() - 2;
		int h = tft.height() - 2;
		for (i = 0; i <= 16; i += 1) {
			tft.drawRoundRect(x, y, w, h, 5, color);
			x += 2;
			y += 3;
			w -= 4;
			h -= 6;
			color += 1100;
		}
		color += 100;
	}
}

