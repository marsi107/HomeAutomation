#include <RTClib.h>        // Include the RTC library
#include <OneWire.h>       // Include the OneWire library
#include <DallasTemperature.h> // Include the DallasTemperature library

#define ONE_WIRE_BUS 2     // Pin for the OneWire bus
#define RELAY_PIN 3        // Pin for the relay control

RTC_DS3231 rtc;             // Create an instance of the RTC object
OneWire oneWire(ONE_WIRE_BUS);  // Create an instance of the OneWire object
DallasTemperature sensors(&oneWire);  // Create an instance of the DallasTemperature object

float desiredTemperature = 25.0;  // Default desired temperature in degrees Celsius
int heatingTimeOnHour = 8;        // Default heating ON time (hour in 24-hour format)
int heatingTimeOnMinute = 0;      // Default heating ON time (minute)
int heatingTimeOffHour = 18;      // Default heating OFF time (hour in 24-hour format)
int heatingTimeOffMinute = 0;     // Default heating OFF time (minute)

void setup() {
  pinMode(RELAY_PIN, OUTPUT);      // Set the relay pin as output
  
  Serial.begin(9600);              // Initialize serial communication
  sensors.begin();                 // Initialize the temperature sensors
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set the RTC to the current date and time
  }
}

void loop() {
  DateTime now = rtc.now();  // Get the current date and time
  
  float currentTemperature = readTemperature();  // Read the current temperature

  if (now.hour() >= heatingTimeOnHour && now.minute() >= heatingTimeOnMinute &&
      now.hour() < heatingTimeOffHour && now.minute() < heatingTimeOffMinute) {
    if (currentTemperature < desiredTemperature) {
      // Turn on the heating
      digitalWrite(RELAY_PIN, HIGH);
    } else {
      // Turn off the heating
      digitalWrite(RELAY_PIN, LOW);
    }
  } else {
    // Turn off the heating outside the scheduled time
    digitalWrite(RELAY_PIN, LOW);
  }

  // Display temperature and heating status
  Serial.print("Current Temperature: ");
  Serial.print(currentTemperature);
  Serial.print("°C\tHeating: ");
  Serial.println(digitalRead(RELAY_PIN) ? "ON" : "OFF");

  delay(1000);  // Delay for stability
}

float readTemperature() {
  sensors.requestTemperatures();  // Request temperature readings
  float temperature = sensors.getTempCByIndex(0);  // Read the first temperature sensor
  return temperature;
}
