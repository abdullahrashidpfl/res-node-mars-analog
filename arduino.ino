#include <DHT.h>

// --- ESP32 Pin Definitions ---
#define DHTPIN 4          // DHT11 Data pin connected to GPIO 4 (D4)
#define DHTTYPE DHT11     // Sensor type: DHT11
#define SOIL_PIN 34       // DIY Soil Moisture Voltage Divider connected to GPIO 34 (D34)

DHT dht(DHTPIN, DHTTYPE);

// --- NASA Mars Reference Benchmarks ---
// Source 1: NASA Goddard Mars Fact Sheet (nssdc.gsfc.nasa.gov)
const float MARS_MEAN_TEMP_C     = -59.0;  // Global mean surface temp (°C)
const float MARS_DIURNAL_MAX_C   = -31.0;  // Typical diurnal high (°C)

// Source 2: NASA MSL Curiosity Rover REMS Data (Gale Crater, Mars)
const float CURIOSITY_DAY_PEAK_C =   5.0;  // Warm equatorial afternoon surface peak (°C)
const float MARS_DAY_HUMIDITY    =   2.0;  // Hyper-arid daytime relative humidity (%)
const float MARS_SOIL_MOISTURE   =   1.5;  // Regolith water content (<2% hyper-arid baseline)

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // ESP32 12-bit ADC (0 - 4095)
  dht.begin();
  delay(2000);
  Serial.println(F("=========================================================="));
  Serial.println(F("   ARES-NODE ESP32: TERRESTRIAL ANALOG TELEMETRY PROBE    "));
  Serial.println(F("   Benchmarking Local Site vs. NASA Mars Open Datasets    "));
  Serial.println(F("=========================================================="));
}

void loop() {
  // 1. Read DHT11 Atmospheric Temperature & Humidity
  float localHumidity = dht.readHumidity();
  float localTempC    = dht.readTemperature();

  // 2. Read DIY Soil Moisture Sensor (Averaged over 10 samples)
  long rawSum = 0;
  for (int i = 0; i < 10; i++) {
    rawSum += analogRead(SOIL_PIN);
    delay(10);
  }
  int soilRaw = rawSum / 10;

  // ESP32 12-bit ADC: 0 = bone dry air/sand, ~2800 = wet soil/water
  float localSoilPct = constrain(map(soilRaw, 0, 2800, 0, 100), 0, 100);

  // Check if DHT11 read failed
  if (isnan(localHumidity) || isnan(localTempC)) {
    Serial.println(F("[ERROR] Failed to read from DHT11! Check GPIO 4 (D4) wiring."));
    delay(2000);
    return;
  }

  // 3. Calculate Mars Terrestrial Analog Similarity Scores (0% to 100%)
  float soilDelta = abs(localSoilPct - MARS_SOIL_MOISTURE);
  float soilSimilarity = constrain(100.0 - soilDelta, 0.0, 100.0);

  float humDelta = abs(localHumidity - MARS_DAY_HUMIDITY);
  float humSimilarity = constrain(100.0 - humDelta, 0.0, 100.0);

  float tempDelta = abs(localTempC - CURIOSITY_DAY_PEAK_C);
  float tempSimilarity = constrain(100.0 - (tempDelta * 1.5), 0.0, 100.0);

  // Weighted Overall Analog Suitability Index (Soil Aridity 45%, Air Dryness 35%, Thermal 20%)
  float overallAnalogScore = (soilSimilarity * 0.45) + (humSimilarity * 0.35) + (tempSimilarity * 0.20);

  // 4. Output Structured Telemetry & NASA Dataset Comparison
  Serial.println(F("\n----------------------------------------------------------"));
  Serial.println(F("PARAMETER        | LOCAL PROBE  | NASA MARS BENCHMARK | MATCH"));
  Serial.println(F("----------------------------------------------------------"));
  
  Serial.print(F("Temperature (C)  | "));
  Serial.print(localTempC, 1);
  Serial.print(F(" C       | +5.0 C (Gale Peak)  | "));
  Serial.print(tempSimilarity, 1);
  Serial.println(F("%"));

  Serial.print(F("Air Humidity (%) | "));
  Serial.print(localHumidity, 1);
  Serial.print(F(" %       | ~2.0 % (REMS Day)   | "));
  Serial.print(humSimilarity, 1);
  Serial.println(F("%"));

  Serial.print(F("Soil Moisture(%) | "));
  Serial.print(localSoilPct, 1);
  Serial.print(F(" %       | <2.0 % (Regolith)   | "));
  Serial.print(soilSimilarity, 1);
  Serial.println(F("%"));

  Serial.println(F("----------------------------------------------------------"));
  Serial.print(F(">> OVERALL MARS ANALOG SUITABILITY SCORE: "));
  Serial.print(overallAnalogScore, 1);
  Serial.println(F("%"));

  if (overallAnalogScore >= 75.0) {
    Serial.println(F(">> STATUS: HIGH-FIDELITY ANALOG (Ideal for Rover/Habitat Testing)"));
  } else if (overallAnalogScore >= 50.0) {
    Serial.println(F(">> STATUS: MODERATE ANALOG (Suitable for Arid Regolith Trials)"));
  } else {
    Serial.println(F(">> STATUS: LOW ANALOG (Excess Moisture/Humidity vs. Martian Baseline)"));
  }
  Serial.println(F("----------------------------------------------------------"));

  delay(3000);
}
