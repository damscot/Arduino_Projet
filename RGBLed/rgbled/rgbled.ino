/*
 * LilyPad tutorial: color (RGB LEDs)
 *
 * Uses a LilyPad RGB LED module to play with
  * dynamic colored light. See this web color chart
  * for the (Red,Green,Blue) values of different colors:
 * http://www.visibone.com/colorlab/
 */
 
int lightPin = 1;       // define a pin for Photo resistor  
int redPin = 3; 	// R petal on RGB LED module connected to digital pin 11
int greenPin = 5; 	// G petal on RGB LED module connected to digital pin 9
int bluePin = 6; 	// B petal on RGB LED module connected to digital pin 10
  	 
void setup() 	 
{ 	 
         Serial.begin(115200);  //Begin serial communcation
         pinMode(redPin, OUTPUT); 	// sets the redPin to be an output
         pinMode(greenPin, OUTPUT); 	// sets the greenPin to be an output
         pinMode(bluePin, OUTPUT); 	// sets the bluePin to be an output
} 	 
  	 
void loop() 	// run over and over again
{
          int r = 0, g = 0 ,b = 0;
          
          // Basic colors:
          color(255, 0, 0); 	// turn the RGB LED red
          delay(20); 	// delay for 1 second
          Serial.println(analogRead(lightPin)); //Write the value of the photoresistor
          color(0, 255, 0); 	// turn the RGB LED green
          delay(20); 	// delay for 1 second
          Serial.println(analogRead(lightPin)); //Write the value of the photoresistor
          color(0, 0, 255); 	// turn the RGB LED blue
          delay(20); 	// delay for 1 second
          Serial.println(analogRead(lightPin)); //Write the value of the photoresistor
          color(0,0,0); 	// turn the RGB LED off
          delay(20); 	// delay for 1 second
          Serial.println(analogRead(lightPin)); //Write the value of the photoresistor
 
 #if 0 
          // Example blended colors:
          color(255, 255, 0); 	// turn the RGB LED yellow
          delay(500); 	// delay for 1 second
          color(255, 255, 255); 	// turn the RGB LED white
          delay(500); 	// delay for 1 second
          color(255, 0, 255); 	// turn the RGB LED purple
          delay(500); 	// delay for 1 second
          color(0, 255, 255); 	// turn the RGB LED cyan
          delay(500); 	// delay for 1 second
          color(0, 0, 0); 	// turn the RGB LED off
          delay(500); 	// delay for 1 second

          for (r = 0; r < 8; r++)
          {
          for (g = 0; g < 8; g++)
          {
          for (b = 0; b < 8; b++)
          {            
                  color(r*32, g*32, b*32); // turn the RGB LED off
                  delay(50); 	// delay for 10 ms
          }
          }
          }
          
          for (b = 8; b > 0; b--)
          {
          for (g = 8; g > 0; g--)
          {
          for (r = 8; r > 0; r--)
          {            
                  color(r*32, g*32, b*32); // turn the RGB LED off
                  delay(50); 	// delay for 10 ms
          }
          }
          }
#endif       
} 	 
  	 
void color (unsigned char red, unsigned char green, unsigned char blue)     // the color generating function
{ 	 
          analogWrite(redPin, 255-red); 	 
          analogWrite(bluePin, 255-blue);
          analogWrite(greenPin, 255-green);
} 
