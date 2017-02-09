/********************************************************************************************************
* Arduino Control Brushed ESC
*
* Description
* program for controlling Brushed ESC
*
* modified 09 Feb 2017
* by TungstenEXE
*
* If you find my code useful, do support me by subscribing my YouTube Channel, thanks.
*
* My YouTube Channel Link - Nerf related
* https://www.youtube.com/channel/UCDMDwz4Hf8E0R0khyh40SxQ
* 
* ESC used     - Turnigy 20A BRUSHED ESC
********************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce-Arduino-Wiring
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Bounce2.h>
#include <Servo.h>

#define PIN_PMETER          A5    // PIN Potentiometer
#define PIN_PUSHERESC       3     // PIN to control ESC, normally the white wire from ESC 
#define PIN_BTN             7     // PIN for button to On/Off reading from Potentiometer

/*///////////////////////////////////////////////////////////////////////////////////////////////////////
 * The following are the setting for the ESC used, for other ESC, just chnage setting according to
 * ESC Spec
 *///////////////////////////////////////////////////////////////////////////////////////////////////////
#define THROTTLE_MIN        1000
#define THROTTLE_MAX        2000
#define THROTTLE_BAKE       1000
// End of ESC setting ///////////////////////////////////////////////////////////////////////////////////

int     throttle      = THROTTLE_MIN; // default throttle
boolean startReading  = false;        // default to not reading 

Servo   pusherESC;                    
Bounce  btnStart      = Bounce(); 

void setup() {
  Serial.begin(9600);
  Serial.println("Started Setup........");

  pinMode(PIN_BTN, INPUT_PULLUP);   // Using Internal PULLUP resistor
  btnStart.attach(PIN_BTN);
  btnStart.interval(5);
  
  pusherESC.attach(PIN_PUSHERESC);
  pusherESC.writeMicroseconds(THROTTLE_MIN);
  Serial.println("Arming........");   // just some display message 
  delay(2000);
  Serial.println("Arming........After delay");  

  // when running arduino without computer, after seeing the onboard LED goes on and then off
  // I will know the setup had completed and the arduino is now in the loop 
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  btnStart.update(); // update the button status

  if (btnStart.fell()) { // button pressed
    if (!startReading) {   // if not in reading mode
      startReading = true; // change to reading mode
      digitalWrite(LED_BUILTIN, HIGH); // on the onboard LED, for visual indicator  
      Serial.println("Start Sensor Read ----- ");
    } else { // currently in reading mode, should stop reading and brake DC brushed motor
      pusherESC.writeMicroseconds(THROTTLE_BAKE); // brake DC brushed motor
      startReading = false; // change to stop reading mode
      throttle = THROTTLE_BAKE;
      digitalWrite(LED_BUILTIN, LOW); // off the onboard LED, for visual indicator
      Serial.println("Stop Sensor Read ----- ");
      Serial.println("Bake ");
    }
  }

  if (startReading) { // in reading mode
    int sensorValue = analogRead(PIN_PMETER); // update reading from Potentiometer

    int newThrottle  = map(sensorValue, 0, 1023, THROTTLE_MIN, THROTTLE_MAX); 
    int throttleDiff = abs(newThrottle - throttle);

    if (throttleDiff > 5) { // update ESC only when the reading differ by 5
      throttle = newThrottle;
      Serial.print("Sensor Read : ");
      Serial.print(sensorValue);
    
      Serial.print("Throttle Value : ");
      Serial.println(throttle);
      pusherESC.writeMicroseconds(throttle); // update ESC
    }
  }
}
