#include <EtherCard.h>
#include <NanodeMAC.h>

#include "keys.h"

static byte mymac[] = { 0,0,0,0,0,0 };

byte Ethernet::buffer[700];
Stash stash;
char website[] PROGMEM = "api.pachube.com";

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

    if (!ether.dnsLookup(website))
        critical(F("DNS failed"));
    ether.printIp(F("SRV: "), ether.hisip);
}

void uploadData()
{
    Serial.print(F("pushing to cosm: "));

    byte sd = stash.create();
    stash.print("0,");
    stash.println(temp);
    stash.print("1,");
    stash.println(elecW);
    stash.save();

    char c;
    while (c = stash.get()) {
        if (c != 10)
            Serial.print(c == 13 ? '\t' : c);
    }
    Serial.println();

    // generate the header with payload - note that the stash size is used,
    // and that a "stash descriptor" is passed in as argument using "$H"
    Stash::prepare(PSTR("PUT http://$F/v2/feeds/$F.csv HTTP/1.0" "\r\n"
                        "Host: $F" "\r\n"
                        "X-ApiKey: $F" "\r\n"
                        "Content-Length: $D" "\r\n"
                        "\r\n"
                        "$H"),
            website, PSTR(FEED), website, PSTR(APIKEY), stash.size(), sd);

    // send the packet - this also releases all stash buffers once done
    ether.tcpSend();
}

//call regularly to keep network going
void checkNetwork()
{
  // handle ping and wait for a tcp packet
  ether.packetLoop(ether.packetReceive());
}
