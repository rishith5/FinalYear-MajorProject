#include <Wire.h>
#include <SoftwareSerial.h>
#include "adxl345.h"
ADXL345 accel;
const int buttonPin = 13;
const int buzzerPin = 27;
const int ledPin = 2;
SoftwareSerial gpsSerial(16, 17); // RX, TX
SoftwareSerial gsmSerial(5, 4);   // RX, TX
String contactNumber = "+919876543210";
void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  gsmSerial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  accel.begin();
  delay(1000);
  Serial.println("Smart Pendant System Active");
}
bool detectFall() {
  int16_t x, y, z;
  accel.read(x, y, z);
  int threshold = 250;
  return abs(x) > threshold || abs(y) > threshold || abs(z) > threshold;
}
String getGPSLocation() {
  if (gpsSerial.available()) {
    String line = gpsSerial.readStringUntil('\n');
    if (line.startsWith("$GPGGA")) {
      int comma1 = line.indexOf(',', 7);
      int comma2 = line.indexOf(',', comma1 + 1);
      int comma3 = line.indexOf(',', comma2 + 1);
      int comma4 = line.indexOf(',', comma3 + 1);
      String latRaw = line.substring(comma1 + 1, comma2);
      String lonRaw = line.substring(comma3 + 1, comma4);
      float lat = convertToDegrees(latRaw);
      float lon = convertToDegrees(lonRaw);
      return "https://maps.google.com/?q=" + String(lat, 6) + "," + String(lon, 6);
    }
  }
  return "";
}
float convertToDegrees(String rawVal) {
  if (rawVal == "") return 0.0;
  float val = rawVal.toFloat();
  int d = int(val / 100);
  float m = val - (d * 100);
  return d + m / 60.0;
}
void sendSMS(String msg) {
  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CMGF=1");
  delay(1000);
  gsmSerial.println("AT+CMGS=\"" + contactNumber + "\"");
  delay(1000);
  gsmSerial.print(msg);
  gsmSerial.write(26);  // Ctrl+Z to send
  delay(3000);
}
void loop() {
  if (digitalRead(buttonPin) == LOW || detectFall()) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    String loc = getGPSLocation();
    String msg = loc == "" ? "EMERGENCY! Location not available." : "EMERGENCY! Location: " + loc;
    sendSMS(msg);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
    delay(5000);
  }
}
