#include <SerialAPI.h>

SerialAPI api;

// SEND: "output -pin  2 -on \n" where 2 is the PIN number
// SEND: "output -pin 2 -off \n" where 2 is the PIN number
// SEND: "input -pin 2 \n" where 2 is the PIN number


void setup()
{
  
  Serial.begin(115200);
  Serial.println("start");
}

void loop()
{
    if(api.available())
    {
        if(api.command("output"))
        {
          Serial.println("output");
            if(api.has("pin"))
            {
                int pin = api.get("pin");

                if(api.has("off"))
                {
                    Serial.print("off: ");
                    Serial.println(pin);
                    pinMode(pin,OUTPUT);
                    digitalWrite(pin,LOW);
                }

                if(api.has("on"))
                {
                    Serial.print("on: ");
                    Serial.println(pin);
                    pinMode(pin,OUTPUT);
                    digitalWrite(pin,HIGH);
                }
            }
        }
        else if(api.command("input"))
        {
            if(api.has("pin"))
            {
                int pin = api.get("pin");
                pinMode(pin,INPUT);
                
                Serial.print("input ");
                Serial.print(pin);
                Serial.print(": ");
                Serial.println(digitalRead(pin));
            }
        }
        
        switch(api.error)
        {
            case UNKNOWN:
                break;

            case COMMANDNOTEXICUTED:
                break;

            case COM_NOT_EXIST:
                break;

            case PAR_NOT_EXIST:
                break;

            case MISSING_REQUIRED:
                break;

            case FORMAT_ERROR:
                break;
        }
    }
}