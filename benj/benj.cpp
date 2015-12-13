#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <RH_NRF24.h>
#include <Adafruit_NeoPixel.h>
#include <avr/pgmspace.h>

#define PIXELPIN 7
#define REDLEDPIN 9

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 2

#define BUTTON1_PIN 2
#define BUTTON2_PIN 3

Adafruit_NeoPixel pixels = Adafruit_NeoPixel (NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);

typedef enum state_machine
{
	STATE_FIRST = 0,
	STATE_SECOND,
	STATE_THIRD,
	STATE_FOUR,
	STATE_ABORT,
	STATE_MAX,
} state_machine_t;

state_machine_t sm = STATE_FIRST;

typedef enum led_mask
{
	LED0 = 0x1,
	LED1 = 0x2,
	LED_ALL = 0x3
} led_mask_t;

typedef enum button_mask
{
	BUTTON0 = 0x1,
	BUTTON1 = 0x2,
	BUTTON_ALL = 0x3
} button_mask_t;

int
led_colors(led_mask_t lm, unsigned char r, unsigned char g, unsigned char b ) {
	for (int i = 0; i < NUMPIXELS; i++)
	{
		if ((1<<i)&lm)
			pixels.setPixelColor (i, pixels.Color (r, g, b));
		else
			pixels.setPixelColor (i, pixels.Color (0, 0, 0));

	}
	pixels.show ();
	return 0;
}
int

led_colors_keep_others(led_mask_t lm, unsigned char r, unsigned char g, unsigned char b ) {
	for (int i = 0; i < NUMPIXELS; i++)
	{
		if ((1<<i)&lm)
			pixels.setPixelColor (i, pixels.Color (r, g, b));
	}
	pixels.show ();
	return 0;
}

char Read_Button_PB(int debounce, char * exiting, button_mask_t mask)
{
	int Button1, Button2;
	Button1 = digitalRead (BUTTON1_PIN);
	Button2 = digitalRead (BUTTON2_PIN);
	//Serial.println (Button1,HEX);
	//Serial.println (Button2,HEX);

	if ((Button1 == 0) && ((mask&BUTTON0) == BUTTON0) && exiting && (*exiting == 0)) {
		*exiting = 1;
		return BUTTON0;
	}

	if ((Button2 == 0) && ((mask&BUTTON1) == BUTTON1) && exiting && (*exiting == 0)) {
		*exiting = 1;
		return BUTTON1;
	}

	if ((((Button1 == 0)&&(mask&BUTTON0)) || ((Button2 == 0)&&(mask&BUTTON1))) && exiting && (*exiting == 0)) {
		*exiting = 1;
		return (((Button1 == 0) ? BUTTON0 : 0) | ((Button2 == 0) ? BUTTON1 : 0));
	}
	delay(debounce);

	return (((Button1 == 0) ? BUTTON0 : 0) | ((Button2 == 0) ? BUTTON1 : 0));
}

void setup() {

	Serial.begin (115200);	// Initialisation du port série pour avoir un retour sur le serial monitor
	pinMode (REDLEDPIN, OUTPUT);
	analogWrite(REDLEDPIN, 0);
	Serial.println ("RGB LED initialization");
	pixels.begin ();		// This initializes the NeoPixel library.
	led_colors(LED_ALL, 0, 0, 0);
	delay(100);
	led_colors(LED_ALL, 255, 0, 0);
	delay(100);
	led_colors(LED_ALL, 0, 255, 0);
	delay(100);
	led_colors(LED_ALL, 0, 0, 255);
	delay(100);
	led_colors(LED0, 0, 0, 0);
	delay(100);
	led_colors(LED0, 255, 0, 0);
	delay(100);
	led_colors(LED0, 0, 255, 0);
	delay(100);
	led_colors(LED0, 0, 0, 255);
	delay(100);
	led_colors(LED1, 0, 0, 0);
	delay(100);
	led_colors(LED1, 255, 0, 0);
	delay(100);
	led_colors(LED1, 0, 255, 0);
	delay(100);
	led_colors(LED1, 0, 0, 255);
	delay(100);
	led_colors(LED_ALL, 0, 0, 0);
}

void loop() {

	char exiting;
	uint8_t i,j,k;

	exiting = 0;

	/*****************/
	/* TRANSMIT EMIT */
	/*****************/
	if (sm == STATE_FIRST)
	{
		Serial.print ("STATE_MACHINE_FIRST\n");
		analogWrite(REDLEDPIN, 255);
		while (!exiting) {
			Read_Button_PB(100, &exiting, BUTTON0);
			if (exiting == 1) {
				sm = STATE_SECOND;
				delay (1000);
				break;
			}
			led_colors_keep_others(LED0, (i++%16)<<4 , (j++%14)<<4, (k++%12)<<4);
			led_colors_keep_others(LED1, (j%14)<<4 , (k%12)<<4, (i%16)<<4);
		}
	}
	else if (sm == STATE_SECOND)
	{
		Serial.print ("STATE_MACHINE_SECOND\n");
		analogWrite(REDLEDPIN, 64);
		while (!exiting) {
			Read_Button_PB(100, &exiting, BUTTON0);
			if (exiting == 1) {
				sm = STATE_THIRD;
				delay (1000);
				break;
			}
			led_colors_keep_others(LED0, (i++%16)<<1 , (j++%14)<<1, (k++%12)<<1);
			led_colors_keep_others(LED1, (j%14)<<1 , (k%12)<<1, (i%16)<<1);
		}
	}
	else if (sm == STATE_THIRD)
	{
		Serial.print ("STATE_MACHINE_THIRD\n");
		while (!exiting) {
			Read_Button_PB(100, &exiting, BUTTON0);
			if (exiting == 1) {
				sm = STATE_FOUR;
				delay (1000);
				break;
			}
			led_colors_keep_others(LED_ALL, ((i++)%12), ((j++ + i)%14), ((k++ + i + j)%20));
			if (i%64 > 32)
				analogWrite(REDLEDPIN, 255);
			else
				analogWrite(REDLEDPIN, 0);
		}
	}
	else if (sm == STATE_FOUR)
	{
		int8_t z = 20;
		Serial.print ("STATE_MACHINE_THIRD\n");
		while (!exiting) {
			Read_Button_PB(100, &exiting, BUTTON0);
			if (exiting == 1) {
				sm = STATE_FIRST;
				delay (1000);
				break;
			}
			if (i++>250)
				led_colors_keep_others(LED_ALL, 16, 0, 32);
			else
				led_colors_keep_others(LED_ALL, 0, 0, 0);

			analogWrite(REDLEDPIN, j);
			j += z;
			if (j >= 235)
				z = -20;
			if (j <= 20)
				z = 20;
		}
	}
	/*********/
	/* ABORT */
	/*********/
	else if (sm == STATE_ABORT)
	{
		Serial.print ("STATE_MACHINE_ABORT\n");
		delay (2000);
		sm = STATE_FIRST;
	}
	/***********/
	/* UNKNOWN */
	/***********/
	else
	{
		Serial.print ("STATE_MACHINE_ISSUE\n");
		delay (2000);
		sm = STATE_FIRST;
	}
}


