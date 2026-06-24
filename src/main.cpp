#include <Arduino.h>

constexpr uint8_t RELAY = 4;
constexpr uint8_t FEEDBACK = 18;
constexpr uint8_t NUMBER_OF_MEASUREMENTS = 10;

volatile bool relayClosed = false;
volatile uint32_t triggerTime = 0;

uint32_t startTime = 0;
uint32_t totalDelay = 0;
uint8_t count = 0;
bool measurementInProgress = false;

void prepareMeasurement();
void printMeasurementResults();
void printResults();

void IRAM_ATTR relaySwitch() {
  triggerTime = millis();
  relayClosed = true;
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);

  pinMode(FEEDBACK, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(FEEDBACK), relaySwitch, FALLING);
}

void loop() {
  if (!measurementInProgress) {
    prepareMeasurement();
  }

  if (measurementInProgress && relayClosed) {
    printMeasurementResults();

    if (count == NUMBER_OF_MEASUREMENTS) {
      printResults();
    }
  }

  delay(1000);
}

void prepareMeasurement() {
    relayClosed = false;
    while (digitalRead(FEEDBACK) == LOW) {
      // wait until contact is open
    }

    startTime = millis();
    digitalWrite(RELAY, HIGH);
    measurementInProgress = true;
}

void printMeasurementResults() {
    uint32_t delayMs = triggerTime - startTime;

    Serial.printf(
        "Measurement %d: %lu ms\n",
        count + 1,
        delayMs
    );

    totalDelay += delayMs;
    count++;

    digitalWrite(RELAY, LOW);
    measurementInProgress = false;
}

void printResults() {
  float average = totalDelay / (float)NUMBER_OF_MEASUREMENTS;

  Serial.printf("Average delay = %.2f ms\n", average);
  Serial.println();
  totalDelay = 0;
  count = 0;
}
