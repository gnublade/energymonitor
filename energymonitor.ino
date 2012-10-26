#define GREEN_LED 5
#define RED_LED 6

#define DEBUG  0
#define SERIAL 1

#if SERIAL
#define CC_RX 0
#else
#define CC_RX 4
#endif

boolean dataReady=false;

void setup() {
#if DEBUG
    Serial.begin(57600);
    Serial.println(F("\n[energymonitor]"));
#endif

    currentCostSetup();

    //pin setups
    pinMode(RED_LED, OUTPUT);
    digitalWrite(RED_LED, HIGH);
    pinMode(GREEN_LED, OUTPUT);

    setupNetwork();
    digitalWrite(GREEN_LED, LOW);
#if DEBUG
    Serial.print(F("Completed setup in "));
    Serial.print(millis());
    Serial.println(F("ms"));
#endif
}

void loop() {
    checkNetwork();
    readCurrentCost();
    if (dataReady) {
        dataReady = false;
        uploadData();
    }
}

void critical(__FlashStringHelper* message) {
#if DEBUG
    // Print the error message.
    Serial.print(F("Error: "));
    Serial.println(message);
#endif
    // Turn off the green led, and on the red.
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    // Wait here.
    while(1);
}
