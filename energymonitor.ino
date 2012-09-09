#include <EtherCard.h>
#include <NanodeMAC.h>

#define GREEN_LED 5
#define RED_LED 6

static byte mymac[] = { 0,0,0,0,0,0 };

byte Ethernet::buffer[700];

void setup() {
    Serial.begin(57600);
    Serial.println(F("\n[energymonitor]"));

    //pin setups
    pinMode(RED_LED, OUTPUT);
    digitalWrite(RED_LED, HIGH);
    pinMode(GREEN_LED, OUTPUT);

    setupNetwork();
    digitalWrite(GREEN_LED, LOW);
    Serial.print(F("Completed setup in "));
    Serial.print(millis());
    Serial.print(F("ms"));
}

void setupNetwork() {
    NanodeMAC mac(mymac);
    Serial.print(F("MAC: "));
    for (byte i = 0; i < 6; ++i) {
        Serial.print(mymac[i], HEX);
        if (i < 5)
            Serial.print(':');
    }
    Serial.println();

    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
        critical(F("Failed to access Ethernet controller"));

    Serial.println(F("Setting up DHCP"));
    if (!ether.dhcpSetup())
        critical(F("DHCP failed"));

    ether.printIp(F("My IP: "), ether.myip);
    ether.printIp(F("Netmask: "), ether.mymask);
    ether.printIp(F("GW IP: "), ether.gwip);
    ether.printIp(F("DNS IP: "), ether.dnsip);
}

void loop() {}

void critical(__FlashStringHelper* message) {
    // Print the error message.
    Serial.print(F("Error: "));
    Serial.println(message);
    // Turn off the green led, and on the red.
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    // Wait here.
    while (true);
}
