#include <LiquidCrystal_I2C.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>

const byte interruptPin = 2;

RtcDS3231<TwoWire> Rtc(Wire);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

String line1;
String line2;

String oldLine1 = "";
String oldLine2 = "";

long randomNumber;

bool showReadable = true;

RtcDateTime lastUpdate;

// handy routine to return true if there was an error
// but it will also print out an error message with the given topic
bool wasError(const char* errorTopic = "")
{
    uint8_t error = Rtc.LastError();
    if (error != 0)
    {
        // we have a communications error
        // see https://www.arduino.cc/reference/en/language/functions/communication/wire/endtransmission/
        // for what the number means
        // Serial.print("[");
        // Serial.print(errorTopic);
        // Serial.print("] WIRE communications error (");
        // Serial.print(error);
        // Serial.print(") : ");

        switch (error)
        {
        case Rtc_Wire_Error_None:
            // Serial.println("(none?!)");
            break;
        case Rtc_Wire_Error_TxBufferOverflow:
            // Serial.println("transmit buffer overflow");
            break;
        case Rtc_Wire_Error_NoAddressableDevice:
            // Serial.println("no device responded");
            break;
        case Rtc_Wire_Error_UnsupportedRequest:
            // Serial.println("device doesn't support request");
            break;
        case Rtc_Wire_Error_Unspecific:
            // Serial.println("unspecified error");
            break;
        case Rtc_Wire_Error_CommunicationTimeout:
            // Serial.println("communications timed out");
            break;
        }
        return true;
    }
    return false;
}

void changeDisplay ()
{
  showReadable = ! showReadable;
}

void setup () 
{
    // initialize random seed
    randomSeed(analogRead(0));
    
    // initialize the lcd 
    lcd.init();                      
    lcd.backlight();
    
    lcd.setCursor(0,0);
    lcd.print("Initializing...");
    lcd.setCursor(0,1);
    lcd.print("Please wait!");
    
    // initialize sierial
    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(interruptPin), changeDisplay, FALLING);

    Rtc.Begin();
#if defined(WIRE_HAS_TIMEOUT)
    Wire.setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
#endif

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    // printDateTime(compiled);
    // Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        if (!wasError("setup IsDateTimeValid"))
        {
            // Common Causes:
            //    1) first time you ran and the device wasn't running yet
            //    2) the battery on the device is low or even missing

            //Serial.println("RTC lost confidence in the DateTime!");

            // following line sets the RTC to the date & time this sketch was compiled
            // it will also reset the valid flag internally unless the Rtc device is
            // having an issue

            Rtc.SetDateTime(compiled);
        }
    }

    if (!Rtc.GetIsRunning())
    {
        if (!wasError("setup GetIsRunning"))
        {
            // Serial.println("RTC was not actively running, starting now");
            Rtc.SetIsRunning(true);
        }
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (!wasError("setup GetDateTime"))
    {
        if (now < compiled)
        {
            // Serial.println("RTC is older than compile time, updating DateTime");
            Rtc.SetDateTime(compiled);
        }
        else if (now > compiled)
        {
            // Serial.println("RTC is newer than compile time, this is expected");
        }
        else if (now == compiled)
        {
            // Serial.println("RTC is the same as compile time, while not expected all is still fine");
        }
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    wasError("setup Enable32kHzPin");
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
    wasError("setup SetSquareWavePin");

}

void loop () 
{

    if (!Rtc.IsDateTimeValid()) 
    {
        if (!wasError("loop IsDateTimeValid"))
        {
            // Common Causes:
            //    1) the battery on the device is low or even missing and the power line was disconnected
            // Serial.println("RTC lost confidence in the DateTime!");
            line1 = "RTC error!";
        }
    } else 
    {
      RtcDateTime now = Rtc.GetDateTime();

      if (now != lastUpdate) 
      {
        lastUpdate = now;

        if (showReadable)
        {
          setNormalTime(now);
        } else
        {
          if (now.Second() % 15 == 0)
          {
            setTimeCalc(now);
          }
        }

        if ((line1 != oldLine1) || (line2 != oldLine2)) 
        {
            oldLine1 = line1;
            oldLine2 = line2;

            // Serial.println(line1);

            lcd.clear();

            lcd.setCursor(0, 0);
            lcd.print(line1);

            lcd.setCursor(0, 1);
            lcd.print(line2);
        }
      }
    }
    // delay(1000); // one seconds
}

void setNormalTime(RtcDateTime now)
{
  char timeString[16];
  char datestring[16];


  snprintf_P(timeString, 
          countof(timeString),
          PSTR("%02u:%02u:%02u"),
          now.Hour(),
          now.Minute(),
          now.Second() );

  snprintf_P(datestring, 
          countof(datestring),
          PSTR("%04u-%02u-%02u"),
          now.Year(),
          now.Month(),
          now.Day() );

  line1 = String (timeString);
  line2 = String (datestring);
}

void setTimeCalc(RtcDateTime now)
{
  randomNumber = random(2);

  //Serial.println(String(randomNumber));
  if (randomNumber == 0)
  {
    // Serial.println("Random 0");
    set2NumberCalc(now.Hour(), line1);
    set3NumberCalc(now.Minute(), line2); 
  } else
  {
    // Serial.println("Random 1");
    set3NumberCalc(now.Hour(), line1);
    set2NumberCalc(now.Minute(), line2);
  }
}

void set3NumberCalc(const int targetNumber, const String& line)
{
  int totalFound = 0;
  int result;

  for (int i = 0; i <= 9; i ++) 
    for (int j = 0; j <= 9; j ++)
      for (int k = 0; k<= 9; k ++)
      {
        // + +
        result = i + j + k;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // + -
        result = i + j - k;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // + *
        result = i + j * k;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // * +
        result = i * j + k;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // * -
        result = i * j - k;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // - +
        if (i >= j)
        {
          result = i - j + k;
          if (result == targetNumber)
          {
            totalFound ++;
          }
        }

        // - -
        if (i >= j)
        {
          result = i - j - k;
          if (result == targetNumber)
          {
            totalFound ++;
          }
        }

        // - *
        result = i - j * k;
        if (result == targetNumber)
        {
          totalFound ++;
        }
      }

  randomNumber = random(totalFound);

  /*
  Serial.println("Target:");
  Serial.println(targetNumber);
  Serial.println("Total found:");
  Serial.println(totalFound);
  Serial.println("Random:");
  Serial.println(randomNumber);
  */

  totalFound = 0;

  for (int i = 0; i <= 9; i ++) 
    for (int j = 0; j <= 9; j ++)
      for (int k = 0; k<= 9; k ++)
      {
        // + +
        result = i + j + k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " + " + String(j) + " + " + String(k);
            return;
          }
        }

        // + -
        result = i + j - k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " + " + String(j) + " - " + String(k);
            return;
          }
        }

        // + *
        result = i + j * k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " + " + String(j) + " x " + String(k);
            return;
          }
        }

        // * +
        result = i * j + k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " x " + String(j) + " + " + String(k);
            return;
          }
        }

        // * -
        result = i * j - k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " x " + String(j) + " - " + String(k);
            return;
          }
        }

        // - +
        if (i >= j)
        {
          result = i - j + k;
          if (result == targetNumber)
          {
            totalFound ++;
            if (totalFound > randomNumber)
            {
              line = String(i) + " - " + String(j) + " + " + String(k);
              return;
            }
          }
        }

        // - -
        if (i >= j)
        {
          result = i - j - k;
          if (result == targetNumber)
          {
            totalFound ++;
            if (totalFound > randomNumber)
            {
              line = String(i) + " - " + String(j) + " - " + String(k);
              return;
            }
          }
        }

        // - *
        result = i - j * k;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " - " + String(j) + " x " + String(k);
            return;
          }
        }
      }
}

void set2NumberCalc(const int targetNumber, const String& line)
{
  int totalFound = 0;
  int result;

  for (int i = 0; i <= 58; i ++)
    for (int j = 0; j<= 58; j ++)
    {
      if ((i <= 9) || (j <= 9))
      {
        // +
        result = i + j;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // -
        result = i - j;
        if (result == targetNumber)
        {
          totalFound ++;
        }

        // *
        result = i * j;
        if (result == targetNumber)
        {
          totalFound ++;
        }
      }
    }

  randomNumber = random(totalFound);

  /*
  Serial.println("Target:");
  Serial.println(targetNumber);
  Serial.println("Total found:");
  Serial.println(totalFound);
  Serial.println("Random:");
  Serial.println(randomNumber);
  */

  totalFound = 0;

  for (int i = 0; i <= 68; i ++)
    for (int j = 0; j<= 68; j ++)
    {
      if ((i <= 9) || (j <= 9))
      {
        // +
        result = i + j;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " + " + String(j);
            return;
          }
        }

        // + -
        result = i - j;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " - " + String(j);
            return;
          }
        }

        // *
        result = i * j;
        if (result == targetNumber)
        {
          totalFound ++;
          if (totalFound > randomNumber)
          {
            line = String(i) + " x " + String(j);
            return;
          }
        }
      }
    }
}
