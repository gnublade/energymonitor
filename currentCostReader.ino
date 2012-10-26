#include <SoftwareSerial.h>

SoftwareSerial ccSerial(CC_RX, NULL);

char startPwr[] = "<ch1><watts>";
char startTmpr[] = "<tmpr>";
char endChar = '<';

char readChar = 0xFF;

int sizePwr = sizeof(startPwr)-1;
int sizeTmpr = sizeof(startTmpr)-1;

int statePwr = sizePwr;
int stateTmpr = sizeTmpr;

int newstate = 0;

unsigned int PwrSize = 0;
unsigned int TmprSize = 0;
unsigned int DataSize = 0;

char Pwr[8] = "";
char Tmpr[8] = "";

//instantaneous energy values for gas and electricity
float elecW = 0;

double temp;

void currentCostSetup()
{
    // Set the data rate for the SoftwareSerial port
    pinMode(CC_RX, INPUT);
    ccSerial.begin(9600);
}

void readCurrentCost()
{
    if (!ccSerial.available())
        return;
    digitalWrite(GREEN_LED, HIGH);
    while ((readChar = ccSerial.read()) != -1)
    {
        if (ccSerial.overflow())
            digitalWrite(RED_LED, LOW);
        //Serial.print(readChar);
        if (readChar > 31) {
            stateTmpr = parseDataTmpr(stateTmpr, readChar);
            if (stateTmpr < 0) {
                Tmpr[abs(stateTmpr)-1] = readChar;
            }
            statePwr = parseDataPwr(statePwr, readChar);
            if (statePwr < 0) {
                Pwr[abs(statePwr)-1] = readChar;
            }
        } else if (readChar == 13) {
            //Serial.println( Pwr );
            elecW = atol(Pwr);
            temp = strtod(Tmpr,NULL);
            if (elecW && temp) {
#if DEBUG
                Serial.flush();
                Serial.print("Pwr: ");
                Serial.print(elecW);
                Serial.print(" ");
                Serial.print(" Tmpr: ");
                Serial.print(temp);
                Serial.println(".");
                Serial.flush();
#endif
                dataReady = true;
                digitalWrite(GREEN_LED, LOW);
                digitalWrite(RED_LED, HIGH);
            }

            memset(Pwr,255,sizeof(Pwr));
            memset(Tmpr,255,sizeof(Tmpr));
        }
    }
    //Serial.println();
}

int parseData2(int oldstate, int size, char *start, char chr) {
    newstate = oldstate;
    if (newstate > 0) {
        if (chr == start[size-newstate]) {
            newstate--;
        } else {
            newstate = size;
        }
    } else if (newstate <= 0) {
        newstate--;
        if (chr == endChar) {
            newstate = size;
        }
    }
    return newstate;
}

int parseDataTmpr(int oldstate, char chr) {
    newstate = oldstate;
    if (newstate > 0) {
        if (chr == startTmpr[sizeTmpr-newstate]) {
            newstate--;
        } else {
            newstate = sizeTmpr;
        }
    } else if (newstate <= 0) {
        newstate--;
        if (chr == endChar) {
            newstate = sizeTmpr;
        }
    }
    return newstate;
}
int parseDataPwr(int oldstate, char chr) {
    newstate = oldstate;
    if (newstate > 0) {
        if (chr == startPwr[sizePwr-newstate]) {
            newstate--;
        } else {
            newstate = sizePwr;
        }
    } else if (newstate <= 0) {
        newstate--;
        if (chr == endChar) {
            newstate = sizePwr;
        }
    }
    return newstate;
}
