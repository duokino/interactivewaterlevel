// This project will use HC-SR04 as the water level sensor to detect
// the water level. The default setting is 30cm from the land surface.
// The data will be transmit to TelegramBot by request using command /getdata
// but the alert will be send automatically.
//
// Additionally it can also turn ON and OFF the Light using /lighton and /lightoff
// Be sure to connect the ESP32 to the local WiFi signal
//
// Pin for HC-SR04 to ESP32
// Vcc  -> VIN
// Trig -> GPIO 13
// Echo -> GPIO 12
// Gnd  -> GND


#include <Arduino.h>
#include <NewPing.h>
#include <CTBot.h>


CTBot myBot;

String ssid = "mySSID";     // REPLACE mySSID WITH YOUR WIFI SSID
String pass = "myPassword"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "myToken";   // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN
// for group ID, find in the page and add -100 at the front group ID


uint8_t led = 2;            // the onboard ESP32 LED.

// Ultrasonic sensor pins (adjust these based on your wiring)
#define TRIGGER_PIN 13 // GPIO 13 for trigger
#define ECHO_PIN    12 // GPIO 12 for echo
#define MAX_DISTANCE 100 // Maximum distance (in centimeters) to measure

// Positioning the water level sensor
#define mintank 30          //lowest level  - 30cm tank water level from sensor
#define maxtank 0           //highest level - 0cm tank water level from sensor
#define alert_threshold 10  // Water level threshold for sending an alert

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  // set the pin connected to the LED to act as output pin
	pinMode(led, OUTPUT);
	digitalWrite(led, LOW); // turn off the led (inverted logic!)

  Serial.begin(115200);

  Serial.println("Starting TelegramBot...");

	// connect the ESP32 to the desired access point
	myBot.wifiConnect(ssid, pass);

	// set the telegram bot token
	myBot.setTelegramToken(token);

	// check if all things are ok
	if (myBot.testConnection()) {
		Serial.println("\ntestConnection OK");
    digitalWrite(led, HIGH);
    delay(200);
    digitalWrite(led, LOW);
    delay(200);
    digitalWrite(led, HIGH);
    delay(200);
    digitalWrite(led, LOW);
  }
	else
		Serial.println("\ntestConnection NOK");

}


void loop() {
  int waterlevel = readWaterLevel();

	// a variable to store telegram message data
	TBMessage msg;

	// if there is an incoming message...
	if (CTBotMessageText == myBot.getNewMessage(msg)) {

		if (msg.text.equalsIgnoreCase("/lighton")) {              // if the received message is "LIGHT ON"...
			digitalWrite(led, HIGH);                               // turn on the LED (inverted logic!)
			myBot.sendMessage(msg.sender.id, "Light is now ON");  // notify the sender
		}
		else if (msg.text.equalsIgnoreCase("/lightoff")) {        // if the received message is "LIGHT OFF"...
			digitalWrite(led, LOW);                              // turn off the led (inverted logic!)
			myBot.sendMessage(msg.sender.id, "Light is now OFF"); // notify the sender
		}
    else if (msg.text.equalsIgnoreCase("/getdata")) {        // if the received message is "LIGHT OFF"...
			String waterlevelmessage = "Water Level: " + String(waterlevel) + " cm";
      myBot.sendMessage(msg.sender.id, waterlevelmessage);  // notify the sender
		}
		else {                                                    // otherwise...
			// generate the message for the sender
			String reply;
			reply = (String)"Welcome " + msg.sender.username + (String)". Please click Menu button below.";
			myBot.sendMessage(msg.sender.id, reply);             // and send it
		}
	}
  	// wait 1000 milliseconds
	delay(1000);


  Serial.print("Water level: ");
  Serial.print(waterlevel);
  Serial.println(" cm");

  if (readWaterLevel() < 0) {
    Serial.println("Please relocate the sensor");
    myBot.sendMessage(msg.sender.id, "Please relocate the sensor");  // notify the sender. can change with telegram group ID
  }
  else if (readWaterLevel() > alert_threshold) {
    Serial.println("Alert: Water level above normal threshold!");
    myBot.sendMessage(msg.sender.id, "Alert: Water level above normal threshold!");  // notify the sender
    String message = "Water Level: " + String(waterlevel) + " cm";
    myBot.sendMessage(msg.sender.id, message);  // notify the sender. can change with telegram group ID
  }

}


int readWaterLevel() {
  unsigned int distance = sonar.ping_cm(); // Send a ping and get the distance in centimeters
  unsigned int waterlevel = map(distance,mintank,maxtank,maxtank,mintank);
  
  return waterlevel;
}
