#include <EtherCard.h>
#include <NanodeMAC.h>

#include "keys.h"

static byte mymac[] = { 0,0,0,0,0,0 };
static byte session;
char statusline[13];

byte Ethernet::buffer[700];
Stash stash;
char website[] PROGMEM = "api.pachube.com";
bool awaiting_reply = false;

void setupNetwork() {
    NanodeMAC mac(mymac);
#if DEBUG
    Serial.print(F("MAC: "));
    for (byte i = 0; i < 6; ++i) {
        Serial.print(mymac[i], HEX);
        if (i < 5)
            Serial.print(':');
    }
    Serial.println();
#endif

    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
        critical(F("Failed to access Ethernet controller"));

#if DEBUG
    Serial.println(F("Setting up DHCP"));
#endif
    if (!ether.dhcpSetup())
        critical(F("DHCP failed"));

#if DEBUG
    ether.printIp(F("My IP: "), ether.myip);
    ether.printIp(F("Netmask: "), ether.mymask);
    ether.printIp(F("GW IP: "), ether.gwip);
    ether.printIp(F("DNS IP: "), ether.dnsip);
#endif

    if (!ether.dnsLookup(website))
        critical(F("DNS failed"));
#if DEBUG
    ether.printIp(F("SRV: "), ether.hisip);
#endif
}

void uploadData()
{
#if DEBUG
    Serial.print(F("pushing to cosm: "));
#endif

    byte sd = stash.create();
    stash.print("0,");
    stash.println(temp);
    stash.print("1,");
    stash.println(elecW);
    stash.save();

#if DEBUG
    char c;
    while (c = stash.get()) {
        if (c != 10)
            Serial.print(c == 13 ? '\t' : c);
    }
    Serial.println();
#endif

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
    session = ether.tcpSend();
    awaiting_reply = true;
}

//call regularly to keep network going
void checkNetwork()
{
  // handle ping and wait for a tcp packet
  ether.packetLoop(ether.packetReceive());
  if (awaiting_reply) {
    const char* reply = ether.tcpReply(session);
    if (reply != 0) {
      awaiting_reply = false;
      strncpy(statusline, reply, 13);
      String replyString = statusline;
      if (replyString.substring(9, 12) == "200") {
        digitalWrite(RED_LED, HIGH);
#if DEBUG
        Serial.println(F("200 response received"));
#endif
      }
      else {
        digitalWrite(RED_LED, LOW);
#if DEBUG
        Serial.print(F("Response received: "));
        Serial.println(reply);
#endif
      }
    }
  }
}
