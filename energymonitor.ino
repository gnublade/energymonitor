#include <EtherCard.h>
#include <NanodeMAC.h>

static byte mymac[] = { 0,0,0,0,0,0 };

byte Ethernet::buffer[700];

void setup() {
    Serial.begin(57600);
    Serial.println("\n[energymonitor]");

    setup_network();
}

void setup_network() {
    NanodeMAC mac(mymac);
    Serial.print("MAC: ");
    for (byte i = 0; i < 6; ++i) {
        Serial.print(mymac[i], HEX);
        if (i < 5)
            Serial.print(':');
    }
    Serial.println();
    
    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
        Serial.println( "Failed to access Ethernet controller");

    Serial.println("Setting up DHCP");
    if (!ether.dhcpSetup())
        Serial.println( "DHCP failed");
    
    ether.printIp("My IP: ", ether.myip);
    ether.printIp("Netmask: ", ether.mymask);
    ether.printIp("GW IP: ", ether.gwip);
    ether.printIp("DNS IP: ", ether.dnsip);
}  

void loop() {}
