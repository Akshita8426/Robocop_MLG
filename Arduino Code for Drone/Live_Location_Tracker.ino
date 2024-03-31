#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
  
  // Call the GPS calibration function
  GPS_calibration();
}

void GPS_calibration() {
  // Keep reading GPS data until we have valid location data
  while (!gps.location.isValid()) {
    smartDelay(1000);
    if (millis() > 5000 && gps.charsProcessed() < 10) {
      Serial.println(F("No GPS data received: check wiring"));
      return; // Exit function if no valid GPS data after 5 seconds
    }
  }

  // Print latitude and longitude once
  Serial.print("Latitude: ");
  Serial.print(gps.location.lat(), 6);
  Serial.print(", Longitude: ");
  Serial.println(gps.location.lng(), 6);

  // Enter an infinite loop to prevent further execution
  while (true) {
    // Do nothing, just stay in this loop indefinitely
  }
}

void loop() {
  // Empty loop as all functionality is contained within the GPS_calibration function
}

static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
