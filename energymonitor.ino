#define CC_RX 4
#define GREEN_LED 5
#define RED_LED 6

boolean dataReady=false;

void setup() {
    Serial.begin(57600);
    Serial.println(F("\n[energymonitor]"));

    currentCostSetup();

    //pin setups
    pinMode(RED_LED, OUTPUT);
    digitalWrite(RED_LED, HIGH);
    pinMode(GREEN_LED, OUTPUT);

    setupNetwork();
    digitalWrite(GREEN_LED, LOW);
    Serial.print(F("Completed setup in "));
    Serial.print(millis());
    Serial.println(F("ms"));
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
    // Print the error message.
    Serial.print(F("Error: "));
    Serial.println(message);
    // Turn off the green led, and on the red.
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    // Wait here.
    while(1);
}
