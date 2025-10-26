#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ----------------- CONFIGURATION -----------------
#define DHTPIN 2          // DHT11 sensor connected to digital pin 2
#define DHTTYPE DHT11     // Sensor type: DHT11
#define SOIL_PIN A0       // Soil moisture sensor analog pin
#define RELAY_PIN 7       // Relay connected to digital pin 7

// ----------------- OBJECTS -----------------
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x3F, 16, 2); // LCD address 0x3F (try 0x27 if not working)

// ----------------- SETTINGS -----------------
int soilRaw = 0;
int soilPercent = 0;
const int soilThreshold = 40; // Soil dryness threshold (%)

// ----------------- SETUP -----------------
void setup() {
  Serial.begin(9600);

  // Initialize sensors and LCD
  dht.begin();
  lcd.init();
  lcd.backlight();

  // Setup relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Relay OFF initially (active HIGH)

  // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("SOIL & TEMP SYS");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

// ----------------- MAIN LOOP -----------------
void loop() {
  // --- Read sensors ---
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius
  soilRaw = analogRead(SOIL_PIN);

  // Convert raw soil reading to percentage
  // 0 = wet, 1023 = dry → inverted mapping
  soilPercent = map(soilRaw, 1023, 0, 0, 100);

  // --- Display Temperature & Humidity ---
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print((char)223); // Degree symbol
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("%");

  // --- Display Soil Moisture & Control Pump ---
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(soilPercent);
  lcd.print("% ");

  if (soilPercent > soilThreshold) {
    digitalWrite(RELAY_PIN, LOW);  // Soil wet → Pump OFF
    lcd.print("Pump:OFF");
  } else {
    digitalWrite(RELAY_PIN, HIGH); // Soil dry → Pump ON
    lcd.print("Pump:ON ");
  }

  // --- Debugging: Serial Output ---
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" C, Hum: "); Serial.print(humidity);
  Serial.print("%, SoilRaw: "); Serial.print(soilRaw);
  Serial.print(", Soil%: "); Serial.print(soilPercent);
  Serial.print(", Relay: "); Serial.println(digitalRead(RELAY_PIN));

  delay(2000); // Wait before next reading
}
